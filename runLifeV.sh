#! /bin/bash
LifeV_DIR=$PWD
LIFEV_SRC=$LifeV_DIR/lifev-src
LIFEV_BUILD=$LifeV_DIR/lifev-build
LIFEV_TET=$LifeV_DIR/tetgen1.5.0
mkdir -p ${LIFEV_BUILD}

gnome-terminal -e "docker run -it \
    mmartinolli/validation-wmbp:latest /bin/bash" &
