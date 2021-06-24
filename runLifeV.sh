#! /bin/bash
LIFEV_BENCHMARK=$LifeV_DIR/benchmark

gnome-terminal -e "docker run -it \
    -v ${LIFEV_BENCHMARK}:/home/lifev/benchmark \
    martin592/validation-wmbp:base /bin/bash" &
