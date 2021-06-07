##############################################################################
#
# CMake variable for use by LifeV clients. 
#
# Do not edit: This file was generated automatically by CMake.
#
##############################################################################

#
# Ensure CMAKE_CURRENT_LIST_DIR is usable.
#

IF (NOT DEFINED CMAKE_CURRENT_LIST_DIR)
  GET_FILENAME_COMPONENT(_THIS_SCRIPT_PATH ${CMAKE_CURRENT_LIST_FILE} PATH)
  SET(CMAKE_CURRENT_LIST_DIR ${_THIS_SCRIPT_PATH})
ENDIF()


## ---------------------------------------------------------------------------
## Compilers used by LifeV build
## ---------------------------------------------------------------------------

SET(LifeV_CXX_COMPILER "/usr/bin/mpicxx")

SET(LifeV_C_COMPILER "/usr/bin/mpicc")

SET(LifeV_Fortran_COMPILER "mpif90")

## ---------------------------------------------------------------------------
## Compiler flags used by LifeV build
## ---------------------------------------------------------------------------

SET(LifeV_CXX_COMPILER_FLAGS " -DTRILIBRARY -DTETLIBRARY -std=c++11 -O3 -msse3 -Wno-unused-local-typedefs -Wno-literal-suffix -Wno-cpp -Wunused-parameter -O3")

SET(LifeV_C_COMPILER_FLAGS " -DTRILIBRARY -DTETLIBRARY -std=c++11 -O3 -msse3 -O3")

SET(LifeV_Fortran_COMPILER_FLAGS "-Og -g ")

## Extra link flags (e.g., specification of fortran libraries)
SET(LifeV_EXTRA_LD_FLAGS "")

## This is the command-line entry used for setting rpaths. In a build
## with static libraries it will be empty. 
SET(LifeV_SHARED_LIB_RPATH_COMMAND "")
SET(LifeV_BUILD_SHARED_LIBS "OFF")

SET(LifeV_LINKER /usr/bin/ld)
SET(LifeV_AR ar)


## ---------------------------------------------------------------------------
## Set library specifications and paths 
## ---------------------------------------------------------------------------

## The project version number
SET(LifeV_VERSION "3.10.1")

