#!/bin/bash

TESTBIN=$1
shift;

unset AGENT
unset RTDB_CONFIG_PATH

cd "$(dirname "$0")"
./$TESTBIN $* | diff $TESTBIN.txt -

