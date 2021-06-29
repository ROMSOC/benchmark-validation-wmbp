#! /bin/bash
LifeV_DIR=$PWD
LIFEV_BENCHMARK=$LifeV_DIR/benchmark

gnome-terminal -e "docker run -it \
    -v ${LIFEV_BENCHMARK}:/home/lifev/benchmark \
    martin592/lifev-validation:env /bin/bash" &
