#!/bin/sh

RTDB_IMAGE_NAME=dev
if [ $# -gt 0 ]; then
    RTDB_IMAGE_NAME=$1
fi

RTDB_IMAGE=rtdb/${RTDB_IMAGE_NAME}:latest

echo "Building image: ${RTDB_IMAGE}"
docker build -f src/Dockerfile.${RTDB_IMAGE_NAME} -t ${RTDB_IMAGE} .
