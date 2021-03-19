# RtDB v2.5

Extension on 2nd Generation of CAMBADA's Real-time Database

http://robotica.ua.pt/CAMBADA/

Modified by Falcons:
- upgrading comm to comm2 (and later renaming it back to comm)
- redesign database backend a bit
- remove legacy rtdb1 adapter
- extend semaphore-based wait functionality
- change license from GPLv3 to Apache2.0
- rewrite basic interfacing

Also, a logging- and playback library was created, but it currently resides in Falcons codebase.

## Requirements

This project uses some 3rd-party libraries:
- LZ4
- zstd
- LMDB
- Msgpack

## Instructions

```
./clean.sh # optional clean before build
./build.sh
./test.sh # optional tests and demo's
```

RTDB needs to know where to find the configuration file(s).
You should set environment variable RTDB_CONFIG_PATH.

## Docker

The instructions in the previous chapter can also be performed in a docker environment. To create a docker image for rtdb run `docker-build.sh` and to start the docker image run `docker-run.sh`:

```
cd docker
./docker-build.sh
./docker-run.sh
```

Now, inside the docker container, the commands described in the previous chapter can be executed to build rtdb.

After rtdb has been build successfully, the comm process can be run with (still inside the docker container):

```
cd build/comm
AGENT=1 ./comm
```

Starting a second container and running an AGENT with another id, will show two agents that are communicating with each other:

```
./docker-run.sh
cd build/comm
AGENT=2 ./comm
```


## Also included

- Comm: process used to broadcast data between agents in the team
- Two compressors available (Lz4, zstd)
- Tool: dictionary generator (to update the dictionary for compression)
- Tool: rtop (provides realtime info of all items in RtDB) and many more command-line utilities

## License

RtDB is licensed under Apache 2.0

