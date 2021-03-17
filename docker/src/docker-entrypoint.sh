#!/bin/bash
set -e

# Sourcing doesn't work properly, use this workaround
# source /root/.bashrc
eval "$(cat ~/.bashrc | tail -n +10)"

if [ -z ${RTDB_BUILD+x} ]; then
  echo "RTDB_BUILD environment variable is not set. Aborting."
  exec "$@"
fi

# Make sure the directory is mounted properly
RTDBDIR="$(dirname "${RTDB_BUILD}")"
if [ ! -d "$RTDBDIR" ]; then
    echo "ERROR: rtdb directory not found at '$RTDBDIR'"
    echo "       Make sure to mount the directory into the docker container by"
    echo "         adding '-v \$(pwd):/rtdb' to the docker run command."
    exec "$@"
fi

# Set up user in container with same UID:GID as host
DOCKER_USER="rtdb"
OWNER_UID=$(stat -c '%u' $RTDBDIR)
OWNER_GID=$(stat -c '%g' $RTDBDIR)

if [ "$(id -u)" = "0" ]; then
    groupadd --gid $OWNER_GID $DOCKER_USER
    useradd --gid $OWNER_GID --uid $OWNER_UID -MN $DOCKER_USER
fi

# Make sure the build directory exists
if [ ! -d "$RTDB_BUILD" ]; then
    mkdir -p $RTDB_BUILD
    chown $OWNER_UID:$OWNER_GID $RTDB_BUILD
fi

# Make /rtdb the default directory on startup
cd $RTDBDIR

# Drop permissions from root to user
set -- gosu $DOCKER_USER "$@"

# unused
exec "$@"
