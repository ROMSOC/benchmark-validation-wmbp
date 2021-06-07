##############################################################################
#
# CMake variable for use by LifeV/XFEM clients. 
#
# Do not edit: This file was generated automatically by CMake.
#
##############################################################################

#
# Make sure CMAKE_CURRENT_LIST_DIR is usable
#

IF (NOT DEFINED CMAKE_CURRENT_LIST_DIR)
  GET_FILENAME_COMPONENT(_THIS_SCRIPT_PATH ${CMAKE_CURRENT_LIST_FILE} PATH)
  SET(CMAKE_CURRENT_LIST_DIR ${_THIS_SCRIPT_PATH})
ENDIF()


## ---------------------------------------------------------------------------
## Compilers used by LifeV/XFEM build
## ---------------------------------------------------------------------------

SET(XFEM_CXX_COMPILER "/usr/bin/mpicxx")

SET(XFEM_C_COMPILER "/usr/bin/mpicc")

SET(XFEM_FORTRAN_COMPILER "mpif90")


## ---------------------------------------------------------------------------
## Compiler flags used by LifeV/XFEM build
## ---------------------------------------------------------------------------

## Set compiler flags, including those determined by build type
SET(XFEM_CXX_FLAGS " -pedantic -Wall -Wno-long-long -Wwrite-strings  -DTRILIBRARY -DTETLIBRARY -std=c++11 -O3 -msse3 -Wno-unused-local-typedefs -Wno-literal-suffix -Wno-cpp -Wunused-parameter -O3")

SET(XFEM_C_FLAGS " -pedantic -Wall -Wno-long-long  -DTRILIBRARY -DTETLIBRARY -std=c++11 -O3 -msse3 -O3")

SET(XFEM_FORTRAN_FLAGS " ")

## Extra link flags (e.g., specification of fortran libraries)
SET(XFEM_EXTRA_LD_FLAGS "")

## This is the command-line entry used for setting rpaths. In a build
## with static libraries it will be empty. 
SET(XFEM_SHARED_LIB_RPATH_COMMAND "")
SET(XFEM_BUILD_SHARED_LIBS "OFF")

SET(XFEM_LINKER /usr/bin/ld)
SET(XFEM_AR ar)

## ---------------------------------------------------------------------------
## Set library specifications and paths 
## ---------------------------------------------------------------------------

## List of package include dirs
SET(XFEM_INCLUDE_DIRS "/home/lifev/lifev-src;/home/lifev/lifev-build;/usr/include/trilinos;/usr/include;/usr/include/superlu;/usr/include/hdf5/openmpi;/usr/include/scotch;/home/lifev/tetgen1.5.0")

## List of package library paths
SET(XFEM_LIBRARY_DIRS "/home/lifev/lifev-build/lifev/xfem;/home/lifev/lifev-build/lifev/eta;/home/lifev/lifev-build/lifev/core;/usr/lib/x86_64-linux-gnu")

## List of package libraries
SET(XFEM_LIBRARIES "lifevxfem;lifeveta;lifevcore")

## Specification of directories for TPL headers
SET(XFEM_TPL_INCLUDE_DIRS "/usr/include/trilinos;/usr/include;/usr/include/superlu;/usr/include/hdf5/openmpi;/usr/include/scotch;/usr/include;/home/lifev/tetgen1.5.0;/usr/include/hdf5/openmpi;/usr/include;/usr/include")

## Specification of directories for TPL libraries
SET(XFEM_TPL_LIBRARY_DIRS "/usr/lib/x86_64-linux-gnu")

