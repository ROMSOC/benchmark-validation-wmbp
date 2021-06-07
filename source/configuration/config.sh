#!/bin/bash

LifeV_DIR=$PWD
LifeV_SRC=$LifeV_DIR/lifev-src
LifeV_BUILD=$LifeV_DIR/lifev-build
LifeV_LIB=$LifeV_DIR/lifev-install
TetGen_DIR=$LifeV_DIR/tetgen1.5.0
cd $TetGen_DIR

make tetlib

cd $LifeV_BUILD

cmake \
    -D BUILD_SHARED_LIBS:BOOL=OFF \
    -D CMAKE_BUILD_TYPE:STRING=RELEASE \
    -D CMAKE_INSTALL_PREFIX:PATH=${LifeV_LIB}\
    -D CMAKE_C_COMPILER:STRING="mpicc" \
    -D CMAKE_CXX_COMPILER:STRING="mpicxx" \
    -D CMAKE_CXX_FLAGS:STRING="-O3 -msse3 -Wno-unused-local-typedefs -Wno-literal-suffix -Wno-cpp -Wunused-parameter" \
    -D CMAKE_C_FLAGS:STRING="-O3 -msse3" \
    -D CMAKE_Fortran_COMPILER:STRING="mpif90" \
    -D CMAKE_Fortran_FLAGS:STRING="-Og -g" \
    -D CMAKE_AR:STRING="ar" \
    -D CMAKE_MAKE_PROGRAM:STRING="make" \
        \
    -D TPL_ENABLE_AMD=ON \
    -D   AMD_INCLUDE_DIRS=/usr/include/suitesparse/ \
    -D   AMD_LIBRARY_DIRS=/usr/lib/x86_64-linux-gnu/ \
    -D   AMD_LIBRARY_NAMES=amd \
    -D TPL_ENABLE_BLACS=ON \
    -D   BLACS_INCLUDE_DIRS=/usr/include/ \
    -D   BLACS_LIBRARY_DIRS=/usr/lib/ \
    -D   BLACS_LIBRARY_NAMES=blacs \
    -D TPL_ENABLE_Boost=ON \
    -D   Boost_INCLUDE_DIRS=/usr/include/boost/ \
    -D TPL_ENABLE_BoostLib=ON \
    -D   Boost_NO_BOOST_CMAKE:BOOL=ON \
    -D   BoostLib_INCLUDE_DIRS=/usr/include/boost/ \
    -D   BoostLib_LIBRARY_DIRS=/usr/lib/x86_64-linux-gnu/ \
    -D TPL_ENABLE_HDF5=ON \
    -D   HDF5_INCLUDE_DIRS=/usr/include/hdf5/openmpi/ \
    -D   HDF5_LIBRARY_DIRS=/usr/lib/x86_64-linux-gnu/ \
    -D TPL_ENABLE_SCALAPACK=ON \
    -D   SCALAPACK_INCLUDE_DIRS= \
    -D   SCALAPACK_LIBRARY_DIRS=/usr/lib/ \
    -D   SCALAPACK_LIBRARY_NAMES=scalapack \
    -D TPL_ENABLE_MPI=ON \
    -D   MPI_BASE_DIR:PATH=/usr/ \
    -D   ParMETIS_INCLUDE_DIRS=/usr/include/ \
    -D   ParMETIS_LIBRARY_DIRS=/usr/lib/x86_64-linux-gnu/ \
    -D TPL_ENABLE_UMFPACK=ON \
    -D   UMFPACK_INCLUDE_DIRS=/usr/include/suitesparse/ \
    -D   UMFPACK_LIBRARY_DIRS=/usr/lib/x86_64-linux-gnu/ \
    -D   UMFPACK_LIBRARY_NAMES=umfpack \
    -D   Trilinos_INCLUDE_DIRS:PATH="/usr/include/trilinos/" \
    -D   Trilinos_LIBRARY_DIRS:PATH="/usr/lib/x86_64-linux-gnu/" \
    -D   TetGen_INCLUDE_DIRS:PATH="${TetGen_DIR}" \
    -D   TetGen_LIBRARY_DIRS:PATH="${TetGen_DIR}" \
    -D   TetGen_LIBRARY_NAMES="tet" \
    -D   Triangle_INCLUDE_DIRS:PATH="/usr/include" \
    -D   Triangle_LIBRARY_DIRS:PATH="/usr/lib" \
    -D   Triangle_LIBRARY_NAMES="triangle" \
        \
    -D   LifeV_ENABLE_DEBUG:BOOL=OFF \
    -D   LifeV_ENABLE_TESTS:BOOL=ON \
        \
    -D   LifeV_ENABLE_ALL_PACKAGES:BOOL=ON \
    -D   LifeV_ENABLE_Core:BOOL=ON \
    -D   LifeV_ENABLE_ETA:BOOL=ON \
    -D   LifeV_ENABLE_NavierStokes:BOOL=ON \
    -D   LifeV_ENABLE_BCInterface:BOOL=ON \
    -D   LifeV_ENABLE_Structure:BOOL=ON \
    -D   LifeV_ENABLE_ZeroDimensional:BOOL=ON \
    -D   LifeV_ENABLE_OneDFSI:BOOL=ON \
    -D   LifeV_ENABLE_LevelSet:BOOL=ON \
    -D   LifeV_ENABLE_Darcy:BOOL=ON \
    -D   LifeV_ENABLE_Electrophysiology:BOOL=OFF \
    -D   LifeV_ENABLE_Heart:BOOL=OFF \
    -D   LifeV_ENABLE_FSI:BOOL=ON \
    -D   LifeV_ENABLE_Multiscale:BOOL=ON \
    -D   LifeV_ENABLE_XFEM:BOOL=ON \
    -D   LifeV_ENABLE_Dummy:BOOL=OFF \
        \
    -D   LifeV_STRUCTURE_ENABLE_ANISOTROPIC:BOOL=ON \
    -D   LifeV_STRUCTURE_COLORING_MESH:BOOL=ON \
    -D   LifeV_STRUCTURE_COMPUTATION_JACOBIAN:BOOL=ON \
    -D   LifeV_STRUCTURE_EXPORTVECTORS:BOOL=ON \
        \
    -D   BCInterface_ENABLE_TESTS:BOOL=ON \
    -D   Core_ENABLE_TESTS:BOOL=ON \
    -D   Darcy_ENABLE_TESTS:BOOL=ON \
    -D   Dummy_ENABLE_TESTS:BOOL=OFF \
    -D   Electrophysiology_ENABLE_TESTS:BOOL=OFF \
    -D   ETA_ENABLE_TESTS:BOOL=ON \
    -D   FSI_ENABLE_TESTS:BOOL=ON \
    -D   Heart_ENABLE_TESTS:BOOL=OFF \
    -D   LevelSet_ENABLE_TESTS:BOOL=ON \
    -D   Multiscale_ENABLE_TESTS:BOOL=ON \
    -D   NavierStokes_ENABLE_TESTS:BOOL=ON \
    -D   OneDFSI_ENABLE_TESTS:BOOL=ON \
    -D   Structure_ENABLE_TESTS:BOOL=ON \
    -D   XFEM_ENABLE_TESTS:BOOL=ON \
    -D   ZeroDimensional_ENABLE_TESTS:BOOL=ON \
        \
    -D   BCInterface_ENABLE_EXAMPLES:BOOL=ON \
    -D   Core_ENABLE_EXAMPLES:BOOL=ON \
    -D   Darcy_ENABLE_EXAMPLES:BOOL=ON \
    -D   Dummy_ENABLE_EXAMPLES:BOOL=OFF \
    -D   Electrophysiology_ENABLE_EXAMPLES:BOOL=OFF \
    -D   ETA_ENABLE_EXAMPLES:BOOL=ON \
    -D   FSI_ENABLE_EXAMPLES:BOOL=ON \
    -D   Heart_ENABLE_EXAMPLES:BOOL=OFF \
    -D   LevelSet_ENABLE_EXAMPLES:BOOL=ON \
    -D   Multiscale_ENABLE_EXAMPLES:BOOL=ON \
    -D   NavierStokes_ENABLE_EXAMPLES:BOOL=ON \
    -D   OneDFSI_ENABLE_EXAMPLES:BOOL=ON \
    -D   Structure_ENABLE_EXAMPLES:BOOL=ON \
    -D   XFEM_ENABLE_EXAMPLES:BOOL=ON \
    -D   ZeroDimensional_ENABLE_EXAMPLES:BOOL=ON \
        \
    -D   LifeV_ENABLE_STRONG_CXX_COMPILE_WARNINGS:BOOL=ON \
       ${LifeV_SRC}

cd $LifeV_DIR


