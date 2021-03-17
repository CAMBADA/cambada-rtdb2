#!/bin/sh

RTDB_IMAGE_NAME=dev
if [ $# -gt 0 ]; then
    RTDB_IMAGE_NAME=$1
fi

RTDB_IMAGE=rtdb/${RTDB_IMAGE_NAME}:latest
RTDB_DIR=$(realpath $(dirname $0)/..)

NETWORK_NAME=rtdb-network
if [ -z $(docker network ls --filter name=^${NETWORK_NAME}$ --format="{{ .Name }}") ]; then 
     docker network create ${NETWORK_NAME}; 
fi

echo "Running image: ${RTDB_IMAGE}"
docker run --rm \
    --cap-add sys_nice \
    --network ${NETWORK_NAME} \
    -it \
    -v ${RTDB_DIR}:/rtdb \
    ${RTDB_IMAGE}