## List of required TPLs
SET(XFEM_TPL_LIBRARIES "-L/usr/lib/x86_64-linux-gnu -ltrilinos_rythmos -ltrilinos_shylu -ltrilinos_stokhos_muelu -ltrilinos_stokhos_ifpack2 -ltrilinos_stokhos_amesos2 -ltrilinos_stokhos_tpetra -ltrilinos_stokhos_sacado -ltrilinos_stokhos -ltrilinos_locathyra -ltrilinos_locaepetra -ltrilinos_localapack -ltrilinos_loca -ltrilinos_noxepetra -ltrilinos_noxlapack -ltrilinos_nox -ltrilinos_sacado -ltrilinos_muelu-adapters -ltrilinos_muelu-interface -ltrilinos_muelu -ltrilinos_teko -ltrilinos_stratimikos -ltrilinos_stratimikosbelos -ltrilinos_stratimikosaztecoo -ltrilinos_stratimikosamesos -ltrilinos_stratimikosml -ltrilinos_stratimikosifpack -ltrilinos_ifpack2-adapters -ltrilinos_ifpack2 -ltrilinos_zoltan2 -ltrilinos_amesos2 -ltrilinos_ml -ltrilinos_galeri-xpetra -ltrilinos_galeri -ltrilinos_isorropia -ltrilinos_xpetra-sup -ltrilinos_xpetra -ltrilinos_thyratpetra -ltrilinos_thyraepetraext -ltrilinos_ifpack -ltrilinos_amesos -ltrilinos_anasazitpetra -ltrilinos_ModeLaplace -ltrilinos_anasaziepetra -ltrilinos_anasazi -ltrilinos_thyraepetra -ltrilinos_thyracore -ltrilinos_rtop -ltrilinos_belostpetra -ltrilinos_belosepetra -ltrilinos_belos -ltrilinos_tpetraext -ltrilinos_tpetrainout -ltrilinos_tpetra -ltrilinos_kokkostsqr -ltrilinos_tpetrakernels -ltrilinos_tpetraclassiclinalg -ltrilinos_tpetraclassicnodeapi -ltrilinos_tpetraclassic -ltrilinos_kokkosalgorithms -ltrilinos_kokkoscontainers -ltrilinos_aztecoo -ltrilinos_zoltan -ltrilinos_epetraext -ltrilinos_triutils -ltrilinos_epetra -ltrilinos_teuchoskokkoscomm -ltrilinos_teuchoskokkoscompat -ltrilinos_teuchosremainder -ltrilinos_teuchosnumerics -ltrilinos_teuchoscomm -ltrilinos_teuchosparameterlist -ltrilinos_teuchoscore -ltrilinos_kokkoscore;/usr/lib/libsmumps.so;/usr/lib/libdmumps.so;/usr/lib/libcmumps.so;/usr/lib/libzmumps.so;/usr/lib/libpord.so;/usr/lib/libmumps_common.so;/usr/lib/x86_64-linux-gnu/libsuperlu.so;/usr/lib/x86_64-linux-gnu/hdf5/openmpi/libhdf5.so;/usr/lib/x86_64-linux-gnu/libtbb.so;/usr/lib/x86_64-linux-gnu/libz.so;/usr/lib/libptscotch.so;/usr/lib/libptscotcherr.so;/usr/lib/libscotch.so;/usr/lib/libscotcherr.so;/usr/lib/liblapack.so;/usr/lib/libblas.so;/usr/lib/x86_64-linux-gnu/libbfd.so;/usr/lib/x86_64-linux-gnu/libiberty.a;/usr/lib/libtriangle.so;/home/lifev/tetgen1.5.0/libtet.a;/usr/lib/x86_64-linux-gnu/libz.so;/usr/lib/libparmetis.so;/usr/lib/x86_64-linux-gnu/libmetis.so;/usr/lib/liblapack.so;/usr/lib/libblas.so")

## ---------------------------------------------------------------------------
## MPI specific variables
##   These variables are provided to make it easier to get the mpi libraries
##   and includes on systems that do not use the mpi wrappers for compiling
## ---------------------------------------------------------------------------

SET(XFEM_MPI_LIBRARIES "")
SET(XFEM_MPI_LIBRARY_DIRS "")
SET(XFEM_MPI_INCLUDE_DIRS "")
SET(XFEM_MPI_EXEC "/usr/bin/mpiexec")
SET(XFEM_MPI_EXEC_MAX_NUMPROCS "4")
SET(XFEM_MPI_EXEC_NUMPROCS_FLAG "-np")

## ---------------------------------------------------------------------------
## Set useful general variables 
## ---------------------------------------------------------------------------

## The packages enabled for this project
SET(XFEM_PACKAGE_LIST "XFEM;ETA;Core")

## The TPLs enabled for this project
SET(XFEM_TPL_LIST "Trilinos;Triangle;TetGen;HDF5;ParMETIS;Boost;LAPACK;BLAS;MPI")


# Import LifeV targets
IF(NOT LifeV_TARGETS_IMPORTED)
  SET(LifeV_TARGETS_IMPORTED 1)
  INCLUDE("/home/lifev/lifev-build/LifeVTargets.cmake")
ENDIF()

