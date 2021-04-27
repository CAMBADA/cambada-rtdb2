#!/bin/bash


cd "$(dirname "$0")"
./demo_basics | diff - demo_basics.txt

