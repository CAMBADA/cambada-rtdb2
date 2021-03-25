#!/bin/bash


# clear any previous database, if existing
# (it may have been locked by a different user?)
p=/tmp/rtdb2_storage # TODO: centralize; this is getting duplicated too much ...
if [ -e $p ]; then
    \rm -rf $p || exit 1
fi

# Generate output when test fails
export CTEST_OUTPUT_ON_FAILURE=1

# assumes build has completed successfully
cd build/
make test

