//@HEADER
/*
*******************************************************************************

    Copyright (C) 2004, 2005, 2007 EPFL, Politecnico di Milano, INRIA
    Copyright (C) 2010 EPFL, Politecnico di Milano, Emory University

    This file is part of LifeV.

    LifeV is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    LifeV is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with LifeV.  If not, see <http://www.gnu.org/licenses/>.

*******************************************************************************
*/
//@HEADER

/*!
    @file main_benchmark.cpp
    @brief Time-dependent FSI problem with XFEM
    Simulation in FULL blood pump domain, with two structures FG1 - membrane, FG2 - rigid magnet ring 
    NS semi-implicit/implicit + Hooke model + implicit geometry
    ONLY FOR BDF1
    RESTART

    @author Marco Martinolli <marco.martinolli@polimi.it>
    @date 05-05-2021
 */

#include <Epetra_ConfigDefs.h>
#ifdef EPETRA_MPI
#include <mpi.h>
#include <Epetra_MpiComm.h>
#else
#include <Epetra_SerialComm.h>
#endif

#include <lifev/core/LifeV.hpp>
#include <lifev/core/util/LifeTraits.hpp>
#include <lifev/core/util/LifeChrono.hpp>
#include <lifev/core/util/ConditionalOstream.hpp>

#include <lifev/core/mesh/MeshPartitioner.hpp>
#include <lifev/core/mesh/MeshPartitionerIntersection.hpp>
#include <lifev/core/mesh/RegionMesh3DStructured.hpp>
#include <lifev/core/mesh/RegionMesh.hpp>
#include <lifev/core/array/MatrixEpetra.hpp>

#include <lifev/core/array/VectorEpetraStructured.hpp>
#include <lifev/core/array/MatrixEpetraStructured.hpp>
#include <lifev/core/array/MatrixEpetraStructuredView.hpp>
#include <lifev/core/array/MatrixEpetraStructuredUtility.hpp>

#include <lifev/core/fem/BCManage.hpp>
#include <lifev/core/fem/FESpaceInterpolation.hpp>
#include <lifev/eta/fem/ETFESpace.hpp>
#include <lifev/eta/expression/BuildGraph.hpp>
#include <lifev/eta/expression/Integrate.hpp>
#include <lifev/eta/expression/InteriorIntegrate.hpp>
#include <lifev/eta/expression/InterfaceIntegrate.hpp>
#include <lifev/eta/expression/Evaluate.hpp>
#include <Epetra_FECrsGraph.h>

#include <lifev/xfem/eta/fem/ExpressionDefinitionsMCI.hpp>
#include <lifev/xfem/eta/fem/Materials.hpp>
#include <lifev/xfem/eta/fem/CouplingTypes.hpp>

#include <Teuchos_ParameterList.hpp>
#include <Teuchos_XMLParameterListHelpers.hpp>
#include <Teuchos_RCP.hpp>
#include <lifev/core/algorithm/LinearSolver.hpp>
#include <lifev/xfem/core/algorithm/UnfittedMonolithicDN.hpp>

#include <lifev/core/filter/ExporterHDF5.hpp>
#include <lifev/core/filter/ExporterHDF5XFEM.hpp>

#include <lifev/core/mesh/RegionMesh.hpp>
#include <lifev/core/mesh/ElementShapes.hpp>
#include <lifev/core/mesh/MeshEntityContainer.hpp>
#include <lifev/core/mesh/MeshData.hpp>
#include <lifev/core/mesh/MeshIntersection.hpp>
#include <lifev/core/mesh/CutElements.hpp>
#include <lifev/core/mesh/Contact.hpp>

#include <lifev/xfem/core/mesh/UnfitMeshUtility.hpp>

#include <lifev/core/dataStructure/ADTree.hpp>
#include <lifev/core/dataStructure/Domain.hpp>

#include <lifev/core/fem/DOFExtended.hpp>

#include <lifev/core/fem/TimeData.hpp>
#include <lifev/core/fem/TimeAdvanceData.hpp>
#include <lifev/core/fem/TimeAdvanceNewmark.hpp>
#include <lifev/core/fem/TimeAdvanceBDF.hpp>

#include <chrono>
#include <thread>

// Define the following macro to enable the CIP
#define ENABLE_CIP

// Define the following macro to enable the Backflow stabilization
// #define ENABLE_BACKFLOW_STABILIZATION

// Define the following macro to enable the
// symmetric terms of the DG mortaring
#define ENABLE_SYMM_TERMS

using namespace LifeV;

using mesh_Type = RegionMesh<LinearTetra, LinearTetraMCI_Type>;
using meshPtr_Type = std::shared_ptr< mesh_Type >;
using interPointsPtr_Type = std::shared_ptr< IntersectionPoints >;

using map_Type = MapEpetra;
using mapPtr_Type = std::shared_ptr< map_Type >;

using dSpace_Type = FESpace< mesh_Type, map_Type>;
using dSpacePtr_Type = std::shared_ptr< dSpace_Type >;
using dSpaceETA_Type = ETFESpace< mesh_Type, map_Type, 3, 3 >;
using dSpaceETAPtr_Type = std::shared_ptr< dSpaceETA_Type >;

using sSpace_Type = FESpace< mesh_Type, map_Type>;
using sSpacePtr_Type = std::shared_ptr< sSpace_Type >;
using sSpaceETA_Type = ETFESpace< mesh_Type, map_Type, 3, 1 >;
using sSpaceETAPtr_Type = std::shared_ptr< sSpaceETA_Type >;

using uXSpace_Type = FESpace< mesh_Type, map_Type, DOFXfem>;
using uXSpacePtr_Type = std::shared_ptr< uXSpace_Type >;
using uXSpaceETA_Type = ETFESpace< mesh_Type, map_Type, 3, 3, DOFXfem >;
using uXSpaceETAPtr_Type = std::shared_ptr< uXSpaceETA_Type >;

using uExportSpace_Type = FESpace< mesh_Type, map_Type , DOFXfemExport >;
using uExportSpacePtr_Type = std::shared_ptr< uExportSpace_Type >;

using pXSpace_Type = FESpace< mesh_Type, map_Type, DOFXfem>;
using pXSpacePtr_Type = std::shared_ptr< pXSpace_Type >;
using pXSpaceETA_Type = ETFESpace< mesh_Type, map_Type, 3, 1, DOFXfem >;
using pXSpaceETAPtr_Type = std::shared_ptr< pXSpaceETA_Type >;

using pExportSpace_Type = FESpace< mesh_Type, map_Type , DOFXfemExport >;
using pExportSpacePtr_Type = std::shared_ptr< pExportSpace_Type >;

using graph_Type = Epetra_FECrsGraph;
using graphPtr_Type = std::shared_ptr< Epetra_FECrsGraph >;
using matrix_Type = MatrixEpetra< Real >;
using matrixPtr_Type = std::shared_ptr< MatrixEpetra< Real > >;
using vector_Type = VectorEpetra;
using vectorPtr_Type = std::shared_ptr< VectorEpetra >;
using blockMatrix_Type = MatrixEpetraStructured< Real >;
using blockMatrixPtr_Type = std::shared_ptr< blockMatrix_Type >;
using blockVector_Type = VectorEpetraStructured;
using blockVectorPtr_Type = std::shared_ptr< blockVector_Type >;
using matrixBlockView_Type = MatrixEpetraStructuredView<Real>;

using matrixSmall_Type = MatrixSmall<3, 3>;
using basePrec_Type = LifeV::Preconditioner;
using basePrecPtr_Type = std::shared_ptr<basePrec_Type>;
using prec_Type = UnfittedMonolithicDN;

using timeData_Type = TimeData;
using timeDataPtr_Type = std::shared_ptr< timeData_Type >;
using timeAdvanceData_Type = TimeAdvanceData;
using timeAdvanceDataPtr_Type = std::shared_ptr< timeAdvanceData_Type >;

using timeAdvance_Type = TimeAdvance<vector_Type>;
using timeAdvancePtr_Type = std::shared_ptr< timeAdvance_Type >;
using stencil_Type = timeAdvance_Type::feVectorContainerPtr_Type;

using constitutiveLaw_Type = Hooke;
using fiberFunction_Type = constitutiveLaw_Type::fiberFunction_Type;
using fiberFunctionPtr_Type = constitutiveLaw_Type::fiberFunctionPtr_Type;

using contact_Type = ContactCone;
using couplingFSI_Type = NoSlip;
using couplingFSIContact_Type = NoSlip;

#define PI 3.14159265

#include "BCSettings.hpp"

// Fluid Boundaries identifiers
unsigned int INLET = 1;
unsigned int OUTLET = 2;

std::set<unsigned int> WALLS = {3,4};

// Solid1 Boundary identifiers: MEMBRANE
unsigned int INTERFACE = 30;    // free portion of the interface
unsigned int TITANIUM = 40;     // constrained portion of the interface (where osciallation is imposed)

// Solid2 Boundary identifiers
unsigned int MAGNET_INTERFACE = 200;    // magnet ring surface