## The project include file directories.
SET(LifeV_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/../../../include")

## The project library directories.
SET(LifeV_LIBRARY_DIRS "${CMAKE_CURRENT_LIST_DIR}/../../../lib")

## The project libraries.
SET(LifeV_LIBRARIES "lifevxfem;lifeveta;lifevcore")

## The project tpl include paths
SET(LifeV_TPL_INCLUDE_DIRS "/usr/include/trilinos;/usr/include;/usr/include/superlu;/usr/include/hdf5/openmpi;/usr/include/scotch;/home/lifev/tetgen1.5.0")

## The project tpl library paths
SET(LifeV_TPL_LIBRARY_DIRS "/usr/lib/x86_64-linux-gnu")

## The project tpl libraries
SET(LifeV_TPL_LIBRARIES "-L/usr/lib/x86_64-linux-gnu -ltrilinos_rythmos -ltrilinos_shylu -ltrilinos_stokhos_muelu -ltrilinos_stokhos_ifpack2 -ltrilinos_stokhos_amesos2 -ltrilinos_stokhos_tpetra -ltrilinos_stokhos_sacado -ltrilinos_stokhos -ltrilinos_locathyra -ltrilinos_locaepetra -ltrilinos_localapack -ltrilinos_loca -ltrilinos_noxepetra -ltrilinos_noxlapack -ltrilinos_nox -ltrilinos_sacado -ltrilinos_muelu-adapters -ltrilinos_muelu-interface -ltrilinos_muelu -ltrilinos_teko -ltrilinos_stratimikos -ltrilinos_stratimikosbelos -ltrilinos_stratimikosaztecoo -ltrilinos_stratimikosamesos -ltrilinos_stratimikosml -ltrilinos_stratimikosifpack -ltrilinos_ifpack2-adapters -ltrilinos_ifpack2 -ltrilinos_zoltan2 -ltrilinos_amesos2 -ltrilinos_ml -ltrilinos_galeri-xpetra -ltrilinos_galeri -ltrilinos_isorropia -ltrilinos_xpetra-sup -ltrilinos_xpetra -ltrilinos_thyratpetra -ltrilinos_thyraepetraext -ltrilinos_ifpack -ltrilinos_amesos -ltrilinos_anasazitpetra -ltrilinos_ModeLaplace -ltrilinos_anasaziepetra -ltrilinos_anasazi -ltrilinos_thyraepetra -ltrilinos_thyracore -ltrilinos_rtop -ltrilinos_belostpetra -ltrilinos_belosepetra -ltrilinos_belos -ltrilinos_tpetraext -ltrilinos_tpetrainout -ltrilinos_tpetra -ltrilinos_kokkostsqr -ltrilinos_tpetrakernels -ltrilinos_tpetraclassiclinalg -ltrilinos_tpetraclassicnodeapi -ltrilinos_tpetraclassic -ltrilinos_kokkosalgorithms -ltrilinos_kokkoscontainers -ltrilinos_aztecoo -ltrilinos_zoltan -ltrilinos_epetraext -ltrilinos_triutils -ltrilinos_epetra -ltrilinos_teuchoskokkoscomm -ltrilinos_teuchoskokkoscompat -ltrilinos_teuchosremainder -ltrilinos_teuchosnumerics -ltrilinos_teuchoscomm -ltrilinos_teuchosparameterlist -ltrilinos_teuchoscore -ltrilinos_kokkoscore;/usr/lib/libsmumps.so;/usr/lib/libdmumps.so;/usr/lib/libcmumps.so;/usr/lib/libzmumps.so;/usr/lib/libpord.so;/usr/lib/libmumps_common.so;/usr/lib/x86_64-linux-gnu/libsuperlu.so;/usr/lib/x86_64-linux-gnu/hdf5/openmpi/libhdf5.so;/usr/lib/x86_64-linux-gnu/libtbb.so;/usr/lib/x86_64-linux-gnu/libz.so;/usr/lib/libptscotch.so;/usr/lib/libptscotcherr.so;/usr/lib/libscotch.so;/usr/lib/libscotcherr.so;/usr/lib/liblapack.so;/usr/lib/libblas.so;/usr/lib/x86_64-linux-gnu/libbfd.so;/usr/lib/x86_64-linux-gnu/libiberty.a;/usr/lib/libtriangle.so;/home/lifev/tetgen1.5.0/libtet.a;/usr/lib/x86_64-linux-gnu/libz.so;/usr/lib/libparmetis.so;/usr/lib/x86_64-linux-gnu/libmetis.so;/usr/lib/liblapack.so;/usr/lib/libblas.so")

## ---------------------------------------------------------------------------
## MPI specific variables
##   These variables are provided to make it easier to get the mpi libraries
##   and includes on systems that do not use the mpi wrappers for compiling
## ---------------------------------------------------------------------------

SET(LifeV_MPI_LIBRARIES "")
SET(LifeV_MPI_LIBRARY_DIRS "")
SET(LifeV_MPI_INCLUDE_DIRS "")
SET(LifeV_MPI_EXEC "/usr/bin/mpiexec")
SET(LifeV_MPI_EXEC_MAX_NUMPROCS "4")
SET(LifeV_MPI_EXEC_NUMPROCS_FLAG "-np")

## ---------------------------------------------------------------------------
## Set useful general variables 
## ---------------------------------------------------------------------------

## The packages enabled for this project
SET(LifeV_PACKAGE_LIST "XFEM;ETA;Core")

## The TPLs enabled for this project
SET(LifeV_TPL_LIST "Trilinos;Triangle;TetGen;HDF5;ParMETIS;Boost;LAPACK;BLAS;MPI")


# Import LifeV targets
IF(NOT LifeV_TARGETS_IMPORTED)
  SET(LifeV_TARGETS_IMPORTED 1)
  INCLUDE("${CMAKE_CURRENT_LIST_DIR}/LifeVTargets.cmake")
ENDIF()

# Load configurations from enabled packages
include("${CMAKE_CURRENT_LIST_DIR}/../XFEM/XFEMConfig.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/../ETA/ETAConfig.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/../Core/CoreConfig.cmake")

