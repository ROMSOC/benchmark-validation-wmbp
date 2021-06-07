#!/bin/bash
LifeV_DIR=$PWD
LifeV_SRC=$LifeV_DIR/lifev-src
LifeV_BUILD=$LifeV_DIR/lifev-build
LifeV_LIB=$LifeV_DIR/lifev-install
LifeV_BENCHMARK=$LifeV_DIR/benchmark
TetGen_DIR=$LifeV_DIR/tetgen1.5.0
mkdir -p $LifeV_SRC
mkdir -p $LifeV_BUILD
mkdir -p $LifeV_LIB
mkdir -p $LifeV_BENCHMARK

git clone https://mmartinolli@bitbucket.org/Martin592/validation-wmbp.git ${LifeV_SRC}

git clone https://mmartinolli@bitbucket.org/zstefano/tetgen4life.git ${TetGen_DIR}

cp ${LifeV_SRC}/docker/*sh .

