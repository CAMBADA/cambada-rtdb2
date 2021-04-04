#!/bin/bash

NCPUS=$(grep -c ^processor /proc/cpuinfo)
if [ ! -z ${NCPUS+x} ]; then
  MAKE_ARGS="-j${NCPUS}"
fi

[ ! -d build/ ] && mkdir build
cd build/
cmake .. && make ${MAKE_ARGS}

