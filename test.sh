#!/bin/bash


# clear any previous database, if existing
# (it may have been locked by a different user?)
p=/tmp/rtdb2_storage # TODO: centralize; this is getting duplicated too much ...
if [ -e $p ]; then
    \rm -rf $p || exit 1
fi

# RTDB now requires this environment variable to be set
export RTDB_CONFIG_PATH=$(realpath $(dirname "$0"))/config
echo RTDB_CONFIG_PATH=$RTDB_CONFIG_PATH

# assumes build has completed successfully
cd build/
make test

