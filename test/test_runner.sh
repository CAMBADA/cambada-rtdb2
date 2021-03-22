#!/bin/bash

TESTBIN=$1
shift;

cd "$(dirname "$0")"
./$TESTBIN $* | diff - $TESTBIN.txt