int main ( int argc, char** argv )
{
    // MPI initialization
#ifdef HAVE_MPI
    MPI_Init (&argc, &argv);
    std::shared_ptr<Epetra_Comm> Comm (new Epetra_MpiComm (MPI_COMM_WORLD) );
    int nproc;
    MPI_Comm_size (MPI_COMM_WORLD, &nproc);
#else
    std::shared_ptr< Epetra_Comm > Comm ( new Epetra_SerialComm );
#endif

    const UInt myPID = Comm->MyPID();
    const bool verbose (myPID == 0);

    conditional_ostream vout(std::cout);
    conditional_ostream eout(std::cerr);

    if (!verbose)
    {
        vout.deactivate();
        eout.deactivate();
    }

#ifdef HAVE_MPI
    vout << "Using MPI (" << nproc << " proc.)" << std::endl;
#else
    vout << "Using serial version" << std::endl;
#endif

    // Initializing chronometers
    LifeChrono globalChrono;
    LifeChrono initChrono;
    LifeChrono timeChrono;
    LifeChrono nlChrono;
    LifeChrono localChrono;

    globalChrono.start();
    initChrono.start();

    std::stringstream ss;

    vout << " -- Reading the data ... " << std::flush;

    // Reading parameters through GetPot
    GetPot command_line ( argc, argv );
    const std::string dataFileName = command_line.follow ( "dataFile", 2, "-f", "--file" );
    GetPot dataFile ( dataFileName );

    const Real rhoF             = dataFile ( "physics/fluid/rhoF", 1. );
    const Real mu               = dataFile ( "physics/fluid/mu", 0.035 );

    const Real rhoS             = dataFile ( "physics/solid/rhoS", 0.1 );
    const Real YoungModulus     = dataFile ( "physics/solid/youngModulus", 2.e7 );
    const Real PoissonRatio     = dataFile ( "physics/solid/poissonRatio", 0.45 );
    Real LameI                  = 1.;
    Real LameII                 = 1.;
    Real ShearModulus           = 1.;
    Real BulkModulus            = 1.;
    const Real solidGamma       = 1.;
    const bool fibersEnabled    = 0 ;
    const Real stiffnessF       = 0.;
    const Real nonlinearF       = 0.;
    const Real stretchF         = 0.;
    const Real smoothnessF      = 0.;

    if ( (YoungModulus != -1) && (PoissonRatio != -1) )
    {
        LameI  = YoungModulus * PoissonRatio /
                 ( ( 1 + PoissonRatio ) * ( 1 - 2 * PoissonRatio ) );

        LameII = YoungModulus / ( 2 * ( 1 + PoissonRatio ) );

        ShearModulus = LameII;

        BulkModulus  = YoungModulus / ( 3 * ( 1 - 2 * PoissonRatio ) );
    }

    const Real rhoS2             = dataFile ( "physics/solid2/rhoS2", 0.1 );
    const Real YoungModulus2     = dataFile ( "physics/solid2/youngModulus2", 2.e7 );
    const Real PoissonRatio2     = dataFile ( "physics/solid2/poissonRatio2", 0.45 );
    Real LameI2                  = 1.;
    Real LameII2                 = 1.;
    Real BulkModulus2            = 1.;

    if ( (YoungModulus2 != -1) && (PoissonRatio2 != -1) )
    {
        LameI2  = YoungModulus2 * PoissonRatio2 /
                 ( ( 1 + PoissonRatio2 ) * ( 1 - 2 * PoissonRatio2 ) );

        LameII2 = YoungModulus2 / ( 2 * ( 1 + PoissonRatio2 ) );

        BulkModulus2  = YoungModulus2 / ( 3 * ( 1 - 2 * PoissonRatio2 ) );
    }

    const Real perturbCoeff     = 1e-7;

    const Real alpha_inter      = 1.;
    const Real gamma_virt       = 1.e3;
    const Real gamma_inter      = 1.e6;
    const Real gamma_ghost      = 1;
    const Real gamma_v1         = dataFile ( "penalty/gamma_v1", 5.e-2 );
    const Real gamma_v2         = dataFile ( "penalty/gamma_v2", 5.e-1 );
    const Real gamma_p          = dataFile ( "penalty/gamma_p", 5.e-2 );

    const bool movementEnabled  = 1;
    const bool nlConvEnabled    = 1;
    const Real nlMaxIter        = 1;
    const std::string nlMethod  = "static";
    const Real convToll         = 1.e-2;
    const Real kinToll          = 1.e-2;
    const Real dispToll         = 1.e-4 ;
    const Real relaxSolSolid    = 1.;
    const Real mLin             = 0.2;

    const bool nonLinear = ( nlMaxIter > 1 );

    vout << "-------------------- " << std::endl;
    vout << "rhoF               = " << rhoF         << std::endl;
    vout << "mu                 = " << mu           << std::endl;
    vout << "rhoS               = " << rhoS         << std::endl;
    vout << "YoungModulus       = " << YoungModulus << std::endl;
    vout << "PoissonRatio       = " << PoissonRatio << std::endl;
    vout << "LameI              = " << LameI        << std::endl;
    vout << "LameII             = " << LameII       << std::endl;
    vout << "BulkModulus        = " << BulkModulus  << std::endl;
    if ( fibersEnabled )
    {
        vout << "stiffnessF         = " << stiffnessF   << std::endl;
        vout << "nonlinearF         = " << nonlinearF   << std::endl;
        vout << "stretchF           = " << stretchF     << std::endl;
        vout << "smoothnessF        = " << smoothnessF  << std::endl;
    }
    vout << "rhoS2               = " << rhoS2         << std::endl;
    vout << "YoungModulus2       = " << YoungModulus2 << std::endl;
    vout << "PoissonRatio2       = " << PoissonRatio2 << std::endl;
    vout << "LameI2              = " << LameI2        << std::endl;
    vout << "LameII2             = " << LameII2       << std::endl;
    vout << "BulkModulus2        = " << BulkModulus2  << std::endl;
    vout << "-------------------- " << std::endl;
    vout << "alpha_inter        = " << alpha_inter     << std::endl;
    vout << "gamma_virt         = " << gamma_virt      << std::endl;
    vout << "gamma_inter        = " << gamma_inter     << std::endl;
    vout << "gamma_ghost        = " << gamma_ghost     << std::endl;
    vout << "gamma_v1           = " << gamma_v1        << std::endl;
    vout << "gamma_v2           = " << gamma_v2        << std::endl;
    vout << "gamma_p            = " << gamma_p         << std::endl;
    vout << "-------------------- " << std::endl;
    vout << "movementEnabled    = " << movementEnabled << std::endl;
    vout << "nlConvEnabled      = " << nlConvEnabled   << std::endl;
    vout << "nlMaxIter          = " << nlMaxIter       << std::endl;
    vout << "nlMethod           = " << nlMethod        << std::endl;
    vout << "convToll           = " << convToll        << std::endl;
    vout << "kinToll            = " << kinToll         << std::endl;
    vout << "dispToll           = " << dispToll        << std::endl;
    vout << "relaxSolSolid      = " << relaxSolSolid   << std::endl;
    vout << "mLin               = " << mLin            << std::endl;
    vout << "-------------------- " << std::endl;

    const bool restartEnabled            = static_cast<bool> (dataFile ("importer/restartFSI", 0 ) );
    const UInt loadInitTimeIter          = dataFile ( "importer/initTimeIter", 0);
    const UInt loadInitNlIter            = 0;
    
    const std::string loadInitTime       = "00000";
    const std::string loadOld1Time       = "00000";

    if ( restartEnabled )
    {
        vout << "Restart enabled:" << std::endl;
        vout << "time (iter)             = " << loadInitTimeIter  << std::endl;
        vout << "nl (iter)               = " << loadInitNlIter    << std::endl;
        vout << "-------------------- " << std::endl;
    }

    Real P_out      = dataFile ( "bc/P", 40. );
    Real stroke     = dataFile ( "bc/stroke", 0.075 );
    Real frequency  = dataFile ( "bc/frequency", 44 );


    vout << "-------------------- OPERATING POIN --------------------"  << std::endl;
    vout << "Outlet Pressure      = "       << P_out                          << std::endl;
    vout << "Stroke               = "       << stroke                         << std::endl;
    vout << "Frequency            = "       << frequency                      << std::endl;
    vout << "--------------------------------------------------------"  << std::endl;

    BCSettings bcSettings ( P_out, stroke, frequency );


    vout << "-------------------- " << std::endl;
    vout << "perturbCoeff       = " << perturbCoeff    << std::endl;
    vout << "-------------------- " << std::endl;

    vout << " done ! " << std::endl;

    // +-----------------------------------------------+
    // |                 Reading mesh                  |
    // +-----------------------------------------------+

    localChrono.start();

    vout << " -- Reading the meshes ... " << std::endl;

    const std::string meshDir = dataFile ( "mesh/mesh_dir", "./");
    const std::string bgFileName = dataFile ( "mesh/bg_mesh_file", "fluid.mesh");
    const std::string fg1FileName = dataFile ( "mesh/fg1_mesh_file", "solid1.mesh");
    const std::string fg2FileName = dataFile ( "mesh/fg2_mesh_file", "solid2.mesh");
    MeshData * meshData;

    // Read BG mesh: it is never used, it just checks that the mesh is fine.
    meshPtr_Type fullBGMeshPoolPtr ( new mesh_Type ( Comm ) );

    meshData = new MeshData;
    meshData->setMeshDir  ( meshDir );
    meshData->setMeshFile ( bgFileName );
    meshData->setMeshType ( ".mesh" );
    meshData->setMOrder   ( "P1" );
    meshData->setVerbose  ( false );

    vout << "   -- BG mesh file: " << meshData->meshDir() << meshData->meshFile() << std::endl;

    readMesh (*fullBGMeshPoolPtr, *meshData);
    delete meshData;

    fullBGMeshPoolPtr->setId(0);
    fullBGMeshPoolPtr->setLocalId(myPID);

    // Read FG1 mesh: it is never used, it just checks that the mesh is fine.
    meshPtr_Type fullFG1MeshRefPtr ( new mesh_Type ( Comm ) );

    meshData = new MeshData;
    meshData->setMeshDir  ( meshDir );
    meshData->setMeshFile ( fg1FileName );
    meshData->setMeshType ( ".mesh" );
    meshData->setMOrder   ( "P1" );
    meshData->setVerbose  ( false );

    vout << "   -- FG1 mesh file: " << meshData->meshDir() << meshData->meshFile() << std::endl;

    readMesh (*fullFG1MeshRefPtr, *meshData);
    delete meshData;

    fullFG1MeshRefPtr->setId(1);
    fullFG1MeshRefPtr->setLocalId(myPID);

    // Read FG2 mesh: it is never used, it just checks that the mesh is fine.
    meshPtr_Type fullFG2MeshRefPtr ( new mesh_Type ( Comm ) );

    meshData = new MeshData;
    meshData->setMeshDir  ( meshDir );
    meshData->setMeshFile ( fg2FileName );
    meshData->setMeshType ( ".mesh" );
    meshData->setMOrder   ( "P1" );
    meshData->setVerbose  ( false );

    vout << "   -- FG2 mesh file: " << meshData->meshDir() << meshData->meshFile() << std::endl;

    readMesh (*fullFG2MeshRefPtr, *meshData);
    delete meshData;

    fullFG2MeshRefPtr->setId(2);
    fullFG2MeshRefPtr->setLocalId(myPID);

    localChrono.stop();

    vout << " done in " << localChrono.diff() << " s!" << std::endl;

    localChrono.reset();
    localChrono.start();

    vout << " -- Check full meshes ... " << std::endl;

    if ( fullBGMeshPoolPtr->check(0,false,false) )
    {
        std::cout << "PID " << myPID << " something wrong with BG full mesh. " << std::endl;
    }
    if ( fullFG1MeshRefPtr->check(0,false,false) )
    {
        std::cout << "PID " << myPID << " something wrong with FG1 full mesh. " << std::endl;
    }
    if ( fullFG2MeshRefPtr->check(0,false,false) )
    {
        std::cout << "PID " << myPID << " something wrong with FG2 full mesh. " << std::endl;
    }

    fullFG1MeshRefPtr->cleanElementEdges();
    fullFG1MeshRefPtr->cleanElementFaces();
    fullFG1MeshRefPtr->setMaxNumGlobalEdges(0);
    fullFG1MeshRefPtr->updateElementFaces( true, false, 0 );
    fullFG1MeshRefPtr->updateElementEdges( true, false, 0 );

    fullFG2MeshRefPtr->cleanElementEdges();
    fullFG2MeshRefPtr->cleanElementFaces();
    fullFG2MeshRefPtr->setMaxNumGlobalEdges(0);
    fullFG2MeshRefPtr->updateElementFaces( true, false, 0 );
    fullFG2MeshRefPtr->updateElementEdges( true, false, 0 );

    localChrono.stop();

    vout << " done in " << localChrono.diff() << " s!" << std::endl;

    localChrono.reset();
    localChrono.start();

    vout << " -- Partitioning the Reference FG1 mesh ... " << std::endl;

    // Vector of all the meshes of the partition in the reference configuration
    std::vector< meshPtr_Type > fg1MeshRefPtr( nproc , nullptr );
    {
        MeshPartitioner< mesh_Type > fg1MeshPart;
        fg1MeshPart.setPartitionOverlap ( 1 );
        fg1MeshPart.setCleanUp ( false );
        fg1MeshPart.setup ( nproc , Comm );
        fg1MeshPart.doPartition ( fullFG1MeshRefPtr, Comm );
        // Get all the parts of the mesh
        for ( Int k = 0; k < nproc; ++k )
        {
            fg1MeshRefPtr[k] = fg1MeshPart.meshPartition(k);
            fg1MeshRefPtr[k]->setId(1);
            fg1MeshRefPtr[k]->setLocalId(k);
        }
        fg1MeshPart.cleanUp();
    }

    vout << " -- Partitioning the Reference FG2 mesh ... " << std::endl;

    // Vector of all the meshes of the partition in the reference configuration
    std::vector< meshPtr_Type > fg2MeshRefPtr( nproc , nullptr );
    {
        MeshPartitioner< mesh_Type > fg2MeshPart;
        fg2MeshPart.setPartitionOverlap ( 1 );
        fg2MeshPart.setCleanUp ( false );
        fg2MeshPart.setup ( nproc , Comm );
        fg2MeshPart.doPartition ( fullFG2MeshRefPtr, Comm );
        // Get all the parts of the mesh
        for ( Int k = 0; k < nproc; ++k )
        {
            fg2MeshRefPtr[k] = fg2MeshPart.meshPartition(k);
            fg2MeshRefPtr[k]->setId(2);
            fg2MeshRefPtr[k]->setLocalId(k);
        }
        fg2MeshPart.cleanUp();
    }

    localChrono.stop();

    vout << " done in " << localChrono.diff() << " s!" << std::endl;

    // +-----------------------------------------------+
    // |              Setting Time Data                |
    // +-----------------------------------------------+

    localChrono.reset();
    localChrono.start();

    vout << " -- Setting time data ... " << std::endl;

    timeDataPtr_Type timeDataPtr
        ( new timeData_Type ( dataFile, "time_discretization" ) );
    timeAdvanceDataPtr_Type timeAdvanceDataFluidPtr
        ( new timeAdvanceData_Type ( dataFile, "time_discretization/fluid" ) );
    timeAdvanceDataPtr_Type timeAdvanceDataSolidPtr
        ( new timeAdvanceData_Type ( dataFile, "time_discretization/solid" ) );

    std::string fluidTimeAdvanceMethod =
        dataFile ( "time_discretization/fluid/method", "BDF");
    std::string solidTimeAdvanceMethod =
        dataFile ( "time_discretization/solid/method", "Newmark");

    timeAdvancePtr_Type timeAdvanceFluidPtr
        ( TimeAdvanceFactory::instance().createObject ( fluidTimeAdvanceMethod ) );
    timeAdvancePtr_Type timeAdvanceSolid1Ptr
        ( TimeAdvanceFactory::instance().createObject ( solidTimeAdvanceMethod ) );
    timeAdvancePtr_Type timeAdvanceSolid2Ptr
        ( TimeAdvanceFactory::instance().createObject ( solidTimeAdvanceMethod ) );

    if (fluidTimeAdvanceMethod == "Newmark")
    {
        timeAdvanceFluidPtr->setup (timeAdvanceDataFluidPtr->coefficientsNewmark() , 1);
    }
    if (fluidTimeAdvanceMethod == "BDF")
    {
        timeAdvanceFluidPtr->setup (timeAdvanceDataFluidPtr->orderBDF(), 1);
    }
    timeAdvanceFluidPtr->setTimeStep (timeDataPtr->timeStep() );

    if (solidTimeAdvanceMethod == "Newmark")
    {
        timeAdvanceSolid1Ptr->setup (timeAdvanceDataSolidPtr->coefficientsNewmark() , 2);
        timeAdvanceSolid2Ptr->setup (timeAdvanceDataSolidPtr->coefficientsNewmark() , 2);
    }
    if (solidTimeAdvanceMethod == "BDF")
    {
        timeAdvanceSolid1Ptr->setup (timeAdvanceDataSolidPtr->orderBDF(), 2);
        timeAdvanceSolid2Ptr->setup (timeAdvanceDataSolidPtr->orderBDF(), 2);
    }
    timeAdvanceSolid1Ptr->setTimeStep ( timeDataPtr->timeStep() );
    timeAdvanceSolid2Ptr->setTimeStep ( timeDataPtr->timeStep() );

    timeDataPtr->showMe(vout);
    timeAdvanceFluidPtr->showMe(vout);
    timeAdvanceSolid1Ptr->showMe(vout);
    timeAdvanceSolid2Ptr->showMe(vout);

    const Real initialTime = timeDataPtr->initialTime();
    const UInt initialIter = restartEnabled ? loadInitNlIter : 0;
    const Real dt = timeDataPtr->timeStep();
    const Real T  = timeDataPtr->endTime();
    const Real maxTIter = T / dt;
    const UInt tPadding = std::floor( std::log10(maxTIter) ) + 1;
    const UInt nlPadding = std::floor( std::log10(nlMaxIter) ) + 1;
    const Real subdt = dt / nlMaxIter;

    Real time = initialTime;
    UInt timeIter = restartEnabled ? loadInitTimeIter : 0;
    UInt nlIter = initialIter;
    bool nlConverged = false;

    const Real alphaF = timeAdvanceFluidPtr->coefficientFirstDerivative ( 0 ) / dt;
    const Real alphaS = timeAdvanceSolid1Ptr->coefficientSecondDerivative ( 0 ) / ( dt * dt );
    const Real alphaW = timeAdvanceSolid1Ptr->coefficientFirstDerivative ( 0 ) / dt;
    const UInt stencilSize = timeAdvanceFluidPtr->size();

    vout << "dt : " << dt << std::endl;
    vout << "alphaF : " << alphaF << std::endl;
    vout << "alphaS : " << alphaS << std::endl;
    vout << "alphaW : " << alphaW << std::endl;

    localChrono.stop();

    vout << " done in " << localChrono.diff() << " s!" << std::endl;

    // +-----------------------------------------------+
    // |           Building Fluid/Solid stuff          |
    // +-----------------------------------------------+

    localChrono.reset();
    localChrono.start();

    vout << " -- Creating the fluid stuff ... " << std::endl;

    // Velocity on the partitioned fluid mesh
    vectorPtr_Type velocitySol;
    // Velocity on the partitioned fluid mesh at the previous non-linear iteration
    vectorPtr_Type oldNlVelocity;
    // Velocity on the partitioned fluid mesh at the previous time
    vectorPtr_Type oldTimeVelocity;
    // Pressure on the partitioned fluid mesh
    vectorPtr_Type pressureSol;

    // Store the partitioned BG mesh at the previous non-linear iteration
    meshPtr_Type oldNlBGMeshPtr;
    // Store the partitioned BG mesh at the previous time step
    meshPtr_Type oldTimeBGMeshPtr;
    // Store the partitioned extended finite element space at the previous non-linear iteration
    uXSpacePtr_Type oldNlUXFESpace;
    // Store the partitioned extended finite element space at the previous time step
    uXSpacePtr_Type oldTimeUXFESpace;

    // Inizialite the partitioned old fluid stencil
    stencil_Type oldTimeStencilVelocity( stencilSize , nullptr );

    // Export on the fluid export space
    ss.str( std::string() );
    ss << "fsiFluidSol";
    const std::string expFluidSolFileName = ss.str();

    ExporterHDF5XFEMExport< mesh_Type > exporterFluid ( dataFile, expFluidSolFileName );
    exporterFluid.setMultimesh ( true );

    vout << " -- Creating the solid stuff ... " << std::endl;

    const std::string fgDegree = "P1";

    // Solid constitutive law
    constitutiveLaw_Type solidLaw1;

    solidLaw1.setConstitutiveData( YoungModulus, PoissonRatio,
                                   ShearModulus, BulkModulus, solidGamma,
                                   stiffnessF, nonlinearF, stretchF, smoothnessF );

    solidLaw1.setBlocks( 0 , 1 , 2 );


    couplingFSI_Type couplingFSI1;

    couplingFSI1.setData( mu );
    couplingFSI1.setBlocks( 0 , 1 , 2 );
    couplingFSI1.setNonLinearity ( nonLinear );

    // Coupling FSI type
    couplingFSI_Type couplingFSI2;

    couplingFSI2.setData( mu );
    couplingFSI2.setBlocks( 0 , 1 , 3 );
    couplingFSI2.setNonLinearity ( nonLinear );   

    // Defining standard and ET finite element spaces for solid
    dSpacePtr_Type dSolid1FESpace ( new dSpace_Type ( fg1MeshRefPtr[myPID],
                                                      dataFile ( "finite_element/degree_solid", "P1" ),
                                                      3,
                                                      Comm ) );

    dSpaceETAPtr_Type ETdSolid1FESpace ( new dSpaceETA_Type ( fg1MeshRefPtr[myPID],
                                                          & ( dSolid1FESpace->refFE() ),
                                                          & ( dSolid1FESpace->fe().geoMap() ),
                                                              Comm ) );

    dSpacePtr_Type dSolid2FESpace ( new dSpace_Type ( fg2MeshRefPtr[myPID],
                                                      dataFile ( "finite_element/degree_solid", "P1" ),
                                                      3,
                                                      Comm ) );

    dSpaceETAPtr_Type ETdSolid2FESpace ( new dSpaceETA_Type ( fg2MeshRefPtr[myPID],
                                                          & ( dSolid2FESpace->refFE() ),
                                                          & ( dSolid2FESpace->fe().geoMap() ),
                                                              Comm ) );

    // Solution on the partitioned solid mesh
    vectorPtr_Type dispSol1 ( new vector_Type ( ETdSolid1FESpace->map() , Repeated) );
    // Solution on the partitioned solid mesh at the previous iteration
    vectorPtr_Type oldNlDisp1 ( new vector_Type ( ETdSolid1FESpace->map() , Repeated) );
    // Velocity on the partitioned solid mesh
    vectorPtr_Type solidVel1 ( new vector_Type ( ETdSolid1FESpace->map() , Repeated) );

    // Solution on the partitioned solid mesh
    vectorPtr_Type dispSol2 ( new vector_Type ( ETdSolid2FESpace->map() , Repeated) );
    // Solution on the partitioned solid mesh at the previous iteration
    vectorPtr_Type oldNlDisp2 ( new vector_Type ( ETdSolid2FESpace->map() , Repeated) );
    // Velocity on the partitioned solid mesh
    vectorPtr_Type solidVel2 ( new vector_Type ( ETdSolid2FESpace->map() , Repeated) );

    vout << " done in " << localChrono.diff() << " s!" << std::endl << std::endl;

    vout << "    ------------------------ " << std::endl;
    vout << "    --     At time " << time << " ... " << std::endl;
    vout << "    ------------------------ " << std::endl;

    timeChrono.reset();
    timeChrono.start();

    vout << "        ----------------------------- " << std::endl;
    vout << "        --  Internal iteration " << nlIter << " ... " << std::endl;
    vout << "        ----------------------------- " << std::endl;
    eout << "Time " << time << " Iter " << nlIter << std::endl;

    localChrono.reset();
    localChrono.start();

    vout << " -- Copy meshes ... " << std::endl;

    vout << "   -- Copy the BG mesh ..." << std::endl;

    // Store the current BG mesh for assembling
    meshPtr_Type fullBGMeshCurPtr ( new mesh_Type ( *fullBGMeshPoolPtr ) );

    vout << "   -- Copy the FG1 mesh ..." << std::endl;

    // Store the current FG mesh in the current configuration just for intersections
    meshPtr_Type fullFG1MeshCurPtr ( new mesh_Type ( *fullFG1MeshRefPtr ) );

    vout << "   -- Copy the FG2 mesh ..." << std::endl;

    // Store the current FG mesh in the current configuration just for intersections
    meshPtr_Type fullFG2MeshCurPtr ( new mesh_Type ( *fullFG2MeshRefPtr ) );

    localChrono.stop();

    vout << " done in " << localChrono.diff() << " s!" << std::endl;

    // Initialization
    if ( restartEnabled ) // with restart
    {
        localChrono.reset();
        localChrono.start();

        ss.str( std::string() );
        ss << "fsiRestartF_";
        ss << std::setfill('0') << std::setw(tPadding) << loadInitTimeIter;
        ss << "_";
        ss << std::setfill('0') << std::setw(nlPadding) << nlIter;
        const std::string initFluidName = ss.str();
        vout << "initFluidName           = " << initFluidName     << std::endl;
        
        ss.str( std::string() );
        ss << "fsiRestartF_";
        ss << std::setfill('0') << std::setw(tPadding) << loadInitTimeIter-1;
        ss << "_";
        ss << std::setfill('0') << std::setw(nlPadding) << nlIter;
        const std::string oldFluidName = ss.str();
        vout << "oldFluidName            = " << oldFluidName      << std::endl;

        ss.str( std::string() );
        ss << "fsiRestartS1_";
        ss << std::setfill('0') << std::setw(tPadding) << loadInitTimeIter;
        ss << "_";
        ss << std::setfill('0') << std::setw(nlPadding) << nlIter;
        const std::string initSolidName1 = ss.str();
        vout << "initSolidName1          = " << initSolidName1    << std::endl;

        ss.str( std::string() );
        ss << "fsiRestartS1_";
        ss << std::setfill('0') << std::setw(tPadding) << loadInitTimeIter-1;
        ss << "_";
        ss << std::setfill('0') << std::setw(nlPadding) << nlIter;
        const std::string old1SolidName1 = ss.str();
        vout << "old1SolidName1          = " << old1SolidName1    << std::endl;

        ss.str( std::string() );
        ss << "fsiRestartS2_";
        ss << std::setfill('0') << std::setw(tPadding) << loadInitTimeIter;
        ss << "_";
        ss << std::setfill('0') << std::setw(nlPadding) << nlIter;
        const std::string initSolidName2 = ss.str();
        vout << "initSolidName2          = " << initSolidName2    << std::endl;

        ss.str( std::string() );
        ss << "fsiRestartS2_";
        ss << std::setfill('0') << std::setw(tPadding) << loadInitTimeIter-1;
        ss << "_";
        ss << std::setfill('0') << std::setw(nlPadding) << nlIter;
        const std::string old1SolidName2 = ss.str();
        vout << "old1SolidName2          = " << old1SolidName2    << std::endl;



        vout << " -- Restart. Load Previous Solid Solution ... " << std::endl;

        // Serial communicator
        std::shared_ptr<Epetra_Comm> SerialComm (new Epetra_MpiComm (MPI_COMM_SELF) );

        // Full solid FESpace
        dSpacePtr_Type dFullSolid1FESpace
            ( new dSpace_Type ( fullFG1MeshCurPtr,
                                dataFile ( "finite_element/degree_solid", "P1" ),
                                3,
                                SerialComm ) );

        dSpacePtr_Type dFullSolid2FESpace
            ( new dSpace_Type ( fullFG2MeshCurPtr,
                                dataFile ( "finite_element/degree_solid", "P1" ),
                                3,
                                SerialComm ) );

        // Solution on the solid mesh
        vectorPtr_Type fullDispSol1 ( new vector_Type ( dFullSolid1FESpace->map() , Repeated  ) );
        // Solution on the solid mesh at the previous time
        vectorPtr_Type fullOldNlDisp1 ( new vector_Type ( dFullSolid1FESpace->map() , Repeated  ) );

        // Solution on the solid mesh
        vectorPtr_Type fullDispSol2 ( new vector_Type ( dFullSolid2FESpace->map() , Repeated  ) );
        // Solution on the solid mesh at the previous time
        vectorPtr_Type fullOldNlDisp2 ( new vector_Type ( dFullSolid2FESpace->map() , Repeated  ) );

        // Import last non-linear displacement
        ExporterHDF5< mesh_Type > importerInitSolid1 ( dataFile,
                                                       fullFG1MeshCurPtr,
                                                       initSolidName1,
                                                       myPID );

        ExporterData< mesh_Type > initSolidSol1 ( ExporterData< mesh_Type >::VectorField,
                                                  "displacement." + loadInitTime,
                                                  dFullSolid1FESpace,
                                                  fullDispSol1,
                                                  UInt ( 0 ),
                                                  ExporterData< mesh_Type >::UnsteadyRegime );


        ExporterHDF5< mesh_Type > importerOld1Solid1 ( dataFile,
                                                       fullFG1MeshCurPtr,
                                                       old1SolidName1,
                                                       myPID );

        ExporterData< mesh_Type > old1SolidSol1 ( ExporterData< mesh_Type >::VectorField,
                                                  "displacement." + loadOld1Time,
                                                  dFullSolid1FESpace,
                                                  fullOldNlDisp1,
                                                  UInt ( 0 ),
                                                  ExporterData< mesh_Type >::UnsteadyRegime );

        importerInitSolid1.readVariable (initSolidSol1);
        importerInitSolid1.closeFile ();

        importerOld1Solid1.readVariable (old1SolidSol1);
        importerOld1Solid1.closeFile ();


        ExporterHDF5< mesh_Type > importerInitSolid2 ( dataFile,
                                                       fullFG2MeshCurPtr,
                                                       initSolidName2,
                                                       myPID );

        ExporterData< mesh_Type > initSolidSol2 ( ExporterData< mesh_Type >::VectorField,
                                                  "displacement2." + loadInitTime,
                                                  dFullSolid2FESpace,
                                                  fullDispSol2,
                                                  UInt ( 0 ),
                                                  ExporterData< mesh_Type >::UnsteadyRegime );

        ExporterHDF5< mesh_Type > importerOld1Solid2 ( dataFile,
                                                       fullFG2MeshCurPtr,
                                                       old1SolidName2,
                                                       myPID );

        ExporterData< mesh_Type > old1SolidSol2 ( ExporterData< mesh_Type >::VectorField,
                                                  "displacement2." + loadOld1Time,
                                                  dFullSolid2FESpace,
                                                  fullOldNlDisp2,
                                                  UInt ( 0 ),
                                                  ExporterData< mesh_Type >::UnsteadyRegime );
        
        importerInitSolid2.readVariable (initSolidSol2);
        importerInitSolid2.closeFile ();

        importerOld1Solid2.readVariable (old1SolidSol2);
        importerOld1Solid2.closeFile ();

        localChrono.stop();

        vout << " done in " << localChrono.diff() << " s!" << std::endl;

        localChrono.reset();
        localChrono.start();

        vout << " -- Add perturbation to the solution fullDispSol1 ... " << std::endl;
        *fullDispSol1 += perturbCoeff;
        vout << " -- Add perturbation to the solution fullDispSol2 ... " << std::endl;
        *fullDispSol2 += perturbCoeff;

        vout << " -- Move Solid Mesh ... " << std::endl;

        // Linearization of the full solid displacement
        dSpacePtr_Type lFullSolid1FESpace ( new dSpace_Type ( fullFG1MeshCurPtr,
                                                              "P1",
                                                              3,
                                                              SerialComm ) );

        vectorPtr_Type linearFullDispSol1 ( new vector_Type ( lFullSolid1FESpace->map() , Repeated) );

        dSpacePtr_Type lFullSolid2FESpace ( new dSpace_Type ( fullFG2MeshCurPtr,
                                                              "P1",
                                                              3,
                                                              SerialComm ) );

        vectorPtr_Type linearFullDispSol2 ( new vector_Type ( lFullSolid2FESpace->map() , Repeated) );

        if ( fgDegree == "P1" )
        {
            *linearFullDispSol1 = *fullOldNlDisp1;
            *linearFullDispSol2 = *fullOldNlDisp2;
        }
        else
        {
            *linearFullDispSol1 = lFullSolid1FESpace->feToFEInterpolate( *dFullSolid1FESpace,
                                                                         *fullOldNlDisp1,
                                                                         Repeated );
            *linearFullDispSol2 = lFullSolid2FESpace->feToFEInterpolate( *dFullSolid2FESpace,
                                                                         *fullOldNlDisp2,
                                                                         Repeated );
        }

        if ( movementEnabled )
        {
            // Move the solid mesh to the current configuration
            fullFG1MeshCurPtr->meshTransformer().simpleMoveMesh
                ( *linearFullDispSol1 , lFullSolid1FESpace->dof().numTotalDof() );

            // Check that the mesh has no inverted elements
            if ( myPID == 0 )
            {
                std::vector<bool> elSign;
                Switch sw;
                (void) checkVolumes ( *fullFG1MeshCurPtr, elSign, sw );
                if ( sw.test ( "HAS_NEGATIVE_VOLUMES" ) )
                {
                    eout << " ERROR! FG1 mesh has inverted elements. Exit." << std::endl;
                    return ( EXIT_FAILURE );
                }
            }

            // Move the solid mesh to the current configuration
            fullFG2MeshCurPtr->meshTransformer().simpleMoveMesh
                ( *linearFullDispSol2 , lFullSolid2FESpace->dof().numTotalDof() );

            // Check that the mesh has no inverted elements
            if ( myPID == 0 )
            {
                std::vector<bool> elSign;
                Switch sw;
                (void) checkVolumes ( *fullFG2MeshCurPtr, elSign, sw );
                if ( sw.test ( "HAS_NEGATIVE_VOLUMES" ) )
                {
                    eout << " ERROR! FG2 mesh has inverted elements. Exit." << std::endl;
                    return ( EXIT_FAILURE );
                }
            }
        }

        localChrono.stop();

        vout << " done in " << localChrono.diff() << " s!" << std::endl;

        // +-----------------------------------------------+
        // |              Mesh intersection                |
        // +-----------------------------------------------+

        localChrono.reset();
        localChrono.start();

        vout << " -- Mesh intersection ... " << std::endl;

        interPointsPtr_Type interPoints ( new IntersectionPoints() );

        vout << "   -- BG vs FG1 ... " << std::endl;

        MeshIntersection< mesh_Type , mesh_Type >
                         meshInter1(fullBGMeshCurPtr, fullFG1MeshCurPtr, interPoints);

        vout << "     -- compute intersections ... " << std::endl;

        meshInter1.computeIntersections(true,false);

        vout << "     done ! " << std::endl;

        vout << "     -- compute overlaps ... " << std::endl;

        meshInter1.computeOverlaps(true,false);

        vout << "     done ! " << std::endl;

        vout << "   done ! " << std::endl;

        vout << "   -- BG vs FG2 ... " << std::endl;

        MeshIntersection< mesh_Type , mesh_Type >
                         meshInter2(fullBGMeshCurPtr, fullFG2MeshCurPtr, interPoints);

        vout << "     -- compute intersections ... " << std::endl;

        meshInter2.computeIntersections(true,false);

        vout << "     done ! " << std::endl;

        vout << "     -- compute overlaps ... " << std::endl;

        meshInter2.computeOverlaps(true,false);

        vout << "     done ! " << std::endl;

        vout << "   done ! " << std::endl;

        localChrono.stop();

        vout << " done in " << localChrono.diff() << " s!" << std::endl;

        // +-----------------------------------------------+
        // |              Cut-Mesh generation              |
        // +-----------------------------------------------+

        localChrono.reset();
        localChrono.start();

        vout << " -- Cut elements ... " << std::endl;

        std::map< UInt , meshPtr_Type > cuttingMeshes(
            {
              { fullFG1MeshCurPtr->id() , fullFG1MeshCurPtr } ,
              { fullFG2MeshCurPtr->id() , fullFG2MeshCurPtr }
            }
            );

        CutElements cutElem(fullBGMeshCurPtr, interPoints, cuttingMeshes);

        vout << "   -- Build cut elements ... " << std::endl;

        cutElem.buildCutElements();

        vout << "   done ! " << std::endl;

        vout << "   -- Assign global IDs ... " << std::endl;

        cutElem.assignGlobalIds();

        vout << "   done ! " << std::endl;

        vout << "   -- Compute regions ... " << std::endl;

        cutElem.computeRegions(CutElements::ExtendedMethodType::XFEM,
                               CutElements::ContinuityType::Continuous);

        interPoints->clearPointToEntity();

        vout << "   done ! " << std::endl;

        localChrono.stop();

        vout << " done in " << localChrono.diff() << " s!" << std::endl;

        // +-----------------------------------------------+
        // |               Mesh partitioning               |
        // +-----------------------------------------------+

        localChrono.reset();
        localChrono.start();

        vout << " -- Partitioning the Current BG mesh ... " << std::endl;

        // It is in the current configuration
        meshPtr_Type bgMeshCurPtr;
        {
            MeshPartitionerIntersection< mesh_Type > bgMeshPart;
            bgMeshPart.setPartitionOverlap ( 1 );
            bgMeshPart.doPartition ( fullBGMeshCurPtr, Comm, interPoints );
            bgMeshCurPtr = bgMeshPart.meshPartition();
            interPoints = bgMeshPart.intersectionPointPartition();
            bgMeshCurPtr->setId(0);
            bgMeshCurPtr->setLocalId(myPID);
        }

        // Free global mesh
        fullBGMeshCurPtr.reset();

        localChrono.stop();

        vout << " done in " << localChrono.diff() << " s!" << std::endl;

        localChrono.reset();
        localChrono.start();

        vout << " -- Partitioning the Current FG1 mesh ... " << std::endl;

        // Vector of all the meshes of the partition in the current configuration
        std::vector< meshPtr_Type > fg1MeshCurPtr( nproc , nullptr );
        {
            MeshPartitioner< mesh_Type > fg1MeshPart;
            fg1MeshPart.setPartitionOverlap ( 1 );
            fg1MeshPart.setCleanUp ( false );
            fg1MeshPart.setup ( nproc , Comm );
            fg1MeshPart.doPartition ( fullFG1MeshCurPtr, Comm );
            // Get the part of the mesh of the current processor
            fg1MeshCurPtr[myPID] = fg1MeshPart.meshPartition(myPID);
            fg1MeshCurPtr[myPID]->setId(1);
            fg1MeshCurPtr[myPID]->setLocalId(myPID);
            fg1MeshPart.updateAdjacencies( bgMeshCurPtr );
            fg1MeshPart.cleanUp();
        }

        // Free global mesh
        fullFG1MeshCurPtr.reset();

        localChrono.stop();

        vout << " done in " << localChrono.diff() << " s!" << std::endl;

        localChrono.reset();
        localChrono.start();

        vout << " -- Partitioning the Current FG2 mesh ... " << std::endl;

        // Vector of all the meshes of the partition in the current configuration
        std::vector< meshPtr_Type > fg2MeshCurPtr( nproc , nullptr );
        {
            MeshPartitioner< mesh_Type > fg2MeshPart;
            fg2MeshPart.setPartitionOverlap ( 1 );
            fg2MeshPart.setCleanUp ( false );
            fg2MeshPart.setup ( nproc , Comm );
            fg2MeshPart.doPartition ( fullFG2MeshCurPtr, Comm );
            // Get the part of the mesh of the current processor
            fg2MeshCurPtr[myPID] = fg2MeshPart.meshPartition(myPID);
            fg2MeshCurPtr[myPID]->setId(2);
            fg2MeshCurPtr[myPID]->setLocalId(myPID);
            fg2MeshPart.updateAdjacencies( bgMeshCurPtr );
            fg2MeshPart.cleanUp();
        }

        // Free global mesh
        fullFG2MeshCurPtr.reset();

        localChrono.stop();

        vout << " done in " << localChrono.diff() << " s!" << std::endl;

        localChrono.reset();
        localChrono.start();

        vout << " -- Check partitioned meshes ... " << std::endl;

        if ( bgMeshCurPtr->check(0,false,false) )
        {
            std::cout << "PID " << myPID << " something wrong with BG part mesh. " << std::endl;
        }
        if ( fg1MeshCurPtr[myPID]->check(0,false,false) )
        {
            std::cout << "PID " << myPID << " something wrong with FG1 part mesh. " << std::endl;
        }
        if ( fg2MeshCurPtr[myPID]->check(0,false,false) )
        {
            std::cout << "PID " << myPID << " something wrong with FG2 part mesh. " << std::endl;
        }

        localChrono.stop();

        vout << " done in " << localChrono.diff() << " s!" << std::endl;

        localChrono.reset();
        localChrono.start();

        vout << " -- Generate cut mesh ... " << std::endl;

        std::vector<UInt> parentId;

        // It is in the current configuration
        meshPtr_Type cutMeshPtr =
            cutElem.generateCutMesh(bgMeshCurPtr, interPoints, &parentId);

        if ( cutMeshPtr->check(0,false,false) )
        {
            std::cout << "PID " << myPID << " something wrong with the cutMesh part mesh. " << std::endl;
        }

        localChrono.stop();

        vout << " done in " << localChrono.diff() << " s!" << std::endl;

        // +-----------------------------------------------+
        // |            Building Fluid FE spaces           |
        // +-----------------------------------------------+

        localChrono.reset();
        localChrono.start();

        vout << " -- Building the fluid finite elements spaces ... " << std::endl;

        // Defining standard and ET finite element spaces:
        // - for fluid
        uXSpacePtr_Type uXFESpace ( new uXSpace_Type ( bgMeshCurPtr,
                                                       dataFile ( "finite_element/degree_fluid", "P1" ),
                                                       3,
                                                       Comm ) );

        uXSpaceETAPtr_Type ETuXFESpace ( new uXSpaceETA_Type ( bgMeshCurPtr,
                                                           & ( uXFESpace->refFE() ),
                                                           & ( uXFESpace->fe().geoMap() ),
                                                               Comm ) );

        uExportSpacePtr_Type uExportFESpace ( new uExportSpace_Type ( cutMeshPtr,
                                                                      dataFile ( "finite_element/degree_fluid", "P1" ),
                                                                      3 ) );

        pXSpacePtr_Type pXFESpace ( new pXSpace_Type ( bgMeshCurPtr,
                                                       dataFile ( "finite_element/degree_fluid", "P1" ),
                                                       1,
                                                       Comm ) );

        pXSpaceETAPtr_Type ETpXFESpace ( new pXSpaceETA_Type ( bgMeshCurPtr,
                                                           & ( pXFESpace->refFE() ),
                                                           & ( pXFESpace->fe().geoMap() ),
                                                               Comm ) );

        pExportSpacePtr_Type pExportFESpace ( new pExportSpace_Type ( cutMeshPtr,
                                                                      dataFile ( "finite_element/degree_fluid", "P1" ),
                                                                      1 ) );

        localChrono.stop();

        vout << " done in " << localChrono.diff() << " s!" << std::endl;
        vout << " | # DOFS: "
             << 3 * uXFESpace->dof().numTotalDof() << " + "
             << pXFESpace->dof().numTotalDof() << " + "
             << 3 * dSolid1FESpace->dof().numTotalDof() << " + "
             << 3 * dSolid2FESpace->dof().numTotalDof() << " | " << std::endl;

        // +-----------------------------------------------+
        // |              Initialize solution              |
        // +-----------------------------------------------+

        localChrono.reset();
        localChrono.start();

        vout << " -- Initialize solution ... " << std::endl;

        // Declaring and filling union map
        mapPtr_Type unionMap;
        unionMap.reset ( new map_Type( uXFESpace->map() ) );
        *(unionMap) += pXFESpace->map();
        *(unionMap) += dSolid1FESpace->map();
        *(unionMap) += dSolid2FESpace->map();

        // Create solution vector
        blockVectorPtr_Type ETsolution
            ( new blockVector_Type ( ETuXFESpace->map() |
                                     ETpXFESpace->map() |
                                     ETdSolid1FESpace->map() |
                                     ETdSolid2FESpace->map() , Unique ) );
        // Clear solution vector
        *ETsolution *= 0.0;

        blockVectorPtr_Type ETsolutionRepeated;
        ETsolutionRepeated.reset ( new blockVector_Type ( *ETsolution , Repeated ) );

        // Create fluid solution
        velocitySol.reset ( new vector_Type ( ETuXFESpace->map() , Repeated) );
        pressureSol.reset ( new vector_Type ( ETpXFESpace->map() , Repeated) );
        *velocitySol *= 0.0;
        *pressureSol *= 0.0;

        // Clear vectors on the solid mesh
        *dispSol1 *= 0.0;
        *solidVel1 *= 0.0;
        *dispSol2 *= 0.0;
        *solidVel2 *= 0.0;

        localChrono.stop();

        vout << " done in " << localChrono.diff() << " s!" << std::endl;

        // +-----------------------------------------------+
        // |         Initialize Post-Processing            |
        // +-----------------------------------------------+

        localChrono.reset();
        localChrono.start();

        vout << " -- Initialize Post-processing ... " << std::endl;

        // Export on the fluid restart space
        ss.str( std::string() );
        ss << "fsiRestartF_";
        ss << std::setfill('0') << std::setw(tPadding) << timeIter;
        ss << "_";
        ss << std::setfill('0') << std::setw(nlPadding) << nlIter;
        const std::string expRestartFluidFileName = ss.str();

        ExporterHDF5XFEM< mesh_Type > exporterRestartF ( dataFile, bgMeshCurPtr, expRestartFluidFileName, myPID, pXFESpace->dofPtr() );
        exporterRestartF.setMultimesh ( false );

        exporterRestartF.addVariable ( ExporterData< mesh_Type , DOFXfem >::VectorField,
                                       "velocity",
                                       uXFESpace,
                                       velocitySol,
                                       UInt ( 0 ) );

        // Export on the solid restart space
        ss.str( std::string() );
        ss << "fsiRestartS1_";
        ss << std::setfill('0') << std::setw(tPadding) << timeIter;
        ss << "_";
        ss << std::setfill('0') << std::setw(nlPadding) << nlIter;
        const std::string expRestartSolid1FileName = ss.str();

        ExporterHDF5< mesh_Type > exporterRestartS1 ( dataFile, fg1MeshRefPtr[myPID], expRestartSolid1FileName, myPID );
        exporterRestartS1.setMultimesh ( false );

        exporterRestartS1.addVariable ( ExporterData< mesh_Type >::VectorField,
                                        "displacement",
                                        dSolid1FESpace,
                                        dispSol1,
                                        UInt ( 0 ) );

        // Export on the solid restart space
        ss.str( std::string() );
        ss << "fsiRestartS2_";
        ss << std::setfill('0') << std::setw(tPadding) << timeIter;
        ss << "_";
        ss << std::setfill('0') << std::setw(nlPadding) << nlIter;
        const std::string expRestartSolid2FileName = ss.str();

        ExporterHDF5< mesh_Type > exporterRestartS2 ( dataFile, fg2MeshRefPtr[myPID], expRestartSolid2FileName, myPID );
        exporterRestartS2.setMultimesh ( false );

        exporterRestartS2.addVariable ( ExporterData< mesh_Type >::VectorField,
                                        "displacement2",
                                        dSolid2FESpace,
                                        dispSol2,
                                        UInt ( 0 ) );

        localChrono.stop();

        vout << " done in " << localChrono.diff() << " s!" << std::endl;

        localChrono.reset();
        localChrono.start();

        vout << " -- Restart. Load Previous Fluid Solution ... " << std::endl;

        // Import last non-linear velocity
        ExporterHDF5< mesh_Type , DOFXfem > importerFluid( dataFile,
                                                           bgMeshCurPtr,
                                                           initFluidName,
                                                           myPID );

        // Imported (Unique) velocity on the partitioned fluid mesh
        vectorPtr_Type velocitySolUnique ( new vector_Type ( ETuXFESpace->map() , importerFluid.mapType()) );

        ExporterData< mesh_Type , DOFXfem > initFluidSol( ExporterData< mesh_Type , DOFXfem >::VectorField,
                                                          "velocity." + loadInitTime,
                                                          uXFESpace,
                                                          velocitySolUnique,
                                                          UInt ( 0 ),
                                                          ExporterData< mesh_Type , DOFXfem >::UnsteadyRegime );

        importerFluid.readVariable (initFluidSol);

        importerFluid.closeFile ();

        velocitySol.reset ( new vector_Type ( *velocitySolUnique , Repeated ) );
        velocitySolUnique.reset();

        localChrono.stop();

        vout << " done in " << localChrono.diff() << " s!" << std::endl;

        // +-----------------------------------------------+
        // |          Setting Initial Condition            |
        // +-----------------------------------------------+

        localChrono.reset();
        localChrono.start();

        vout << " -- Set initial conditions ... " << std::endl;

        // Set partitioned solution
        for ( UInt gid = 0; gid < fullDispSol1->map().mapSize(); ++gid )
        {
            if ( dispSol1->isGlobalIDPresent(gid) )
            {
                dispSol1->setCoefficient (gid, (*fullDispSol1)[gid]);
            }
        }

        for ( UInt gid = 0; gid < fullDispSol2->map().mapSize(); ++gid )
        {
            if ( dispSol2->isGlobalIDPresent(gid) )
            {
                dispSol2->setCoefficient (gid, (*fullDispSol2)[gid]);
            }
        }

        
        vectorPtr_Type old1TimeDisp1 ( new vector_Type ( ETdSolid1FESpace->map() , Repeated) );
        for ( UInt gid = 0; gid < fullOldNlDisp1->map().mapSize(); ++gid )
        {
            if ( old1TimeDisp1->isGlobalIDPresent(gid) )
            {
                old1TimeDisp1->setCoefficient (gid, (*fullOldNlDisp1)[gid]);
            }
        }
        timeAdvanceSolid1Ptr->setInitialCondition ( *old1TimeDisp1 );

        
        vectorPtr_Type old1TimeDisp2 ( new vector_Type ( ETdSolid2FESpace->map() , Repeated) );
        for ( UInt gid = 0; gid < fullOldNlDisp2->map().mapSize(); ++gid )
        {
            if ( old1TimeDisp2->isGlobalIDPresent(gid) )
            {
                old1TimeDisp2->setCoefficient (gid, (*fullOldNlDisp2)[gid]);
            }
        }
        timeAdvanceSolid2Ptr->setInitialCondition ( *old1TimeDisp2 );


        // Set old non-linear velocity
        oldNlVelocity.reset ( new vector_Type ( ETuXFESpace->map() , Repeated) );
        *oldNlVelocity = *velocitySol;

        oldNlBGMeshPtr = bgMeshCurPtr;
        oldNlUXFESpace = uXFESpace;

        Real timeExp = time - dt + subdt * (nlIter+1);

        // Updating the temporal scheme if reached the maximum number of iteration
        nlConverged = true;
        timeExp = time;

        timeAdvanceFluidPtr->setInitialCondition ( *velocitySol );
        timeAdvanceSolid1Ptr->shiftRight ( *dispSol1 );
        timeAdvanceSolid2Ptr->shiftRight ( *dispSol2 );

        // Set old time velocity
        oldTimeVelocity.reset ( new vector_Type ( ETuXFESpace->map() , Repeated) );
        *oldTimeVelocity = *velocitySol;

        oldTimeBGMeshPtr = bgMeshCurPtr;
        oldTimeUXFESpace = uXFESpace;

        nlIter = 0;
        time += dt;
        ++timeIter;

        // Set old non-linear displacement
        *oldNlDisp1 = *dispSol1;
        *oldNlDisp2 = *dispSol2;

        localChrono.stop();

        vout << " done in " << localChrono.diff() << " s!" << std::endl;

        // +-----------------------------------------------+
        // |            Exporting Initialization           |
        // +-----------------------------------------------+

        localChrono.reset();
        localChrono.start();

        vout << " -- Exporting ... " << std::endl;

        exporterRestartF.postProcess ( timeExp );
        exporterRestartS1.postProcess ( timeExp );
        exporterRestartS2.postProcess ( timeExp );

        localChrono.stop();

        vout << " done in " << localChrono.diff() << " s!" << std::endl;

        exporterRestartF.closeFile();
        exporterRestartS1.closeFile();
        exporterRestartS2.closeFile();
    }
    else // from null solution
    {
        // +-----------------------------------------------+
        // |              Mesh intersection                |
        // +-----------------------------------------------+

        localChrono.reset();
        localChrono.start();

        vout << " -- Mesh intersection ... " << std::endl;

        interPointsPtr_Type interPoints ( new IntersectionPoints() );

        vout << "   -- BG vs FG1 ... " << std::endl;

        MeshIntersection< mesh_Type , mesh_Type >
                         meshInter1(fullBGMeshCurPtr, fullFG1MeshCurPtr, interPoints);

        vout << "     -- compute intersections ... " << std::endl;

        meshInter1.computeIntersections(true,false);

        vout << "     done ! " << std::endl;

        vout << "     -- compute overlaps ... " << std::endl;

        meshInter1.computeOverlaps(true,false);

        vout << "     done ! " << std::endl;

        vout << "   done ! " << std::endl;

        vout << "   -- BG vs FG2 ... " << std::endl;

        MeshIntersection< mesh_Type , mesh_Type >
                         meshInter2(fullBGMeshCurPtr, fullFG2MeshCurPtr, interPoints);

        vout << "     -- compute intersections ... " << std::endl;

        meshInter2.computeIntersections(true,false);

        vout << "     done ! " << std::endl;

        vout << "     -- compute overlaps ... " << std::endl;

        meshInter2.computeOverlaps(true,false);

        vout << "     done ! " << std::endl;

        vout << "   done ! " << std::endl;

        localChrono.stop();

        vout << " done in " << localChrono.diff() << " s!" << std::endl;

        // +-----------------------------------------------+
        // |              Cut-Mesh generation              |
        // +-----------------------------------------------+

        localChrono.reset();
        localChrono.start();

        vout << " -- Cut elements ... " << std::endl;

        std::map< UInt , meshPtr_Type > cuttingMeshes(
            {
              { fullFG1MeshCurPtr->id() , fullFG1MeshCurPtr } ,
              { fullFG2MeshCurPtr->id() , fullFG2MeshCurPtr }
            }
            );

        CutElements cutElem(fullBGMeshCurPtr, interPoints, cuttingMeshes);

        vout << "   -- Build cut elements ... " << std::endl;

        cutElem.buildCutElements();

        vout << "   done ! " << std::endl;

        vout << "   -- Assign global IDs ... " << std::endl;

        cutElem.assignGlobalIds();

        vout << "   done ! " << std::endl;

        vout << "   -- Compute regions ... " << std::endl;

        cutElem.computeRegions(CutElements::ExtendedMethodType::XFEM,
                               CutElements::ContinuityType::Continuous);

        interPoints->clearPointToEntity();

        vout << "   done ! " << std::endl;

        localChrono.stop();

        vout << " done in " << localChrono.diff() << " s!" << std::endl;

        // +-----------------------------------------------+
        // |               Mesh partitioning               |
        // +-----------------------------------------------+

        localChrono.reset();
        localChrono.start();

        vout << " -- Partitioning the Current BG mesh ... " << std::endl;

        // It is in the current configuration
        meshPtr_Type bgMeshCurPtr;
        {
            MeshPartitionerIntersection< mesh_Type > bgMeshPart;
            bgMeshPart.setPartitionOverlap ( 1 );
            bgMeshPart.doPartition ( fullBGMeshCurPtr, Comm, interPoints );
            bgMeshCurPtr = bgMeshPart.meshPartition();
            interPoints = bgMeshPart.intersectionPointPartition();
            bgMeshCurPtr->setId(0);
            bgMeshCurPtr->setLocalId(myPID);
        }

        // Free global mesh
        fullBGMeshCurPtr.reset();

        localChrono.stop();

        vout << " done in " << localChrono.diff() << " s!" << std::endl;

        localChrono.reset();
        localChrono.start();

        vout << " -- Partitioning the Current FG1 mesh ... " << std::endl;

        // Vector of all the meshes of the partition in the current configuration
        std::vector< meshPtr_Type > fg1MeshCurPtr( nproc , nullptr );
        {
            MeshPartitioner< mesh_Type > fg1MeshPart;
            fg1MeshPart.setPartitionOverlap ( 1 );
            fg1MeshPart.setCleanUp ( false );
            fg1MeshPart.setup ( nproc , Comm );
            fg1MeshPart.doPartition ( fullFG1MeshCurPtr, Comm );
            // Get the part of the mesh of the current processor
            fg1MeshCurPtr[myPID] = fg1MeshPart.meshPartition(myPID);
            fg1MeshCurPtr[myPID]->setId(1);
            fg1MeshCurPtr[myPID]->setLocalId(myPID);
            fg1MeshPart.updateAdjacencies( bgMeshCurPtr );
            fg1MeshPart.cleanUp();
        }

        // Free global mesh
        fullFG1MeshCurPtr.reset();

        localChrono.stop();

        vout << " done in " << localChrono.diff() << " s!" << std::endl;

        localChrono.reset();
        localChrono.start();

        vout << " -- Partitioning the Current FG2 mesh ... " << std::endl;

        // Vector of all the meshes of the partition in the current configuration
        std::vector< meshPtr_Type > fg2MeshCurPtr( nproc , nullptr );
        {
            MeshPartitioner< mesh_Type > fg2MeshPart;
            fg2MeshPart.setPartitionOverlap ( 1 );
            fg2MeshPart.setCleanUp ( false );
            fg2MeshPart.setup ( nproc , Comm );
            fg2MeshPart.doPartition ( fullFG2MeshCurPtr, Comm );
            // Get the part of the mesh of the current processor
            fg2MeshCurPtr[myPID] = fg2MeshPart.meshPartition(myPID);
            fg2MeshCurPtr[myPID]->setId(2);
            fg2MeshCurPtr[myPID]->setLocalId(myPID);
            fg2MeshPart.updateAdjacencies( bgMeshCurPtr );
            fg2MeshPart.cleanUp();
        }

        // Free global mesh
        fullFG2MeshCurPtr.reset();

        localChrono.stop();

        vout << " done in " << localChrono.diff() << " s!" << std::endl;

        localChrono.reset();
        localChrono.start();

        vout << " -- Check partitioned meshes ... " << std::endl;

        if ( bgMeshCurPtr->check(0,false,false) )
        {
            std::cout << "PID " << myPID << " something wrong with BG part mesh. " << std::endl;
        }
        if ( fg1MeshCurPtr[myPID]->check(0,false,false) )
        {
            std::cout << "PID " << myPID << " something wrong with FG1 part mesh. " << std::endl;
        }
        if ( fg2MeshCurPtr[myPID]->check(0,false,false) )
        {
            std::cout << "PID " << myPID << " something wrong with FG2 part mesh. " << std::endl;
        }

        localChrono.stop();

        vout << " done in " << localChrono.diff() << " s!" << std::endl;

        localChrono.reset();
        localChrono.start();

        vout << " -- Generate cut mesh ... " << std::endl;

        std::vector<UInt> parentId;

        // It is in the current configuration
        meshPtr_Type cutMeshPtr =
            cutElem.generateCutMesh(bgMeshCurPtr, interPoints, &parentId);

        if ( cutMeshPtr->check(0,false,false) )
        {
            std::cout << "PID " << myPID << " something wrong with the cutMesh part mesh. " << std::endl;
        }
        
        localChrono.stop();

        vout << " done in " << localChrono.diff() << " s!" << std::endl;

        // +-----------------------------------------------+
        // |            Building Fluid FE spaces           |
        // +-----------------------------------------------+

        localChrono.reset();
        localChrono.start();

        vout << " -- Building the fluid finite elements spaces ... " << std::endl;

        // Defining standard and ET finite element spaces:
        // - for fluid
        uXSpacePtr_Type uXFESpace ( new uXSpace_Type ( bgMeshCurPtr,
                                                       dataFile ( "finite_element/degree_fluid", "P1" ),
                                                       3,
                                                       Comm ) );

        uXSpaceETAPtr_Type ETuXFESpace ( new uXSpaceETA_Type ( bgMeshCurPtr,
                                                           & ( uXFESpace->refFE() ),
                                                           & ( uXFESpace->fe().geoMap() ),
                                                               Comm ) );

        pXSpacePtr_Type pXFESpace ( new pXSpace_Type ( bgMeshCurPtr,
                                                       dataFile ( "finite_element/degree_fluid", "P1" ),
                                                       1,
                                                       Comm ) );

        pXSpaceETAPtr_Type ETpXFESpace ( new pXSpaceETA_Type ( bgMeshCurPtr,
                                                           & ( pXFESpace->refFE() ),
                                                           & ( pXFESpace->fe().geoMap() ),
                                                               Comm ) );

        localChrono.stop();

        vout << " done in " << localChrono.diff() << " s!" << std::endl;
        vout << " | # DOFS: "
             << 3 * uXFESpace->dof().numTotalDof() << " + "
             << pXFESpace->dof().numTotalDof() << " + "
             << 3 * dSolid1FESpace->dof().numTotalDof() << " + "
             << 3 * dSolid2FESpace->dof().numTotalDof() << " | " << std::endl;

        // +-----------------------------------------------+
        // |              Initialize solution              |
        // +-----------------------------------------------+

        localChrono.reset();
        localChrono.start();

        vout << " -- Initialize solution ... " << std::endl;

        // Declaring and filling union map
        mapPtr_Type unionMap;
        unionMap.reset ( new map_Type( uXFESpace->map() ) );
        *(unionMap) += pXFESpace->map();
        *(unionMap) += dSolid1FESpace->map();
        *(unionMap) += dSolid2FESpace->map();

        // Create solution vector
        blockVectorPtr_Type ETsolution
            ( new blockVector_Type ( ETuXFESpace->map() |
                                     ETpXFESpace->map() |
                                     ETdSolid1FESpace->map() |
                                     ETdSolid2FESpace->map() , Unique ) );
        // Clear solution vector
        *ETsolution *= 0.0;

        blockVectorPtr_Type ETsolutionRepeated;
        ETsolutionRepeated.reset ( new blockVector_Type ( *ETsolution , Repeated ) );

        // Create fluid solution
        velocitySol.reset ( new vector_Type ( ETuXFESpace->map() , Repeated) );
        pressureSol.reset ( new vector_Type ( ETpXFESpace->map() , Repeated) );

        // Clear velocity and pressure on the fluid mesh
        *velocitySol *= 0.0;
        *pressureSol *= 0.0;

        // Clear vectors on the solid mesh
        *dispSol1 *= 0.0;
        *solidVel1 *= 0.0;
        *dispSol2 *= 0.0;
        *solidVel2 *= 0.0;

        localChrono.stop();

        vout << " done in " << localChrono.diff() << " s!" << std::endl;

        // +-----------------------------------------------+
        // |         Initialize Post-Processing            |
        // +-----------------------------------------------+

        localChrono.reset();
        localChrono.start();

        vout << " -- Initialize Post-processing ... " << std::endl;

        // Export on the fluid restart space
        ss.str( std::string() );
        ss << dataFile ( "exporter/filename", "inter");
        ss << "RestartF_";
        ss << std::setfill('0') << std::setw(tPadding) << timeIter;
        ss << "_";
        ss << std::setfill('0') << std::setw(nlPadding) << nlIter;
        const std::string expRestartFluidFileName = ss.str();

        ExporterHDF5XFEM< mesh_Type > exporterRestartF ( dataFile, bgMeshCurPtr, expRestartFluidFileName, myPID, pXFESpace->dofPtr() );
        exporterRestartF.setMultimesh ( false );

        exporterRestartF.addVariable ( ExporterData< mesh_Type , DOFXfem >::VectorField,
                                       "velocity",
                                       uXFESpace,
                                       velocitySol,
                                       UInt ( 0 ) );

        // Export on the solid restart space
        ss.str( std::string() );
        ss << dataFile ( "exporter/filename", "inter");
        ss << "RestartS1_";
        ss << std::setfill('0') << std::setw(tPadding) << timeIter;
        ss << "_";
        ss << std::setfill('0') << std::setw(nlPadding) << nlIter;
        const std::string expRestartSolid1FileName = ss.str();

        ExporterHDF5< mesh_Type > exporterRestartS1 ( dataFile, fg1MeshRefPtr[myPID], expRestartSolid1FileName, myPID );
        exporterRestartS1.setMultimesh ( false );

        exporterRestartS1.addVariable ( ExporterData< mesh_Type >::VectorField,
                                        "displacement",
                                        dSolid1FESpace,
                                        dispSol1,
                                        UInt ( 0 ) );

        // Export on the solid restart space
        ss.str( std::string() );
        ss << dataFile ( "exporter/filename", "inter");
        ss << "RestartS2_";
        ss << std::setfill('0') << std::setw(tPadding) << timeIter;
        ss << "_";
        ss << std::setfill('0') << std::setw(nlPadding) << nlIter;
        const std::string expRestartSolid2FileName = ss.str();

        ExporterHDF5< mesh_Type > exporterRestartS2 ( dataFile, fg2MeshRefPtr[myPID], expRestartSolid2FileName, myPID );
        exporterRestartS2.setMultimesh ( false );

        exporterRestartS2.addVariable ( ExporterData< mesh_Type >::VectorField,
                                        "displacement2",
                                        dSolid2FESpace,
                                        dispSol2,
                                        UInt ( 0 ) );

        localChrono.stop();

        vout << " done in " << localChrono.diff() << " s!" << std::endl;

        // +-----------------------------------------------+
        // |          Setting Initial Condition            |
        // +-----------------------------------------------+

        localChrono.reset();
        localChrono.start();

        vout << " -- Set initial conditions ... " << std::endl;

        // Fluid velocity
        vectorPtr_Type initU ( new vector_Type ( ETuXFESpace->map() , Repeated) );
        initU->subset( *ETsolutionRepeated , 0 );

        // Solid displacement (velocity, acceleration)
        vectorPtr_Type initD1 ( new vector_Type ( ETdSolid1FESpace->map() , Repeated) );
        initD1->subset( *ETsolutionRepeated , 3 * uXFESpace->dof().numTotalDof() +
                                                  pXFESpace->dof().numTotalDof() );

        vectorPtr_Type initD2 ( new vector_Type ( ETdSolid2FESpace->map() , Repeated) );
        initD2->subset( *ETsolutionRepeated , 3 * uXFESpace->dof().numTotalDof() +
                                                  pXFESpace->dof().numTotalDof() +
                                              3 * dSolid1FESpace->dof().numTotalDof() );


        if (fluidTimeAdvanceMethod == "BDF")
        {
            timeAdvanceFluidPtr->setInitialCondition ( *initU );
        }
        if (fluidTimeAdvanceMethod == "Newmark")
        {
            timeAdvanceFluidPtr->setInitialCondition ( *initU, *initU, *initU );
        }

        if (solidTimeAdvanceMethod == "BDF")
        {
            timeAdvanceSolid1Ptr->setInitialCondition ( *initD1 );
            timeAdvanceSolid2Ptr->setInitialCondition ( *initD2 );
        }
        if (solidTimeAdvanceMethod == "Newmark")
        {
            timeAdvanceSolid1Ptr->setInitialCondition ( *initD1, *initD1, *initD1 );
            timeAdvanceSolid2Ptr->setInitialCondition ( *initD2, *initD2, *initD2 );
        }

        // Set old non-linear velocity
        oldNlVelocity.reset ( new vector_Type ( ETuXFESpace->map() , Repeated) );
        *oldNlVelocity = *velocitySol;

        oldNlBGMeshPtr = bgMeshCurPtr;
        oldNlUXFESpace = uXFESpace;

        // Set old time velocity
        oldTimeVelocity.reset ( new vector_Type ( ETuXFESpace->map() , Repeated) );
        *oldTimeVelocity = *velocitySol;

        oldTimeBGMeshPtr = oldNlBGMeshPtr;
        oldTimeUXFESpace = oldNlUXFESpace;

        // Set old non-linear displacement
        *oldNlDisp1 = *dispSol1;
        *oldNlDisp2 = *dispSol2;

        localChrono.stop();

        vout << " done in " << localChrono.diff() << " s!" << std::endl;

        // +-----------------------------------------------+
        // |            Exporting Initialization           |
        // +-----------------------------------------------+

        localChrono.reset();
        localChrono.start();

        vout << " -- Exporting ... " << std::endl;

        exporterRestartF.postProcess ( time );
        exporterRestartS1.postProcess ( time );
        exporterRestartS2.postProcess ( time );

        localChrono.stop();

        vout << " done in " << localChrono.diff() << " s!" << std::endl;

        exporterRestartF.closeFile();
        exporterRestartS1.closeFile();
        exporterRestartS2.closeFile();

        time += dt;
        ++timeIter;
    }

    timeChrono.stop();

    vout << std::endl << " Time step done in " << timeChrono.diff() << " s!" << std::endl;

    initChrono.stop();

    vout << std::endl << " Initialization done in " << initChrono.diff() << " s!" << std::endl << std::endl;

    // +-----------------------------------------------+   
    // +-----------------------------------------------+
    // |                 TIME LOOP                     |
    // +-----------------------------------------------+
    // +-----------------------------------------------+

    bool repeat = false;

    // Solution on the entire FG mesh
    vectorPtr_Type fullOldNlDisp1 ( new vector_Type ( ETdSolid1FESpace->map() , Repeated) );
    // Solution on the entire FG mesh
    vectorPtr_Type fullOldNlDisp2 ( new vector_Type ( ETdSolid2FESpace->map() , Repeated) );

    // Loop in time
    for ( ; time <= T+dt/2; time += dt, ++timeIter)
    {
        vout << "    ------------------------ " << std::endl;
        vout << "    --     At time " << time << " ... " << std::endl;
        vout << "    ------------------------ " << std::endl;

        timeChrono.reset();
        timeChrono.start();

        nlConverged = false;

        // Save the stecil at the previous time
        for ( UInt i = 0; i < stencilSize; ++i )
        {
            // Create new vector
            oldTimeStencilVelocity[i] = new vector_Type ( oldTimeUXFESpace->map() , Repeated );
            // Save the old fluid solution
            *oldTimeStencilVelocity[i] = *(timeAdvanceFluidPtr->stencil()[i]);
        }

        // Internal loop for solving the non-linearities (fluid, solid and geometry)
        do
        {
            vout << "        ----------------------------- " << std::endl;
            vout << "        --  Internal iteration " << nlIter << " ... " << std::endl;
            vout << "        ----------------------------- " << std::endl;
            eout << "Time " << time << " Iter " << nlIter << std::endl;

            nlChrono.reset();
            nlChrono.start();

            localChrono.reset();
            localChrono.start();

            vout << " -- Copy meshes ... " << std::endl;

            vout << "   -- Copy the BG mesh ..." << std::endl;

            // Store the current BG mesh for assembling
            meshPtr_Type fullBGMeshCurPtr ( new mesh_Type ( *fullBGMeshPoolPtr ) );

            vout << "   -- Copy the FG1 mesh ..." << std::endl;

            // Store the current FG mesh in the current configuration just for intersections
            meshPtr_Type fullFG1MeshCurPtr ( new mesh_Type ( *fullFG1MeshRefPtr ) );

            vout << "   -- Copy the FG2 mesh ..." << std::endl;

            // Store the current FG mesh in the current configuration just for intersections
            meshPtr_Type fullFG2MeshCurPtr ( new mesh_Type ( *fullFG2MeshRefPtr ) );

            localChrono.stop();

            vout << " done in " << localChrono.diff() << " s!" << std::endl;

            localChrono.reset();
            localChrono.start();

            vout << " -- Move Solid Mesh ... " << std::endl;

            if ( repeat == false )
            {
                fullOldNlDisp1.reset( new vector_Type ( ETdSolid1FESpace->map() , Repeated) );
                fullOldNlDisp2.reset( new vector_Type ( ETdSolid2FESpace->map() , Repeated) );

                // In order to move the full solid mesh, it is required the full solution.
                // So, I have to gather the partitioned solution and then linearized it.

                // Set combine mode to Insert
                oldNlDisp1->setCombineMode(Insert);

                // Gather the partitioned oldNlDisp1 and fill fullOldNlDisp1.
                for ( Int k = 0; k < nproc; ++k )
                {
                    if ( static_cast<UInt>(k) != myPID )
                    {
                        vectorPtr_Type tmpFullOldNlDisp1 ( new vector_Type ( *oldNlDisp1 , k ) );
                    }
                    else
                    {
                        fullOldNlDisp1.reset( new vector_Type ( *oldNlDisp1 , k ) );
                    }
                }

                // Set combine mode to Insert
                oldNlDisp2->setCombineMode(Insert);

                // Gather the partitioned oldNlDisp2 and fill fullOldNlDisp2.
                for ( Int k = 0; k < nproc; ++k )
                {
                    if ( static_cast<UInt>(k) != myPID )
                    {
                        vectorPtr_Type tmpFullOldNlDisp2 ( new vector_Type ( *oldNlDisp2 , k ) );
                    }
                    else
                    {
                        fullOldNlDisp2.reset( new vector_Type ( *oldNlDisp2 , k ) );
                    }
                }

                // Restore the default combine mode (Add)
                oldNlDisp1->setCombineMode(Add);
                oldNlDisp2->setCombineMode(Add);
            }

            // Serial communicator
            std::shared_ptr<Epetra_Comm> SerialComm (new Epetra_MpiComm (MPI_COMM_SELF) );

            // Full solid FESpace
            dSpacePtr_Type dFullSolid1FESpace
                ( new dSpace_Type ( fullFG1MeshCurPtr,
                                    dataFile ( "finite_element/degree_solid", "P1" ),
                                    3,
                                    SerialComm ) );

            dSpacePtr_Type dFullSolid2FESpace
                ( new dSpace_Type ( fullFG2MeshCurPtr,
                                    dataFile ( "finite_element/degree_solid", "P1" ),
                                    3,
                                    SerialComm ) );

            // Linearization of the full solid displacement
            dSpacePtr_Type lFullSolid1FESpace ( new dSpace_Type ( fullFG1MeshCurPtr,
                                                                  "P1",
                                                                  3,
                                                                  SerialComm ) );

            vectorPtr_Type linearFullDispSol1 ( new vector_Type ( lFullSolid1FESpace->map() , Repeated) );

            dSpacePtr_Type lFullSolid2FESpace ( new dSpace_Type ( fullFG2MeshCurPtr,
                                                                  "P1",
                                                                  3,
                                                                  SerialComm ) );

            vectorPtr_Type linearFullDispSol2 ( new vector_Type ( lFullSolid2FESpace->map() , Repeated) );

            if ( fgDegree == "P1" )
            {
                *linearFullDispSol1 = *fullOldNlDisp1;
                *linearFullDispSol2 = *fullOldNlDisp2;
            }
            else
            {
                *linearFullDispSol1 = lFullSolid1FESpace->feToFEInterpolate( *dFullSolid1FESpace,
                                                                             *fullOldNlDisp1,
                                                                             Repeated );
                *linearFullDispSol2 = lFullSolid2FESpace->feToFEInterpolate( *dFullSolid2FESpace,
                                                                             *fullOldNlDisp2,
                                                                             Repeated );
            }

            if ( movementEnabled )
            {
                // Move the solid mesh to the current configuration
                fullFG1MeshCurPtr->meshTransformer().simpleMoveMesh
                    ( *linearFullDispSol1 , lFullSolid1FESpace->dof().numTotalDof() );

                // Check that the mesh has no inverted elements
                if ( myPID == 0 )
                {
                    std::vector<bool> elSign;
                    Switch sw;
                    (void) checkVolumes ( *fullFG1MeshCurPtr, elSign, sw );
                    if ( sw.test ( "HAS_NEGATIVE_VOLUMES" ) )
                    {
                        eout << " ERROR! FG1 mesh has inverted elements. Exit." << std::endl;
                        return ( EXIT_FAILURE );
                    }
                }

                // Move the solid mesh to the current configuration
                fullFG2MeshCurPtr->meshTransformer().simpleMoveMesh
                    ( *linearFullDispSol2 , lFullSolid2FESpace->dof().numTotalDof() );

                // Check that the mesh has no inverted elements
                if ( myPID == 0 )
                {
                    std::vector<bool> elSign;
                    Switch sw;
                    (void) checkVolumes ( *fullFG2MeshCurPtr, elSign, sw );
                    if ( sw.test ( "HAS_NEGATIVE_VOLUMES" ) )
                    {
                        eout << " ERROR! FG2 mesh has inverted elements. Exit." << std::endl;
                        return ( EXIT_FAILURE );
                    }
                }
            }

            // Free linearization spaces and vectors
            lFullSolid1FESpace.reset();
            lFullSolid2FESpace.reset();
            linearFullDispSol1.reset();
            linearFullDispSol2.reset();
            localChrono.stop();

            vout << " done in " << localChrono.diff() << " s!" << std::endl;

            // +-----------------------------------------------+
            // |              Mesh intersection                |
            // +-----------------------------------------------+

            localChrono.reset();
            localChrono.start();

            vout << " -- Mesh intersection ... " << std::endl;

            interPointsPtr_Type interPoints ( new IntersectionPoints() );

            vout << "   -- BG vs FG1 ... " << std::endl;

            MeshIntersection< mesh_Type , mesh_Type >
                             meshInter1(fullBGMeshCurPtr, fullFG1MeshCurPtr, interPoints);

            vout << "     -- compute intersections ... " << std::endl;

            meshInter1.computeIntersections(true,false);

            vout << "     done ! " << std::endl;

            vout << "     -- compute overlaps ... " << std::endl;

            meshInter1.computeOverlaps(true,false);

            vout << "     done ! " << std::endl;

            vout << "   done ! " << std::endl;

            vout << "   -- BG vs FG2 ... " << std::endl;

            MeshIntersection< mesh_Type , mesh_Type >
                             meshInter2(fullBGMeshCurPtr, fullFG2MeshCurPtr, interPoints);

            vout << "     -- compute intersections ... " << std::endl;

            meshInter2.computeIntersections(true,false);

            vout << "     done ! " << std::endl;

            vout << "     -- compute overlaps ... " << std::endl;

            meshInter2.computeOverlaps(true,false);

            vout << "     done ! " << std::endl;

            vout << "   done ! " << std::endl;

            localChrono.stop();

            vout << " done in " << localChrono.diff() << " s!" << std::endl;

            // +-----------------------------------------------+
            // |              Cut-Mesh generation              |
            // +-----------------------------------------------+

            localChrono.reset();
            localChrono.start();

            vout << " -- Cut elements ... " << std::endl;

            std::map< UInt , meshPtr_Type > cuttingMeshes(
                {
                  { fullFG1MeshCurPtr->id() , fullFG1MeshCurPtr } ,
                  { fullFG2MeshCurPtr->id() , fullFG2MeshCurPtr } 
                }
                );

            CutElements cutElem(fullBGMeshCurPtr, interPoints, cuttingMeshes);

            vout << "   -- Build cut elements ... " << std::endl;

            bool resCutElem;
            resCutElem = cutElem.buildCutElements();

            vout << "   done ! " << std::endl;

            // successful subtetrahedralization
            if (resCutElem == true)
            {
                vout << "   -- Assign global IDs ... " << std::endl;

                cutElem.assignGlobalIds();

                vout << "   done ! " << std::endl;

                vout << "   -- Compute regions ... " << std::endl;

                resCutElem = cutElem.computeRegions(CutElements::ExtendedMethodType::XFEM,
                                       CutElements::ContinuityType::Continuous);

                interPoints->clearPointToEntity();

                vout << "   done ! " << std::endl;
            }

            if (resCutElem == false)
            {
                vout << "WARNING: Failed mesh intersection. PERTURB and RETRY." << std::endl;

                repeat = true;

                vout << " -- Add perturbation to the solution fullOldNlDisp1 ... " << std::endl;
                vout << "Pre fullOldNlDisp1: max = " << fullOldNlDisp1->maxValue()
                        << " min = " << fullOldNlDisp1->minValue() << std::endl;
                *fullOldNlDisp1 += perturbCoeff;
                vout << "Post fullOldNlDisp1: max = " << fullOldNlDisp1->maxValue()
                        << " min = " << fullOldNlDisp1->minValue() << std::endl << std::flush;

                vout << " -- Add perturbation to the solution fullOldNlDisp2 ... " << std::endl << std::flush;
                vout << "Pre fullOldNlDisp2: max = " << fullOldNlDisp2->maxValue()
                        << " min = " << fullOldNlDisp2->minValue() << std::endl << std::flush;
                *fullOldNlDisp2 += perturbCoeff;
                vout << "Post fullOldNlDisp2: max = " << fullOldNlDisp2->maxValue()
                        << " min = " << fullOldNlDisp2->minValue() << std::endl << std::flush;

                interPoints->clearPointToEntity();
                fullBGMeshCurPtr.reset();
                fullFG1MeshCurPtr.reset();
                fullFG2MeshCurPtr.reset();

                localChrono.stop();
                nlChrono.stop();

                MPI_Barrier (MPI_COMM_WORLD);

                nlConverged = false;
                //exit from internal loop
                continue;
            }
            else
            {
                repeat = false;
            }

            localChrono.stop();

            vout << " done in " << localChrono.diff() << " s!" << std::endl;

            // +-----------------------------------------------+
            // |               Mesh partitioning               |
            // +-----------------------------------------------+

            localChrono.reset();
            localChrono.start();

            vout << " -- Partitioning the Current BG mesh ... " << std::endl;

            // It is in the current configuration
            meshPtr_Type bgMeshCurPtr;
            {
                MeshPartitionerIntersection< mesh_Type > bgMeshPart;
                bgMeshPart.setPartitionOverlap ( 1 );
                bgMeshPart.doPartition ( fullBGMeshCurPtr, Comm, interPoints );
                bgMeshCurPtr = bgMeshPart.meshPartition();
                interPoints = bgMeshPart.intersectionPointPartition();
                bgMeshCurPtr->setId(0);
                bgMeshCurPtr->setLocalId(myPID);
            }

            // Free global mesh
            fullBGMeshCurPtr.reset();

            localChrono.stop();

            vout << " done in " << localChrono.diff() << " s!" << std::endl;

            localChrono.reset();
            localChrono.start();

            vout << " -- Partitioning the Current FG1 mesh ... " << std::endl;

            // Vector of all the meshes of the partition
            std::vector< meshPtr_Type > fg1MeshCurPtr( nproc , nullptr );
            {
                MeshPartitioner< mesh_Type > fg1MeshPart;
                fg1MeshPart.setPartitionOverlap ( 1 );
                fg1MeshPart.setCleanUp ( false );
                fg1MeshPart.setup ( nproc , Comm );
                fg1MeshPart.doPartition ( fullFG1MeshCurPtr, Comm );
                // Get all the parts of the mesh
                for ( Int k = 0; k < nproc; ++k )
                {
                    fg1MeshCurPtr[k] = fg1MeshPart.meshPartition(k);
                    fg1MeshCurPtr[k]->setId(1);
                    fg1MeshCurPtr[k]->setLocalId(k);
                }
                fg1MeshPart.updateAdjacencies( bgMeshCurPtr );
                fg1MeshPart.cleanUp();
            }

            // Free global mesh
            fullFG1MeshCurPtr.reset();

            localChrono.stop();

            vout << " done in " << localChrono.diff() << " s!" << std::endl;

            localChrono.reset();
            localChrono.start();

            vout << " -- Partitioning the Current FG2 mesh ... " << std::endl;

            // Vector of all the meshes of the partition in the current configuration
            std::vector< meshPtr_Type > fg2MeshCurPtr( nproc , nullptr );
            {
                MeshPartitioner< mesh_Type > fg2MeshPart;
                fg2MeshPart.setPartitionOverlap ( 1 );
                fg2MeshPart.setCleanUp ( false );
                fg2MeshPart.setup ( nproc , Comm );
                fg2MeshPart.doPartition ( fullFG2MeshCurPtr, Comm );
                // Get all the parts of the mesh
                for ( Int k = 0; k < nproc; ++k )
                {
                    fg2MeshCurPtr[k] = fg2MeshPart.meshPartition(k);
                    fg2MeshCurPtr[k]->setId(2);
                    fg2MeshCurPtr[k]->setLocalId(k);
                }
                fg2MeshPart.updateAdjacencies( bgMeshCurPtr );
                fg2MeshPart.cleanUp();
            }

            // Free global mesh
            fullFG2MeshCurPtr.reset();

            localChrono.stop();

            vout << " done in " << localChrono.diff() << " s!" << std::endl;

            localChrono.reset();
            localChrono.start();

            vout << " -- Check partitioned meshes ... " << std::endl;

            if ( bgMeshCurPtr->check(0,false,false) )
            {
                std::cout << "PID " << myPID << " something wrong with BG part mesh. " << std::endl;
            }
            if ( fg1MeshCurPtr[myPID]->check(0,false,false) )
            {
                std::cout << "PID " << myPID << " something wrong with FG1 part mesh. " << std::endl;
            }
            if ( fg2MeshCurPtr[myPID]->check(0,false,false) )
            {
                std::cout << "PID " << myPID << " something wrong with FG2 part mesh. " << std::endl;
            }

            localChrono.stop();

            vout << " done in " << localChrono.diff() << " s!" << std::endl;

            localChrono.reset();
            localChrono.start();

            vout << " -- Generate cut mesh ... " << std::endl;

            std::vector<UInt> parentId;

            // It is in the current configuration
            meshPtr_Type cutMeshPtr =
                cutElem.generateCutMesh(bgMeshCurPtr, interPoints, &parentId);

            localChrono.stop();

            vout << " done in " << localChrono.diff() << " s!" << std::endl;

            // +-----------------------------------------------+
            // |            Building Fluid FE spaces           |
            // +-----------------------------------------------+

            localChrono.reset();
            localChrono.start();

            vout << " -- Building the fluid finite elements spaces ... " << std::endl;

            // Defining standard and ET finite element spaces:
            // - for fluid
            uXSpacePtr_Type uXFESpace ( new uXSpace_Type ( bgMeshCurPtr,
                                                           dataFile ( "finite_element/degree_fluid", "P1" ),
                                                           3,
                                                           Comm ) );

            uXSpaceETAPtr_Type ETuXFESpace ( new uXSpaceETA_Type ( bgMeshCurPtr,
                                                               & ( uXFESpace->refFE() ),
                                                               & ( uXFESpace->fe().geoMap() ),
                                                                   Comm ) );

            pXSpacePtr_Type pXFESpace ( new pXSpace_Type ( bgMeshCurPtr,
                                                           dataFile ( "finite_element/degree_fluid", "P1" ),
                                                           1,
                                                           Comm ) );

            pXSpaceETAPtr_Type ETpXFESpace ( new pXSpaceETA_Type ( bgMeshCurPtr,
                                                               & ( pXFESpace->refFE() ),
                                                               & ( pXFESpace->fe().geoMap() ),
                                                                   Comm ) );

            localChrono.stop();

            vout << " done in " << localChrono.diff() << " s!" << std::endl;
            vout << " | # DOFS: "
                 << 3 * uXFESpace->dof().numTotalDof() << " + "
                 << pXFESpace->dof().numTotalDof() << " + "
                 << 3 * dSolid1FESpace->dof().numTotalDof() << " + "
                 << 3 * dSolid2FESpace->dof().numTotalDof() << " | " << std::endl;

            // +-----------------------------------------------+
            // |              Initialize solution              |
            // +-----------------------------------------------+

            localChrono.reset();
            localChrono.start();

            vout << " -- Initialize solution ... " << std::endl;

            // Declaring and filling union map
            mapPtr_Type unionMap;
            unionMap.reset ( new map_Type( uXFESpace->map() ) );
            *(unionMap) += pXFESpace->map();
            *(unionMap) += dSolid1FESpace->map();
            *(unionMap) += dSolid2FESpace->map();

            // Create solution vector
            blockVectorPtr_Type ETsolution
                ( new blockVector_Type ( ETuXFESpace->map() |
                                         ETpXFESpace->map() |
                                         ETdSolid1FESpace->map() |
                                         ETdSolid2FESpace->map() , Unique ) );

            // Create fluid solution
            velocitySol.reset ( new vector_Type ( ETuXFESpace->map() , Repeated) );
            pressureSol.reset ( new vector_Type ( ETpXFESpace->map() , Repeated) );

            // Clear solution vector
            *ETsolution *= 0.0;

            // Clear velocity on the fluid mesh
            *velocitySol *= 0.0;
            *pressureSol *= 0.0;

            // Clear vectors on the solid mesh
            *dispSol1 *= 0.0;
            *solidVel1 *= 0.0;
            *dispSol2 *= 0.0;
            *solidVel2 *= 0.0;

            localChrono.stop();

            vout << " done in " << localChrono.diff() << " s!" << std::endl;

            // +-----------------------------------------------+
            // |         Initialize Exporting            |
            // +-----------------------------------------------+

            localChrono.reset();
            localChrono.start();

            vout << " -- Initialize Exporting ... " << std::endl;

            // Export on the fluid restart space
            ss.str( std::string() );
            ss << "fsiRestartF_";
            ss << std::setfill('0') << std::setw(tPadding) << timeIter;
            ss << "_";
            ss << std::setfill('0') << std::setw(nlPadding) << nlIter;
            const std::string expRestartFluidFileName = ss.str();

            ExporterHDF5XFEM< mesh_Type > exporterRestartF ( dataFile, bgMeshCurPtr, expRestartFluidFileName, myPID, pXFESpace->dofPtr() );
            exporterRestartF.setMultimesh ( false );

            exporterRestartF.addVariable ( ExporterData< mesh_Type , DOFXfem >::VectorField,
                                           "velocity",
                                           uXFESpace,
                                           velocitySol,
                                           UInt ( 0 ) );

            // Export on the solid restart space
            ss.str( std::string() );
            ss << "fsiRestartS1_";
            ss << std::setfill('0') << std::setw(tPadding) << timeIter;
            ss << "_";
            ss << std::setfill('0') << std::setw(nlPadding) << nlIter;
            const std::string expRestartSolid1FileName = ss.str();

            ExporterHDF5< mesh_Type > exporterRestartS1 ( dataFile, fg1MeshRefPtr[myPID], expRestartSolid1FileName, myPID );
            exporterRestartS1.setMultimesh ( false );

            exporterRestartS1.addVariable ( ExporterData< mesh_Type >::VectorField,
                                           "displacement",
                                           dSolid1FESpace,
                                           dispSol1,
                                           UInt ( 0 ) );

            // Export on the solid restart space
            ss.str( std::string() );
            ss << "fsiRestartS2_";
            ss << std::setfill('0') << std::setw(tPadding) << timeIter;
            ss << "_";
            ss << std::setfill('0') << std::setw(nlPadding) << nlIter;
            const std::string expRestartSolid2FileName = ss.str();

            ExporterHDF5< mesh_Type > exporterRestartS2 ( dataFile, fg2MeshRefPtr[myPID], expRestartSolid2FileName, myPID );
            exporterRestartS2.setMultimesh ( false );

            exporterRestartS2.addVariable ( ExporterData< mesh_Type >::VectorField,
                                            "displacement2",
                                            dSolid2FESpace,
                                            dispSol2,
                                            UInt ( 0 ) );

            localChrono.stop();

            vout << " done in " << localChrono.diff() << " s!" << std::endl;

            // +-------------------------------------------------------+
            // |    Interpolating from old to new BG mesh (in time)    |
            // +-------------------------------------------------------+

            localChrono.reset();
            localChrono.start();

            vout << " -- Interpolation from old to new BG mesh (in time) ... " << std::endl;

            // Inizialite the interpolated stencil
            stencil_Type interpSolU( stencilSize , nullptr );

            // Interpolation object between different FESpaces
            FESpaceInterpolation< uXSpace_Type , uXSpace_Type > feInterpOldToNewTime
                ( oldTimeUXFESpace, uXFESpace, Comm );

            // Loop over the stencil
            for (UInt i = 0; i < stencilSize; ++i)
            {
                // Interpolate from the old to the new fespace
                interpSolU[i] = new vector_Type ( uXFESpace->map() , Repeated );

                *interpSolU[i] = feInterpOldToNewTime.interpolateFirstToSecond
                    ( *oldTimeStencilVelocity[i], Repeated );
            }

            // Update the solution at the previous time on the current BG mesh
            timeAdvanceFluidPtr->changeState(interpSolU);

            localChrono.stop();

            vout << " done in " << localChrono.diff() << " s!" << std::endl;

            // +-----------------------------------------------+
            // |              Convective velocity              |
            // +-----------------------------------------------+

            localChrono.reset();
            localChrono.start();

            vout << " -- Compute convective velocity ... " << std::endl;

            // Convective velocity
            vectorPtr_Type velocityConvective
                ( new vector_Type ( ETuXFESpace->map() , Repeated) );

            // If it is the first non-linear iteration
            // or explicit treatment of the convection term
            if ( nlIter == 0 || !nlConvEnabled )
            {
                // use the extrapolation
                timeAdvanceFluidPtr->extrapolation ( *velocityConvective );
            }
            else // otherwise
            {
                // Interpolation object between different FESpaces
                FESpaceInterpolation< uXSpace_Type , uXSpace_Type > feInterpOldToNewNl
                    ( oldNlUXFESpace, uXFESpace, Comm );

                // use the velocity of the previous non-linear iteration
                *velocityConvective = feInterpOldToNewNl.interpolateFirstToSecond
                    ( *oldNlVelocity, Repeated );
            }

            localChrono.stop();

            vout << " done in " << localChrono.diff() << " s!" << std::endl;

            // +-----------------------------------------------+
            // |               Building matrices               |
            // +-----------------------------------------------+

            localChrono.reset();
            localChrono.start();

            vout << " -- Initialize matrices ... " << std::endl;

            // Declaring block matrix
            blockMatrixPtr_Type ETsystemMatrix
                ( new blockMatrix_Type ( ETuXFESpace->map() |
                                         ETpXFESpace->map() |
                                         ETdSolid1FESpace->map() |
                                         ETdSolid2FESpace->map()
                                         ) );

            blockMatrixPtr_Type ETmassFluidMatrix
                ( new blockMatrix_Type ( ETuXFESpace->map() |
                                         ETpXFESpace->map() |
                                         ETdSolid1FESpace->map() |
                                         ETdSolid2FESpace->map() 
                                         ) );

            blockMatrixPtr_Type ETmassSolidMatrix
                ( new blockMatrix_Type ( ETuXFESpace->map() |
                                         ETpXFESpace->map() |
                                         ETdSolid1FESpace->map() |
                                         ETdSolid2FESpace->map()
                                         ) );

            blockMatrixPtr_Type ETfluidMatrix
                ( new blockMatrix_Type ( ETuXFESpace->map() |
                                         ETpXFESpace->map() |
                                         ETdSolid1FESpace->map() |
                                         ETdSolid2FESpace->map() 
                                         ) );

            blockMatrixPtr_Type ETsolidMatrix
                ( new blockMatrix_Type ( ETuXFESpace->map() |
                                         ETpXFESpace->map() |
                                         ETdSolid1FESpace->map() |
                                         ETdSolid2FESpace->map()
                                         ) );

            blockMatrixPtr_Type ETweakFFMatrix
                ( new blockMatrix_Type ( ETuXFESpace->map() |
                                         ETpXFESpace->map() |
                                         ETdSolid1FESpace->map() |
                                         ETdSolid2FESpace->map()
                                         ) );

            blockMatrixPtr_Type ETweakFSMatrix
                ( new blockMatrix_Type ( ETuXFESpace->map() |
                                         ETpXFESpace->map() |
                                         ETdSolid1FESpace->map() |
                                         ETdSolid2FESpace->map()
                                         ) );

            *ETsystemMatrix *= 0.0;
            *ETmassFluidMatrix *= 0.0;
            *ETmassSolidMatrix *= 0.0;
            *ETfluidMatrix *= 0.0;
            *ETsolidMatrix *= 0.0;
            *ETweakFFMatrix *= 0.0;
            *ETweakFSMatrix *= 0.0;

            solidLaw1.setFESpaces( uXFESpace , pXFESpace , dSolid1FESpace ,
                                   ETuXFESpace , ETpXFESpace , ETdSolid1FESpace );
            couplingFSI1.setFESpaces( uXFESpace , pXFESpace , dSolid1FESpace ,
                                     ETuXFESpace , ETpXFESpace , ETdSolid1FESpace );
            couplingFSI2.setFESpaces( uXFESpace , pXFESpace , dSolid2FESpace ,
                                     ETuXFESpace , ETpXFESpace , ETdSolid2FESpace );

            localChrono.stop();

            vout << " done in " << localChrono.diff() << " s!" << std::endl;

            localChrono.reset();
            localChrono.start();

            using volumeList_Type = std::vector< mesh_Type::element_Type* >;
            using faceList_Type = std::vector< mesh_Type::facet_Type* >;
            using indexList_Type = std::vector<UInt>;
            using indexBGList_Type = std::vector<std::array<UInt,3>>;
            using indexFGList_Type = std::vector<UInt>;

            // BG owned elements
            std::shared_ptr< volumeList_Type > bgOwnedVolumeListPtr ( new volumeList_Type() );
            std::shared_ptr< indexList_Type > indexBGOwnedVolumeListPtr ( new indexList_Type() );

            for ( auto & volume : bgMeshCurPtr->volumeList )
            {
                if ( Flag<meshEntityFlag_Type>::testOneNotSet( volume.flag(), EntityFlags::GHOST ) )
                {
                    bgOwnedVolumeListPtr->push_back( &volume );
                    indexBGOwnedVolumeListPtr->push_back( volume.localId() );
                }
            }

            // FG1 owned elements
            std::shared_ptr< volumeList_Type > fg1OwnedVolumeListPtr ( new volumeList_Type() );
            std::shared_ptr< indexList_Type > indexFG1OwnedVolumeListPtr ( new indexList_Type() );

            for ( auto & volume : fg1MeshRefPtr[myPID]->volumeList )
            {
                if ( Flag<meshEntityFlag_Type>::testOneNotSet( volume.flag(), EntityFlags::GHOST ) )
                {
                    fg1OwnedVolumeListPtr->push_back( &volume );
                    indexFG1OwnedVolumeListPtr->push_back( volume.localId() );
                }
            }

            // FG2 owned elements
            std::shared_ptr< volumeList_Type > fg2OwnedVolumeListPtr ( new volumeList_Type() );
            std::shared_ptr< indexList_Type > indexFG2OwnedVolumeListPtr ( new indexList_Type() );

            for ( auto & volume : fg2MeshRefPtr[myPID]->volumeList )
            {
                if ( Flag<meshEntityFlag_Type>::testOneNotSet( volume.flag(), EntityFlags::GHOST ) )
                {
                    fg2OwnedVolumeListPtr->push_back( &volume );
                    indexFG2OwnedVolumeListPtr->push_back( volume.localId() );
                }
            }

            // Faces with discontinuity
            std::shared_ptr< faceList_Type > faceListPtr ( new faceList_Type() );
            std::shared_ptr< indexList_Type > indexFaceListPtr ( new indexList_Type() );

            for ( auto & face : bgMeshCurPtr->faceList )
            {
                if ( Flag<meshEntityFlag_Type>::testOneSet( face.flag(), EntityFlags::DISCONTINUITY ) &&
                     Flag<meshEntityFlag_Type>::testOneNotSet( face.flag(), EntityFlags::GHOST ) )
                {
                    faceListPtr->push_back( &face );
                    indexFaceListPtr->push_back( face.localId() );
                }
            }

            // Ghost faces
            std::shared_ptr< faceList_Type > faceGhostListPtr ( new faceList_Type() );
            std::shared_ptr< indexList_Type > indexFaceGhostListPtr ( new indexList_Type() );

            {
                std::set<UInt> ghostSet;

                for ( auto & elem : bgMeshCurPtr->volumeList )
                {
                    if ( Flag<meshEntityFlag_Type>::testOneSet( elem.flag(), EntityFlags::CUT ) )
                    {
                        for ( UInt i = 0; i < LinearTetra::S_numFaces; ++i )
                        {
                            const UInt faceID = bgMeshCurPtr->localFacetId( elem.localId() , i );
                            auto & face = bgMeshCurPtr->faceList[ faceID ];
                            if ( Flag<meshEntityFlag_Type>::testOneNotSet( face.flag(), EntityFlags::OVERLAP ) &&
                                 Flag<meshEntityFlag_Type>::testOneNotSet( face.flag(), EntityFlags::GHOST ) )
                            {
                                const bool res = ghostSet.insert(faceID).second;
                                if ( res == true )
                                {
                                    faceGhostListPtr->push_back( &face );
                                    indexFaceGhostListPtr->push_back( faceID );
                                }
                            }
                        }
                    }
                }
            }

#ifdef ENABLE_CIP
            // Interior faces
            std::shared_ptr< faceList_Type > faceInteriorListPtr ( new faceList_Type() );
            std::shared_ptr< indexList_Type > indexFaceInteriorListPtr ( new indexList_Type() );

            for ( auto & face : bgMeshCurPtr->faceList )
            {
                if ( !face.boundary() &&
                     Flag<meshEntityFlag_Type>::testOneNotSet( face.flag(), EntityFlags::GHOST ) )
                {
                    faceInteriorListPtr->push_back( &face );
                    indexFaceInteriorListPtr->push_back( face.localId() );
                }
            }
#endif // ENABLE_CIP

#ifdef ENABLE_BACKFLOW_STABILIZATION
            // Boundary faces
            std::shared_ptr< faceList_Type > faceBoundaryListPtr ( new faceList_Type() );
            std::shared_ptr< indexList_Type > indexFaceBoundaryListPtr ( new indexList_Type() );

            for ( auto & face : bgMeshCurPtr->faceList )
            {
                // List of flags to be considered
                std::set<UInt> flags = { OUTLET /* Fill with BG boundary marker */ };
                if ( flags.find( face.markerID() ) != std::end( flags ) &&
                     Flag<meshEntityFlag_Type>::testOneNotSet( face.flag(), EntityFlags::GHOST ) )
                {
                    faceBoundaryListPtr->push_back( &face );
                    indexFaceBoundaryListPtr->push_back( face.localId() );
                }
            }
#endif // ENABLE_BACKFLOW_STABILIZATION

            vout << " -- Assembling the matrices ... " << std::endl;

            vout << "   -- Assembling mass matrices ... " << std::endl;

            // Assembling fluid mass matrix
            {
                using namespace ExpressionAssembly;

                auto loopBGOwnedVolumes = integrationOverSelectedVolumes< mesh_Type >( bgOwnedVolumeListPtr , indexBGOwnedVolumeListPtr );

                // rho^f ( u , v )
                integrate (
                    loopBGOwnedVolumes,
                    uXFESpace->qr(),
                    ETuXFESpace,
                    ETuXFESpace,
                    value( rhoF * alphaF ) * dot ( phi_j , phi_i )
                )
                    >> ETmassFluidMatrix->block(0,0);
            }

            ETmassFluidMatrix->globalAssemble();
            *ETsystemMatrix += *ETmassFluidMatrix;
            ETmassFluidMatrix.reset();

            // Assembling solid mass matrices
            {
                using namespace ExpressionAssembly;

                auto loopFG1OwnedVolumes = integrationOverSelectedVolumes< mesh_Type >( fg1OwnedVolumeListPtr , indexFG1OwnedVolumeListPtr );
                auto loopFG2OwnedVolumes = integrationOverSelectedVolumes< mesh_Type >( fg2OwnedVolumeListPtr , indexFG2OwnedVolumeListPtr );

                // rho^s ( d , w )
                integrate (
                    loopFG1OwnedVolumes,
                    dSolid1FESpace->qr(),
                    ETdSolid1FESpace,
                    ETdSolid1FESpace,
                    value( rhoS * alphaS ) * dot ( phi_j , phi_i )
                )
                    >> ETmassSolidMatrix->block(2,2);
            }

            ETmassSolidMatrix->globalAssemble();
            *ETsystemMatrix += *ETmassSolidMatrix;
            ETmassSolidMatrix.reset();

            vout << "   -- Assembling linear fluid matrices ... " << std::endl;

            // Assembling stokes + pressure stab on BG mesh
            {
                using namespace ExpressionAssembly;

                auto loopBGOwnedVolumes = integrationOverSelectedVolumes< mesh_Type >( bgOwnedVolumeListPtr , indexBGOwnedVolumeListPtr );

                // 2 mu D(u) grad v
                integrate (
                    loopBGOwnedVolumes,
                    uXFESpace->qr(),
                    ETuXFESpace,
                    ETuXFESpace,
                    value( mu ) * dot ( grad ( phi_j ) + transpose ( grad ( phi_j ) ), grad ( phi_i ) )
                )
                    >> ETfluidMatrix->block(0,0);

                // - p div v
                integrate (
                    loopBGOwnedVolumes,
                    uXFESpace->qr(),
                    ETuXFESpace,
                    ETpXFESpace,
                    value( -1.0 ) * phi_j * div ( phi_i )
                )
                    >> ETfluidMatrix->block(0,1);

                // q div u
                integrate (
                    loopBGOwnedVolumes,
                    uXFESpace->qr(),
                    ETpXFESpace,
                    ETuXFESpace,
                    phi_i * div ( phi_j )
                )
                    >> ETfluidMatrix->block(1,0);

#ifndef ENABLE_CIP
                // Pressure stabilization Brezzi-Pitkaranta
                // gamma_p / mu h_K^2 grad p grad q
                integrate (
                    loopBGOwnedVolumes,
                    pXFESpace->qr(),
                    ETpXFESpace,
                    ETpXFESpace,
                    value( gamma_pres / mu ) * h_K * h_K * dot ( grad ( phi_j ) , grad ( phi_i ) ),
                    EvalDomain::Computational
                )
                    >> ETfluidMatrix->block(1,1);
#endif // ENABLE_CIP
            }

            ETfluidMatrix->globalAssemble();
            *ETsystemMatrix += *ETfluidMatrix;
            ETfluidMatrix.reset();

            vout << "   -- Assembling solid matrices ... " << std::endl;

            // Assembling elasticity operators on FG mesh
            {
                // J( \delta d , w )
                solidLaw1.computeJacobian ( ETsolidMatrix , *oldNlDisp1 /* dummy */ );
            }

            vout << "   -- Applying harmonic extension ... " << std::endl;

            // Assembling harmonic extension in FG2
            {
                using namespace ExpressionAssembly;

                auto loopFG2OwnedVolumes = integrationOverSelectedVolumes< mesh_Type >( fg2OwnedVolumeListPtr , indexFG2OwnedVolumeListPtr );

                // 1/2 D(d2) grad w2
                integrate (
                    loopFG2OwnedVolumes,
                    dSolid2FESpace->qr(),
                    ETdSolid2FESpace,
                    ETdSolid2FESpace,
                    0.5 * dot ( grad ( phi_j ) + transpose ( grad ( phi_j ) ), grad ( phi_i ) )
                )
                    >> ETsolidMatrix->block(3,3);    
            }

            ETsolidMatrix->globalAssemble();
            *ETsystemMatrix += *ETsolidMatrix;
            ETsolidMatrix.reset();

            vout << "   -- Assembling DG fluid matrices ... " << std::endl;

            auto loopFacesCont = ExpressionAssembly::interiorIntegrationOverSelectedFaces< mesh_Type >
                ( bgMeshCurPtr, faceListPtr, indexFaceListPtr );

            std::cout << "BG: (" << myPID << ")"
                      << " - Continuity faces: " << indexFaceListPtr->size() << std::endl;

            // Assembling weak continuity and ghost penalty in the BG mesh
            {
                using namespace ExpressionAssembly;

                auto loopFacesGhost = interiorIntegrationOverSelectedFaces< mesh_Type >
                    ( bgMeshCurPtr, faceGhostListPtr, indexFaceGhostListPtr );

                std::cout << "BG: (" << myPID << ")"
                          << " - Ghost faces: " << indexFaceGhostListPtr->size() << std::endl;

                // - ( {{ 2 mu D(u) }} n , [[ v ]] )
                interiorIntegrate (
                    loopFacesCont,
                    uXFESpace->bdQr(),
                    ETuXFESpace,
                    ETuXFESpace,
                    value( -1.0 ) * value( mu ) *
                        dot(
                            ( grad ( phi_j ) + transpose ( grad ( phi_j ) ) ) * Nface ,
                            phi_i ),
                    TraceOperator::Consistency
                )
                    >> ETweakFFMatrix->block(0,0);

                // ( {{ p }} , [[ v ]] n )
                interiorIntegrate (
                    loopFacesCont,
                    uXFESpace->bdQr(),
                    ETuXFESpace,
                    ETpXFESpace,
                    phi_j * dot ( phi_i , Nface ),
                    TraceOperator::Consistency
                )
                    >> ETweakFFMatrix->block(0,1);

#ifdef ENABLE_SYMM_TERMS
                // - ( [[ u ]] , {{ 2 mu D(v) }} n )
                interiorIntegrate (
                    loopFacesCont,
                    uXFESpace->bdQr(),
                    ETuXFESpace,
                    ETuXFESpace,
                    value( -1.0 ) * value( mu ) *
                        dot(
                            phi_j ,
                            ( grad ( phi_i ) + transpose ( grad ( phi_i ) ) ) * Nface ),
                    TraceOperator::Symmetrization
                )
                    >> ETweakFFMatrix->block(0,0);

                // - ( [[ u ]] n , {{ q }} )
                interiorIntegrate (
                    loopFacesCont,
                    uXFESpace->bdQr(),
                    ETpXFESpace,
                    ETuXFESpace,
                    value( -1.0 ) * dot( phi_j , Nface ) * phi_i,
                    TraceOperator::Symmetrization
                )
                    >> ETweakFFMatrix->block(1,0);
#endif // ENABLE_SYMM_TERMS

                // mu gamma_v / h_F ( [[ u ]] , [[ v ]] )
                interiorIntegrate (
                    loopFacesCont,
                    uXFESpace->bdQr(),
                    ETuXFESpace,
                    ETuXFESpace,
                    value( mu * gamma_virt ) / h_F * dot ( phi_j , phi_i ),
                    TraceOperator::Stabilization
                )
                    >> ETweakFFMatrix->block(0,0);

                // mu gamma_g h_F ( [[ grad(u) ]] n , [[ grad(v) ]] n )
                interiorIntegrate (
                    loopFacesGhost,
                    uXFESpace->bdQr(),
                    ETuXFESpace,
                    ETuXFESpace,
                    value( gamma_ghost * mu ) * h_F * dot( grad ( phi_j ) * Nface ,
                                                           grad ( phi_i ) * Nface ) ,
                    TraceOperator::GhostPenalty
                )
                    >> ETweakFFMatrix->block(0,0);
            }

            ETweakFFMatrix->globalAssemble();
            *ETsystemMatrix += *ETweakFFMatrix;
            ETweakFFMatrix.reset();

            vout << "   -- Assembling DG FS-interface matrices ... " << std::endl;

            // Solid FE Spaces of the k-th proc
            std::vector<dSpaceETAPtr_Type> ETdSolid1FESpace_vec(nproc, nullptr);
            std::vector<dSpaceETAPtr_Type> ETdSolid2FESpace_vec(nproc, nullptr);

            // Scalar current solid FE Spaces of the k-th proc
            std::vector<sSpacePtr_Type> sSolidCurFESpace_vec(nproc, nullptr);
            std::vector<sSpaceETAPtr_Type> ETsSolidCurFESpace_vec(nproc, nullptr);

            // Build the FE Space of k-th proc
            for ( Int k = 0; k < nproc; ++k )
            {
                const UInt part = (k+myPID)%nproc;

                dSpacePtr_Type dSolid1FESpace_k;
                dSolid1FESpace_k.reset ( new dSpace_Type ( fg1MeshRefPtr[part],
                                                           dataFile ( "finite_element/degree_fg", "P1" ),
                                                           3,
                                                           Comm ) );

                ETdSolid1FESpace_vec[part].reset
                    ( new dSpaceETA_Type ( fg1MeshRefPtr[part],
                                       & ( dSolid1FESpace_k->refFE() ),
                                       & ( dSolid1FESpace_k->fe().geoMap() ),
                                           Comm ) );


                sSolidCurFESpace_vec[k].reset
                    ( new sSpace_Type ( fg1MeshCurPtr[k],
                                        dataFile ( "finite_element/degree_fg", "P1" ),
                                        1,
                                        Comm ) );

                ETsSolidCurFESpace_vec[k].reset
                    ( new sSpaceETA_Type ( fg1MeshCurPtr[k],
                                       & ( sSolidCurFESpace_vec[k]->refFE() ),
                                       & ( sSolidCurFESpace_vec[k]->fe().geoMap() ),
                                           Comm ) );

                dSpacePtr_Type dSolid2FESpace_k;
                dSolid2FESpace_k.reset ( new dSpace_Type ( fg2MeshRefPtr[part],
                                                           dataFile ( "finite_element/degree_fg", "P1" ),
                                                           3,
                                                           Comm ) );

                ETdSolid2FESpace_vec[part].reset
                    ( new dSpaceETA_Type ( fg2MeshRefPtr[part],
                                       & ( dSolid2FESpace_k->refFE() ),
                                       & ( dSolid2FESpace_k->fe().geoMap() ),
                                           Comm ) );
            }

            // Old non-linear displacement of the k-th proc
            std::vector<vectorPtr_Type> oldNlDisp1_vec(nproc, nullptr);
            std::vector<vectorPtr_Type> oldNlDisp2_vec(nproc, nullptr);

            for ( Int k = 0; k < nproc; ++k )
            {
                mapPtr_Type solid1Map_k ( new map_Type ( ETdSolid1FESpace_vec[k]->map() ) );
                oldNlDisp1_vec[k].reset ( new vector_Type ( solid1Map_k , Repeated ) );

                for ( UInt gid = 0; gid < fullOldNlDisp1->map().mapSize(); ++gid )
                {
                    if ( oldNlDisp1_vec[k]->isGlobalIDPresent(gid) )
                    {
                        oldNlDisp1_vec[k]->setCoefficient (gid, (*fullOldNlDisp1)[gid]);
                    }
                }

                mapPtr_Type solid2Map_k ( new map_Type ( ETdSolid2FESpace_vec[k]->map() ) );
                oldNlDisp2_vec[k].reset ( new vector_Type ( solid2Map_k , Repeated ) );

                for ( UInt gid = 0; gid < fullOldNlDisp2->map().mapSize(); ++gid )
                {
                    if ( oldNlDisp2_vec[k]->isGlobalIDPresent(gid) )
                    {
                        oldNlDisp2_vec[k]->setCoefficient (gid, (*fullOldNlDisp2)[gid]);
                    }
                }

            }

            // Loop over FG mesh parts to assemble the boundary terms
            for ( Int k = 0; k < nproc; ++k )
            {
                // Assembling weak continutity between BG and FG1 mesh
                {
                    // Interface faces
                    std::shared_ptr< indexBGList_Type > indexBG1FaceInterListPtr ( new indexBGList_Type() );

                    fillInterfaceList( bgMeshCurPtr, fg1MeshCurPtr[k], indexBG1FaceInterListPtr ,
                                       { /* Fill with FG boundary marker */ } );

                    auto loopInterFaces1 = ExpressionAssembly::interfaceIntegrationOverSelectedFaces< mesh_Type >
                        ( bgMeshCurPtr, fg1MeshCurPtr[k], indexBG1FaceInterListPtr );

                    dSpaceETAPtr_Type ETdSolid1FESpace_k = ETdSolid1FESpace_vec[k];
                    vectorPtr_Type oldNlDisp1_k = oldNlDisp1_vec[k];

                    solidLaw1.setInterfaceData( alpha_inter , loopInterFaces1 , alphaW , &ETdSolid1FESpace_k );

                    using namespace ExpressionAssembly;

                    std::cout << "BG-FG1: (" << myPID << "," << k << ")"
                              << " - Interface faces: " << indexBG1FaceInterListPtr->size() << std::endl;

                    // Coupling conditions on the FSI interface
                    couplingFSI1.setInterfaceData( alpha_inter, gamma_inter, loopInterFaces1,
                                                  alphaW, &ETdSolid1FESpace_k );

                    if ( std::is_same< couplingFSI_Type , Slip >::value )
                    {
                        couplingFSI1.setCouplingDirection( CouplingDirection::NormalFace );
                    }

                    couplingFSI1.computeCouplingTerms( ETweakFSMatrix );
                }

                // Assembling weak continutity between BG and FG2 mesh
                {
                    // Interface faces
                    std::shared_ptr< indexBGList_Type > indexBG2FaceInterListPtr ( new indexBGList_Type() );

                    fillInterfaceList( bgMeshCurPtr, fg2MeshCurPtr[k], indexBG2FaceInterListPtr ,
                                       { /* Fill with FG boundary marker */ } );

                    auto loopInterFaces2 = ExpressionAssembly::interfaceIntegrationOverSelectedFaces< mesh_Type >
                        ( bgMeshCurPtr, fg2MeshCurPtr[k], indexBG2FaceInterListPtr );

                    dSpaceETAPtr_Type ETdSolid2FESpace_k = ETdSolid2FESpace_vec[k];
                    vectorPtr_Type oldNlDisp2_k = oldNlDisp2_vec[k];

                    using namespace ExpressionAssembly;

                    std::cout << "BG-FG2: (" << myPID << "," << k << ")"
                              << " - Interface faces: " << indexBG2FaceInterListPtr->size() << std::endl;

                    // Coupling conditions on the FSI interface
                    couplingFSI2.setInterfaceData( alpha_inter, gamma_inter, loopInterFaces2,
                                                  alphaW, &ETdSolid2FESpace_k );

                    if ( std::is_same< couplingFSI_Type , Slip >::value )
                    {
                        couplingFSI2.setCouplingDirection( CouplingDirection::NormalFace );
                    }

                    couplingFSI2.computeCouplingTerms( ETweakFSMatrix );
                }

            }

            ETweakFSMatrix->globalAssemble();
            *ETsystemMatrix += *ETweakFSMatrix;
            ETweakFSMatrix.reset();

            localChrono.stop();

            vout << " Matrices assembling done in " << localChrono.diff() << " s!" << std::endl;

            // +-----------------------------------------------+
            // |         Assembling Convective terms           |
            // +-----------------------------------------------+

            localChrono.reset();
            localChrono.start();

            vout << " -- Assembling convective terms ... " << std::endl;

            blockMatrixPtr_Type ETconvMatrix
                ( new blockMatrix_Type ( ETuXFESpace->map() |
                                         ETpXFESpace->map() |
                                         ETdSolid1FESpace->map() |
                                         ETdSolid2FESpace->map()
                                         ) );

            *ETconvMatrix *= 0.0;

#ifdef ENABLE_CIP
            auto loopFacesInterior = ExpressionAssembly::interiorIntegrationOverSelectedFaces< mesh_Type >
                ( bgMeshCurPtr, faceInteriorListPtr, indexFaceInteriorListPtr );

            std::cout << "BG: (" << myPID << ")"
                      << " - Interior faces: " << indexFaceInteriorListPtr->size() << std::endl;

            blockMatrixPtr_Type ETcipMatrix
                ( new blockMatrix_Type ( ETuXFESpace->map() |
                                         ETpXFESpace->map() |
                                         ETdSolid1FESpace->map() |
                                         ETdSolid2FESpace->map()
                                         ) );

            *ETcipMatrix *= 0.0;
#endif // ENABLE_CIP

#ifdef ENABLE_BACKFLOW_STABILIZATION
            auto loopFacesBoundary = ExpressionAssembly::interiorIntegrationOverSelectedFaces< mesh_Type >
                ( bgMeshCurPtr, faceBoundaryListPtr, indexFaceBoundaryListPtr );

            std::cout << "BG: (" << myPID << ")"
                      << " - Boundary faces: " << indexFaceBoundaryListPtr->size() << std::endl;

            blockMatrixPtr_Type ETbackstabMatrix
                ( new blockMatrix_Type ( ETuXFESpace->map() |
                                         ETpXFESpace->map() |
                                         ETdSolid1FESpace->map() |
                                         ETdSolid2FESpace->map()
                                         ) );

            *ETbackstabMatrix *= 0.0;
#endif // ENABLE_BACKFLOW_STABILIZATION

            vout << "   -- Assembling convective term ... " << std::endl;

            // Convective term and correction u^k grad u^k+1
            {
                using namespace ExpressionAssembly;

                auto loopBGOwnedVolumes = integrationOverSelectedVolumes< mesh_Type >( bgOwnedVolumeListPtr , indexBGOwnedVolumeListPtr );

                // rho^f ( u_ext grad u , v )
                integrate (
                    loopBGOwnedVolumes,
                    uXFESpace->qr(),
                    ETuXFESpace,
                    ETuXFESpace,
                    value( rhoF ) *
                        dot (
                            value (ETuXFESpace, *velocityConvective) * grad ( phi_j ) ,
                            phi_i )
                )
                    >> ETconvMatrix->block(0,0);

                // rho^f / 2 ( div u_ext u , v )
                integrate (
                    loopBGOwnedVolumes,
                    uXFESpace->qr(),
                    ETuXFESpace,
                    ETuXFESpace,
                    value( rhoF / 2. ) *
                        dot (
                            trace ( grad (ETuXFESpace, *velocityConvective) ) * phi_j ,
                            phi_i )
                )
                    >> ETconvMatrix->block(0,0);

                // - rho^f ( {{ u_ext }} n [[ u ]] , {{ v }} )
                interiorIntegrate (
                    loopFacesCont,
                    quadRuleTria4pt, // Higher-order QR
                    ETuXFESpace,
                    ETuXFESpace,
                    value( - rhoF ) *
                        dot (
                            dot ( value (ETuXFESpace, *velocityConvective) , Nface ) *
                                phi_j ,
                            phi_i ),
                    TraceOperator::TemamMean
                )
                    >> ETconvMatrix->block(0,0);

                // - rho^f / 2 ( [[ u_ext ]] n , {{ u v }} )
                interiorIntegrate (
                    loopFacesCont,
                    quadRuleTria4pt, // Higher-order QR
                    ETuXFESpace,
                    ETuXFESpace,
                    value( - rhoF / 2. ) *
                        dot ( value (ETuXFESpace, *velocityConvective) , Nface ) *
                        dot ( phi_j , phi_i ),
                    TraceOperator::TemamJump
                )
                    >> ETconvMatrix->block(0,0);
            }

            ETconvMatrix->globalAssemble();
            *ETsystemMatrix += *ETconvMatrix;
            ETconvMatrix.reset();

#ifdef ENABLE_CIP
            vout << "   -- Assembling CIP terms ... " << std::endl;

            // Continuous interior penalty
            {
                using namespace ExpressionAssembly;

                // Variable to store || u_ext ||_L^inf(F)
                faceToValue_Type maxVelExtr;
                // Variable to store || dot ( u_ext , Nface ) ||_L^inf(F)
                faceToValue_Type maxVelExtrN;

                maxVelExtr.resize( bgMeshCurPtr->faceList.size() );
                maxVelExtrN.resize( bgMeshCurPtr->faceList.size() );

                const UInt nIntFaces = indexFaceInteriorListPtr->size();

                const UInt velDofs = uXFESpace->dof().numTotalDof();

                // || u_ext ||_L^inf(F)
                // || dot ( u_ext , Nface ) ||_L^inf(F)
                // Loop over the faces in faceInteriorListPtr
                for ( UInt i = 0; i < nIntFaces; ++i )
                {
                    // Extract the face
                    auto & face = (*(*faceInteriorListPtr)[i]);

                    // Compute normal to the face
                    const auto & A = face.point(0).coordinates();
                    const auto & B = face.point(1).coordinates();
                    const auto & C = face.point(2).coordinates();

                    const auto normal = ( (B-A).cross(C-A) ).normalized();

                    // Extract the map first to second
                    auto & ftsMap = face.firstToSecondAdjacentVirtualElementIdentity();

                    // Get the position of the face wrt the (real)
                    // first adjacent element
                    const UInt facePos1 = face.firstAdjacentElementPosition();

                    // Get the ID of the (real) first adjacent element
                    const UInt adjElemID1 = face.firstAdjacentElementIdentity();

                    // Get the position of the face wrt the (real)
                    // second adjacent element
                    const UInt facePos2 = face.secondAdjacentElementPosition();

                    // Get the ID of the (real) second adjacent element
                    const UInt adjElemID2 = face.secondAdjacentElementIdentity();

                    // Variable to store the temporary value of the maximum
                    Real tmpMax = 0.;

                    // Variable to store the temporary value of the maximum * NFace
                    Real tmpMaxN = 0.;

                    // Variable to store the temporary value of the velocity
                    Vector3D tmpVel;

                    // Loop over the firstToSecondAdjacentVirtualElementIdentity map
                    for( auto it = std::begin( ftsMap ); it != std::end( ftsMap ); ++it )
                    {
                        // Get the region of the first adjacent element
                        const UInt region1 = it->first;

                        // Get the region of the second adjacent element
                        const UInt region2 = it->second;

                        // Get the virtual element of the first adjacent element
                        const UInt virtual1 = uXFESpace->dof().getVirtualElementId( adjElemID1 , region1 );

                        // Get the virtual element of the second adjacent element
                        const UInt virtual2 = uXFESpace->dof().getVirtualElementId( adjElemID2 , region2 );

                        // Loop over the dofs on the face wrt the first element
                        for( UInt i = 0; i < 3; ++i)
                        {
                            // Dof of the face
                            const UInt dof1 = uXFESpace->dof().localToGlobalMap
                                ( virtual1, LinearTetra::faceToPoint(facePos1 , i) );

                            // Variable to store the norm of vel
                            Real norm = 0.;
                            // Variable to store the temporary value of the vel * NFace
                            Real maxDotN = 0.;

                            // Loop over the dimension of the field
                            for (UInt block (0); block < uXSpaceETA_Type::field_dim; ++block)
                            {
                                // Get the dof by considering the field
                                const UInt dofCoord1 = dof1 + block * velDofs;

                                // Build the euclidean norm
                                norm += (*velocityConvective)[dofCoord1] *
                                        (*velocityConvective)[dofCoord1];

                                // Build the velocity
                                tmpVel[i] = (*velocityConvective)[dofCoord1];
                            }

                            // | u_ext * NFace |
                            maxDotN = std::fabs( tmpVel.dot(normal) );

                            // Compute the maximum of the velocity on the virtual face
                            tmpMax = norm > tmpMax ? norm : tmpMax;
                            // Compute the maximum of the velocity * Nface on the virtual face
                            tmpMaxN = maxDotN > tmpMaxN ? maxDotN : tmpMaxN;
                        }

                        // Loop over the dofs on the face wrt the second element
                        for( UInt i = 0; i < 3; ++i)
                        {
                            // Dof of the face
                            const UInt dof2 = uXFESpace->dof().localToGlobalMap
                                ( virtual2, LinearTetra::faceToPoint(facePos2 , i) );

                            // Variable to store the norm of vel
                            Real norm = 0.;
                            // Variable to store the temporary value of the vel * NFace
                            Real maxDotN = 0.;

                            // Loop over the dimension of the field
                            for (UInt block (0); block < uXSpaceETA_Type::field_dim; ++block)
                            {
                                // Get the dof by considering the field
                                const UInt dofCoord2 = dof2 + block * velDofs;

                                // Build the euclidean norm
                                norm += (*velocityConvective)[dofCoord2] *
                                        (*velocityConvective)[dofCoord2];

                                // Build the velocity
                                tmpVel[i] = (*velocityConvective)[dofCoord2];
                            }

                            // | u_ext * NFace |
                            maxDotN = std::fabs( tmpVel.dot(normal) );

                            // Compute the maximum of the velocity on the virtual face
                            tmpMax = norm > tmpMax ? norm : tmpMax;
                            // Compute the maximum of the velocity * Nface on the virtual face
                            tmpMaxN = maxDotN > tmpMaxN ? maxDotN : tmpMaxN;
                        }

                        // Fill maxVelExtr
                        maxVelExtr[ face.localId() ].emplace
                            ( std::make_pair(region1,region2) , std::sqrt(tmpMax) );

                        // Fill maxVelExtrN
                        maxVelExtrN[ face.localId() ].emplace
                            ( std::make_pair(region1,region2) , tmpMaxN );
                    }
                }

                // csi ( x ) = min { 1 , x }
                // Re ( z ) = rho^f || z ||_L^inf h_F / mu
                // csi ( Re( u_ext ) ) = min { 1 , rho^f || u_ext ||_L^inf h_F / mu }
                // See: E. Burman, M.A. Fernandez, P. Hansbo, Continuous interior penalty finite element method for oseen's equations,
                // SIAM J. Numer. Anal. 44 (2006) 1248-1274. URL: http://dx.doi.org/10.1137/040617686

                // gamma_v1 * h_F^2 *
                // csi( Re (u_ext) ) *
                // || u_ext * n ||_L^inf *
                // ( [[ grad u ]] n , [[ grad v ]] n )
                interiorIntegrate (
                    loopFacesInterior,
                    uXFESpace->bdQr(),
                    ETuXFESpace,
                    ETuXFESpace,
                    value( gamma_v1 ) * h_F * h_F *
                    min( value( 1.) , rhoF * parameter( maxVelExtr ) * h_F / mu  ) *
                    parameter( maxVelExtrN ) *
                    dot( grad ( phi_j ) * Nface , grad ( phi_i ) * Nface ) ,
                    TraceOperator::CIP
                )
                    >> ETcipMatrix->block(0,0);

                // gamma_v2 * h_F^2 *
                // csi( Re (u_ext) ) *
                // || u_ext ||_L^inf *
                // ( [[ div u ]] , [[ div v ]] )
                interiorIntegrate (
                    loopFacesInterior,
                    uXFESpace->bdQr(),
                    ETuXFESpace,
                    ETuXFESpace,
                    value( gamma_v2 ) * h_F * h_F *
                    min( value( 1.) , rhoF * parameter( maxVelExtr ) * h_F / mu  ) *
                    parameter( maxVelExtr ) *
                    div ( phi_j ) * div ( phi_i ) ,
                    TraceOperator::CIP
                )
                    >> ETcipMatrix->block(0,0);

                // gamma_p * h_F^2 *
                // csi( Re (u_ext) ) / || u_ext ||_L^inf *
                // ( [[ grad p ]] , [[ grad q ]] )
                interiorIntegrate (
                    loopFacesInterior,
                    pXFESpace->bdQr(),
                    ETpXFESpace,
                    ETpXFESpace,
                    value( gamma_p ) * h_F * h_F *
                    min( value( 1. ) / parameter( maxVelExtr ) , rhoF * h_F / mu ) *
                    dot( grad ( phi_j ) , grad ( phi_i ) ) ,
                    TraceOperator::CIP
                )
                    >> ETcipMatrix->block(1,1);
            }

            ETcipMatrix->globalAssemble();
            *ETsystemMatrix += *ETcipMatrix;
            ETcipMatrix.reset();
#endif // ENABLE_CIP

#ifdef ENABLE_BACKFLOW_STABILIZATION
            vout << "   -- Assembling Backflow Stabilization term ... " << std::endl;

            // Backflow stabilization
            {
                using namespace ExpressionAssembly;

                // - beta rho^f / 2 ( min(u_ext n, 0) , u v )
                // beta == 1
                boundaryIntegrate (
                    loopFacesBoundary,
                    quadRuleTria4pt, // Higher-order QR
                    ETuXFESpace,
                    ETuXFESpace,
                    value( - rhoF / 2. ) *
                        min( dot ( value (ETuXFESpace, *velocityConvective) , Nface ) , 0 ) *
                        dot ( phi_j , phi_i )
                )
                    >> ETbackstabMatrix->block(0,0);
            }

            ETbackstabMatrix->globalAssemble();
            *ETsystemMatrix += *ETbackstabMatrix;
            ETbackstabMatrix.reset();
#endif // ENABLE_BACKFLOW_STABILIZATION

            localChrono.stop();

            vout << " done in " << localChrono.diff() << " s!" << std::endl;

            localChrono.reset();
            localChrono.start();

            vout << " -- Finilize System Matrix ... " << std::endl;

            ETsystemMatrix->globalAssemble();

            localChrono.stop();

            vout << " done in " << localChrono.diff() << " s!" << std::endl;

            // +-----------------------------------------------+
            // |                  Assembling RHS               |
            // +-----------------------------------------------+

            localChrono.reset();
            localChrono.start();

            vout << " -- Assembling RHS term ... " << std::endl;

            // RHS velocity first derivative
            vectorPtr_Type rhsU
                ( new vector_Type ( ETuXFESpace->map() , Repeated) );

            // RHS displacement second derivative
            vectorPtr_Type rhsD1
                ( new vector_Type ( ETdSolid1FESpace->map() , Repeated) );
            vectorPtr_Type rhsD2
                ( new vector_Type ( ETdSolid2FESpace->map() , Repeated) );

            // Rhs for assembling source term
            blockVectorPtr_Type ETrhs
                ( new blockVector_Type ( ETuXFESpace->map() |
                                         ETpXFESpace->map() |
                                         ETdSolid1FESpace->map() |
                                         ETdSolid2FESpace->map() , Repeated ) );

            *ETrhs *= 0.0;

            // RHS
            {
                using namespace ExpressionAssembly;

                auto loopBGOwnedVolumes = integrationOverSelectedVolumes< mesh_Type >( bgOwnedVolumeListPtr , indexBGOwnedVolumeListPtr );

                timeAdvanceFluidPtr->updateRHSContribution ( dt );
                *rhsU = timeAdvanceFluidPtr->rhsContributionFirstDerivative();

                // RHS fluid mass ( uOld , v )
                integrate (
                    loopBGOwnedVolumes,
                    uXFESpace->qr(),
                    ETuXFESpace,
                    value( rhoF ) * dot ( value (ETuXFESpace, *rhsU) , phi_i)
                )
                    >> ETrhs->block (0);

                // FG1
                {
                    auto loopFG1OwnedVolumes = integrationOverSelectedVolumes< mesh_Type >( fg1OwnedVolumeListPtr , indexFG1OwnedVolumeListPtr );

                    timeAdvanceSolid1Ptr->updateRHSContribution ( dt );
                    *rhsD1 = timeAdvanceSolid1Ptr->rhsContributionSecondDerivative();

                    // RHS solid mass ( dOld , w )
                    integrate (
                        loopFG1OwnedVolumes,
                        dSolid1FESpace->qr(),
                        ETdSolid1FESpace,
                        value( rhoS ) * dot ( value (ETdSolid1FESpace, *rhsD1) , phi_i)
                    )
                        >> ETrhs->block (2);

                    // RHS displacement first derivative
                    vectorPtr_Type rhsW1 ( new vector_Type ( ETdSolid1FESpace->map() , Repeated) );
                    *rhsW1 = timeAdvanceSolid1Ptr->rhsContributionFirstDerivative();
                    rhsW1->setCombineMode(Insert);

                    // RHS displacement first derivative on the entire FG1 mesh
                    vectorPtr_Type fullRhsW1;

                    // Gather the partitioned rhsW1 and fill fullRhsW1.
                    for ( Int k = 0; k < nproc; ++k )
                    {
                        if ( static_cast<UInt>(k) != myPID )
                        {
                            vectorPtr_Type tmpFullRhsW1 ( new vector_Type ( *rhsW1 , k ) );
                        }
                        else
                        {
                            fullRhsW1.reset( new vector_Type ( *rhsW1 , k ) );
                        }
                    }

                    // Restore the default combine mode (Add)
                    rhsW1->setCombineMode(Add);

                    // RHS displacement first derivative of the k-th proc
                    std::vector<vectorPtr_Type> rhsW1_vec(nproc, nullptr);

                    for ( Int k = 0; k < nproc; ++k )
                    {
                        mapPtr_Type solid1Map_k ( new map_Type ( ETdSolid1FESpace_vec[k]->map() ) );
                        rhsW1_vec[k].reset ( new vector_Type ( solid1Map_k , Repeated ) );

                        for ( UInt gid = 0; gid < fullRhsW1->map().mapSize(); ++gid )
                        {
                            if ( rhsW1_vec[k]->isGlobalIDPresent(gid) )
                            {
                                rhsW1_vec[k]->setCoefficient (gid, (*fullRhsW1)[gid]);
                            }
                        }
                    }

                    // Loop over FG mesh parts to assemble the boundary terms
                    for ( Int k = 0; k < nproc; ++k )
                    {
                        // Interface faces
                        std::shared_ptr< indexBGList_Type > indexBG1FaceInterListPtr ( new indexBGList_Type() );

                        fillInterfaceList( bgMeshCurPtr, fg1MeshCurPtr[k], indexBG1FaceInterListPtr ,
                                           { /* Fill with FG boundary marker */ } );

                        auto loopInterFaces1 = ExpressionAssembly::interfaceIntegrationOverSelectedFaces< mesh_Type >
                            ( bgMeshCurPtr, fg1MeshCurPtr[k], indexBG1FaceInterListPtr );

                        dSpaceETAPtr_Type ETdSolid1FESpace_k = ETdSolid1FESpace_vec[k];
                        vectorPtr_Type rhsW1_k = rhsW1_vec[k];
                        vectorPtr_Type oldNlDisp1_k = oldNlDisp1_vec[k];

                        solidLaw1.setInterfaceData( alpha_inter , loopInterFaces1 , alphaW , &ETdSolid1FESpace_k );
                        
                        // Coupling conditions on the FSI interface
                        couplingFSI1.setInterfaceData( alpha_inter, gamma_inter, loopInterFaces1,
                                                      alphaW, &ETdSolid1FESpace_k );

                        if ( std::is_same< couplingFSI_Type , Slip >::value )
                        {
                            couplingFSI1.setCouplingDirection( CouplingDirection::NormalFace );
                        }

                        couplingFSI1.computeCouplingRHS( ETrhs, *oldNlDisp1_k, *rhsW1_k );

                        std::cout << "\tBG-FG1: (" << myPID << "," << k << ") - rhs - "
                                  << "Interface faces: " << indexBG1FaceInterListPtr->size() << std::endl;

                    }
/*                    solidLaw1.computeStiffness ( ETrhs , *oldNlDisp1 );*/
                }// end FG1

                // FG2
                {
                    auto loopFG2OwnedVolumes = integrationOverSelectedVolumes< mesh_Type >( fg2OwnedVolumeListPtr , indexFG2OwnedVolumeListPtr );

                    timeAdvanceSolid2Ptr->updateRHSContribution ( dt );
                    *rhsD2 = timeAdvanceSolid2Ptr->rhsContributionSecondDerivative();

                    // RHS displacement first derivative
                    vectorPtr_Type rhsW2 ( new vector_Type ( ETdSolid2FESpace->map() , Repeated) );
                    *rhsW2 = timeAdvanceSolid2Ptr->rhsContributionFirstDerivative();
                    rhsW2->setCombineMode(Insert);

                    // RHS displacement first derivative on the entire FG2 mesh
                    vectorPtr_Type fullRhsW2;

                    // Gather the partitioned rhsW2 and fill fullRhsW2.
                    for ( Int k = 0; k < nproc; ++k )
                    {
                        if ( static_cast<UInt>(k) != myPID )
                        {
                            vectorPtr_Type tmpFullRhsW2 ( new vector_Type ( *rhsW2 , k ) );
                        }
                        else
                        {
                            fullRhsW2.reset( new vector_Type ( *rhsW2 , k ) );
                        }
                    }

                    // Restore the default combine mode (Add)
                    rhsW2->setCombineMode(Add);

                    // RHS displacement first derivative of the k-th proc
                    std::vector<vectorPtr_Type> rhsW2_vec(nproc, nullptr);

                    for ( Int k = 0; k < nproc; ++k )
                    {
                        mapPtr_Type solid2Map_k ( new map_Type ( ETdSolid2FESpace_vec[k]->map() ) );
                        rhsW2_vec[k].reset ( new vector_Type ( solid2Map_k , Repeated ) );

                        for ( UInt gid = 0; gid < fullRhsW2->map().mapSize(); ++gid )
                        {
                            if ( rhsW2_vec[k]->isGlobalIDPresent(gid) )
                            {
                                rhsW2_vec[k]->setCoefficient (gid, (*fullRhsW2)[gid]);
                            }
                        }
                    }

                    // Loop over FG mesh parts to assemble the boundary terms
                    for ( Int k = 0; k < nproc; ++k )
                    {
                        // Interface faces
                        std::shared_ptr< indexBGList_Type > indexBG2FaceInterListPtr ( new indexBGList_Type() );

                        fillInterfaceList( bgMeshCurPtr, fg2MeshCurPtr[k], indexBG2FaceInterListPtr ,
                                           { /* Fill with FG boundary marker */ } );

                        auto loopInterFaces2 = ExpressionAssembly::interfaceIntegrationOverSelectedFaces< mesh_Type >
                            ( bgMeshCurPtr, fg2MeshCurPtr[k], indexBG2FaceInterListPtr );

                        dSpaceETAPtr_Type ETdSolid2FESpace_k = ETdSolid2FESpace_vec[k];
                        vectorPtr_Type rhsW2_k = rhsW2_vec[k];
                        vectorPtr_Type oldNlDisp2_k = oldNlDisp2_vec[k];

                        // Coupling conditions on the FSI interface
                        couplingFSI2.setInterfaceData( alpha_inter, gamma_inter, loopInterFaces2,
                                                      alphaW, &ETdSolid2FESpace_k );

                        if ( std::is_same< couplingFSI_Type , Slip >::value )
                        {
                            couplingFSI2.setCouplingDirection( CouplingDirection::NormalFace );
                        }

                        couplingFSI2.computeCouplingRHS( ETrhs, *oldNlDisp2_k, *rhsW2_k );

                    }
                }// end FG2
            }

            ETrhs->globalAssemble();

            localChrono.stop();

            vout << " done in " << localChrono.diff() << " s!" << std::endl;

            // +-----------------------------------------------+
            // |                  Setting BCs                  |
            // +-----------------------------------------------+

            localChrono.reset();
            localChrono.start();

            vout << " -- Apply Boundary Conditions... " << std::endl;

            // Unique version of rhs for assembling
            blockVectorPtr_Type ETrhsUnique ( new blockVector_Type ( *ETrhs , Unique ) );
            ETrhs.reset();

            BCFunctionBase ZeroBC ( bcSettings.get_zeroFunction() );
            BCFunctionBase inletBC ( bcSettings.get_zeroFunction() );
            BCFunctionBase outletBC ( bcSettings.get_pressureFunction() );
            BCFunctionBase dirichletBC ( bcSettings.get_strokeFunction() );

            // BC for BG mesh
            BCHandler bcHandler;

            vout << "Homogeneous neumann condition at the inlet surface." << std::endl;
            bcHandler.addBC ( "INLET", INLET, Natural, Full, inletBC,  3 );

            vout << "Homogeneous neumann condition at the outlet surface." << std::endl;
            bcHandler.addBC ( "OUTLET", OUTLET, Natural, Full, outletBC,  3 );

            //pump walls: homogeneous dirichlet
            std::string wallName;
            UInt wallIndex=0;
            for ( auto wallFlag : WALLS )
            {
                wallName = "WALL_" + std::to_string(wallIndex+1);
                bcHandler.addBC( wallName, wallFlag, Essential, Full, ZeroBC, 3  );
                wallIndex++;
            }

            bcHandler.bcUpdate ( *uXFESpace->mesh(), uXFESpace->feBd(), uXFESpace->dof() );

            // BC for FG1 mesh
            BCHandler bcHandlerSolid1;

            bcHandlerSolid1.setOffset( 3 * uXFESpace->dof().numTotalDof() +
                                           pXFESpace->dof().numTotalDof() );

            vout << "Membrane oscillation - Dirichlet condition applied on the membrane frame." << std::endl;
            bcHandlerSolid1.addBC ( "TITANIUM_DIRICHLET",  TITANIUM,   Essential, Full, dirichletBC, 3 );          
            bcHandlerSolid1.bcUpdate ( *dSolid1FESpace->mesh(), dSolid1FESpace->feBd(), dSolid1FESpace->dof() );

            // BC for FG2 mesh
            BCHandler bcHandlerSolid2;

            bcHandlerSolid2.setOffset( 3 * uXFESpace->dof().numTotalDof() +
                                           pXFESpace->dof().numTotalDof() +
                                       3 * dSolid1FESpace->dof().numTotalDof() );

            vout << "Membrane oscillation - Dirichlet condition applied on the magnetic ring surface." << std::endl;
            bcHandlerSolid2.addBC ( "MAGNET_DIRICHLET",  MAGNET_INTERFACE,   Essential, Full, dirichletBC, 3 );          
            bcHandlerSolid2.bcUpdate ( *dSolid2FESpace->mesh(), dSolid2FESpace->feBd(), dSolid2FESpace->dof() );

            // Update the RHS with BCs
            bcManage ( *ETsystemMatrix, *ETrhsUnique, *uXFESpace->mesh(),
                       uXFESpace->dof(), bcHandler, uXFESpace->feBd(), 1.0, time);

            // Update the RHS with BCs
            bcManage ( *ETsystemMatrix, *ETrhsUnique, *dSolid1FESpace->mesh(),
                       dSolid1FESpace->dof(), bcHandlerSolid1, dSolid1FESpace->feBd(), 1.0, time);

            // Update the RHS with BCs
            bcManage ( *ETsystemMatrix, *ETrhsUnique, *dSolid2FESpace->mesh(),
                       dSolid2FESpace->dof(), bcHandlerSolid2, dSolid2FESpace->feBd(), 1.0, time);

            localChrono.stop();

            vout << " done in " << localChrono.diff() << " s!" << std::endl;

            // +-----------------------------------------------+
            // |       Setting solver and preconditioner       |
            // +-----------------------------------------------+

            localChrono.reset();
            localChrono.start();

            vout << " -- Setting up the solver ... " << std::endl;

            LinearSolver linearSolver ( Comm );

            Teuchos::RCP< Teuchos::ParameterList > paramList = Teuchos::rcp ( new Teuchos::ParameterList );
            const std::string paramListFile  = dataFile ( "prec/paramListFile", "solverFile.xml" );
            vout << "Using parameters from "  << paramListFile << " file." << std::endl;
            paramList = Teuchos::getParametersFromXmlFile ( paramListFile );

            linearSolver.setParameters ( *paramList );

            prec_Type * precRawPtr = new prec_Type( Comm );
            precRawPtr->setDataFromGetPot ( dataFile, "prec" );
            precRawPtr->setBlockSize ( { 3 * uXFESpace->dof().numTotalDof() ,
                                             pXFESpace->dof().numTotalDof() ,
                                         3 * dSolid1FESpace->dof().numTotalDof() +
                                         3 * dSolid2FESpace->dof().numTotalDof() } );

            basePrecPtr_Type basePrecPtr;
            basePrecPtr.reset ( precRawPtr );

            linearSolver.setPreconditioner ( basePrecPtr );

            vout << "   -- Print information about preconditioner -- " << std::endl;
            basePrecPtr->showMe();
            vout << "   -- Print information about linear solver -- " << std::endl;
            linearSolver.showMe();

            localChrono.stop();

            vout << " done in " << localChrono.diff() << " s!" << std::endl;

            // +-----------------------------------------------+
            // |                   Solving                     |
            // +-----------------------------------------------+

            localChrono.reset();
            localChrono.start();

            vout << " -- Solving ... " << std::endl;

            // Solve the linear system
            linearSolver.setOperator ( ETsystemMatrix );
            linearSolver.setRightHandSide ( ETrhsUnique );
            linearSolver.solve ( ETsolution );

            localChrono.stop();

            vout << " Solution computed in " << localChrono.diff() << " s!" << std::endl;

            // +-----------------------------------------------+
            // |            Checking convergence               |
            // +-----------------------------------------------+

            localChrono.reset();
            localChrono.start();

            vout << " -- Checking convergence ... " << std::endl;

            blockVectorPtr_Type ETsolutionRepeated;
            ETsolutionRepeated.reset ( new blockVector_Type ( *ETsolution , Repeated ) );
            ETsolution.reset();

            // Store the velocity, as the solution at the previous non-linear iteration
            velocitySol->subset( *ETsolutionRepeated , 0 );

            // Pressure solution
            pressureSol->subset ( *ETsolutionRepeated, 3 * uXFESpace->dof().numTotalDof());

            // Solution on the solid mesh
            dispSol1->subset ( *ETsolutionRepeated, 3 * uXFESpace->dof().numTotalDof() +
                                                        pXFESpace->dof().numTotalDof() );
            dispSol2->subset ( *ETsolutionRepeated, 3 * uXFESpace->dof().numTotalDof() +
                                                        pXFESpace->dof().numTotalDof() +
                                                    3 * dSolid1FESpace->dof().numTotalDof() );
            // Local norms
            Real div = 0.;
            Real fluxOut = 0.;
            Real pressOut = 0.;

            {
                using namespace ExpressionAssembly;

                auto loopBGOwnedVolumes = integrationOverSelectedVolumes< mesh_Type >( bgOwnedVolumeListPtr , indexBGOwnedVolumeListPtr );

                // int(\Omega) div u
                integrate (
                    loopBGOwnedVolumes,
                    uXFESpace->qr(),
                    trace ( grad (ETuXFESpace, *velocitySol) ),
                    ETuXFESpace
                )
                    >> div;

                {
                    std::shared_ptr< faceList_Type > faceBoundaryListPtr ( new faceList_Type() );
                    std::shared_ptr< indexList_Type > indexFaceBoundaryListPtr ( new indexList_Type() );

                    for ( auto & face : bgMeshCurPtr->faceList )
                    {
                        if ( face.markerID() == OUTLET &&
                             Flag<meshEntityFlag_Type>::testOneNotSet( face.flag(), EntityFlags::GHOST ) )
                        {
                            faceBoundaryListPtr->push_back( &face );
                            indexFaceBoundaryListPtr->push_back( face.localId() );
                        }
                    }

                    auto loopFacesBoundary = ExpressionAssembly::interiorIntegrationOverSelectedFaces< mesh_Type >
                        ( bgMeshCurPtr, faceBoundaryListPtr, indexFaceBoundaryListPtr );

                    // int(\Gamma_out) u * n
                    boundaryIntegrate (
                        loopFacesBoundary,
                        uXFESpace->bdQr(),
                        dot ( value (ETuXFESpace, *velocitySol) , Nface ),
                        ETuXFESpace
                    )
                        >> fluxOut;

                    // int(\Gamma_out) p
                    boundaryIntegrate (
                        loopFacesBoundary,
                        pXFESpace->bdQr(),
                        value (ETpXFESpace, *pressureSol),
                        ETpXFESpace
                    )
                        >> pressOut;  

                }
            }

            Comm->Barrier();

            // Global norms
            Real divTot = 0.;
            Real fluxOutTot = 0.;
            Real pressOutTot = 0.;

            Comm->SumAll (&div, &divTot, 1);
            Comm->SumAll (&fluxOut, &fluxOutTot, 1);
            Comm->SumAll (&pressOut, &pressOutTot, 1);

            // divide by the outlet area to have the average value
            pressOutTot /= (PI*0.55*0.55);
            // divide by 1333,22 to convert to mmHg
            pressOutTot /= 1333.22;

            vout << "int(Omega) div u           : " << divTot << std::endl;
            vout << "int(Gamma_out) u * n       : " << fluxOutTot << std::endl;
            vout << "int(Gamma_out) p/area      : " << pressOutTot << std::endl;

            const Real actualRelax = std::min( mLin * (nlIter) * relaxSolSolid, 1. );

            if (nlMaxIter > 1 && nlIter > 0)
            {
                vout << "   Actual relaxation : " << actualRelax << std::endl;


                // d^{k+1} =  \omega d^{k+1} + (1 - \omega) d^k

                *dispSol1 *= actualRelax; 
                *dispSol2 *= actualRelax;

                *oldNlDisp1 *= (1.0-actualRelax);
                *oldNlDisp2 *= (1.0-actualRelax); 

                *dispSol1 += *oldNlDisp1; 
                *dispSol2 += *oldNlDisp2; 

            }

            // Updating the temporal scheme only at convergence of the internal loop
            if ( nlIter == nlMaxIter-1 )
            {
                // Fluid velocity
                vectorPtr_Type endU ( new vector_Type ( ETuXFESpace->map() , Repeated) );
                endU->subset( *ETsolutionRepeated , 0 );

                // Solid displacement (velocity, acceleration)
                vectorPtr_Type endD1 ( new vector_Type ( ETdSolid1FESpace->map() , Repeated) );
                endD1->subset( *ETsolutionRepeated , 3 * uXFESpace->dof().numTotalDof() +
                                                         pXFESpace->dof().numTotalDof() );
                vectorPtr_Type endD2 ( new vector_Type ( ETdSolid2FESpace->map() , Repeated) );
                endD2->subset( *ETsolutionRepeated , 3 * uXFESpace->dof().numTotalDof() +
                                                         pXFESpace->dof().numTotalDof() +
                                                     3 * dSolid1FESpace->dof().numTotalDof() );

                timeAdvanceFluidPtr->shiftRight ( *endU );
                timeAdvanceSolid1Ptr->shiftRight ( *endD1 );
                timeAdvanceSolid2Ptr->shiftRight ( *endD2 );

                // Set the correct solid displacement
                *dispSol1 = *endD1;
                *dispSol2 = *endD2;

                nlConverged = true;
            }

            localChrono.stop();

            vout << " done in " << localChrono.diff() << " s!" << std::endl;

            // +-----------------------------------------------+
            // |              Post-processing                  |
            // +-----------------------------------------------+

            localChrono.reset();
            localChrono.start();

            vout << " -- Initialize Post-processing ... " << std::endl;

            // Store the velocity, as the solution at the previous non-linear iteration
            oldNlVelocity.reset( new vector_Type ( ETuXFESpace->map() , Repeated ) );
            *oldNlVelocity = *velocitySol;

            // Store the displacement, as the solution at the previous non-linear iteration
            *oldNlDisp1 = *dispSol1;
            *oldNlDisp2 = *dispSol2;

            localChrono.stop();

            vout << " done in " << localChrono.diff() << " s!" << std::endl;

            // +-----------------------------------------------+
            // |               Exporting solution              |
            // +-----------------------------------------------+

            localChrono.reset();
            localChrono.start();

            vout << " -- Exporting ... " << std::endl;

            Real timeExp = time - dt + subdt * (nlIter+1);
            if ( nlConverged )
            {
                timeExp = time;
            }

            exporterRestartF.postProcess ( timeExp );
            exporterRestartS1.postProcess ( timeExp );
            exporterRestartS2.postProcess ( timeExp );

            ++nlIter;

            localChrono.stop();

            vout << " done in " << localChrono.diff() << " s!" << std::endl;

            oldNlBGMeshPtr = bgMeshCurPtr;
            oldNlUXFESpace = uXFESpace;

            MPI_Barrier (MPI_COMM_WORLD);

            nlChrono.stop();

            vout << std::endl << " Non-linear iteration done in " << nlChrono.diff() << " s!" << std::endl;
        }
        while( !nlConverged && timeIter > 0 ); // end internal loop

        for ( UInt i = 0; i < stencilSize; ++i )
        {
            if ( oldTimeStencilVelocity[i] != nullptr )
            {
                delete oldTimeStencilVelocity[i];
            }
        }

        oldTimeBGMeshPtr = oldNlBGMeshPtr;
        oldTimeUXFESpace = oldNlUXFESpace;

        // Reset nlIter to zero
        nlIter = 0;

        timeChrono.stop();

        vout << std::endl << " Time step done in " << timeChrono.diff() << " s!" << std::endl;
    } // end time loop

    globalChrono.stop();

    vout << std::endl << "Problem solved in  " << globalChrono.diff() << " s!" << std::endl;

#ifdef HAVE_MPI
    MPI_Finalize();
#endif

    return ( EXIT_SUCCESS );
}
