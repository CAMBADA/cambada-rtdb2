#!/bin/bash


# first clean
for d in build/ bin/ lib/ ; do
    [ -d $d ] && rm -rf $d
done

# now build
mkdir build/
cd build/
cmake .. && make

# TODO: test?

