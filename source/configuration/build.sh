#!/bin/bash
LifeV_DIR=$PWD
LifeV_SRC=$LifeV_DIR/lifev-src
LifeV_BUILD=$LifeV_DIR/lifev-build
LifeV_LIB=$LifeV_DIR/lifev-install
TetGen_DIR=$LifeV_DIR/tetgen1.5.0
LIFEV_BENCHMARK=$LifeV_DIR/benchmark

cd $LifeV_BUILD

make -j 3

cd $LifeV_DIR

cp $LifeV_BUILD/lifev/xfem/examples/validation/XFEM_BENCHMARK.exe WMBP.exe

mkdir $LIFEV_BENCHMARK
cp WMBP.exe $LIFEV_BENCHMARK/.
cp $LifeV_SRC/lifev/xfem/examples/validation/dataFile* $LIFEV_BENCHMARK/.
cp $LifeV_SRC/lifev/xfem/examples/validation/solverFile* $LIFEV_BENCHMARK/.
cp -r $LifeV_SRC/lifev/xfem/examples/validation/meshes* $LIFEV_BENCHMARK/.
cp -r $LifeV_SRC/data $LIFEV_BENCHMARK/.
cp -r $LifeV_SRC/post-processing $LIFEV_BENCHMARK/.