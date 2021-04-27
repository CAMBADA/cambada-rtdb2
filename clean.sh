#!/bin/bash

for d in build/ bin/ lib/ ; do
    [ -d $d ] && rm -rf $d
done
exit 0

