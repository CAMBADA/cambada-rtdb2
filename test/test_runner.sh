#!/bin/bash


cd "$(dirname "$0")"
./$1 | diff - $1.txt

