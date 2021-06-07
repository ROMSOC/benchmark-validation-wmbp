#! /bin/bash
LifeV_DIR=$PWD
LIFEV_SRC=$LifeV_DIR/lifev-src
LIFEV_BUILD=$LifeV_DIR/lifev-build
LIFEV_TET=$LifeV_DIR/tetgen1.5.0
LIFEV_BENCHMARK=$LifeV_DIR/benchmark
mkdir -p ${LIFEV_BUILD}

gnome-terminal -e "docker run -it \
    -v ${LIFEV_SRC}:/home/lifev/lifev-src \
    -v ${LIFEV_BUILD}:/home/lifev/lifev-build \
    -v ${LIFEV_TET}:/home/lifev/tetgen1.5.0 \
    -v ${LIFEV_BENCHMARK}:/home/lifev/benchmark \
    martin592/validation-wmbp:latest /bin/bash" &
