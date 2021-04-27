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
clean.sh # optional clean before build
build.sh
test.sh # optional tests and demo's
```

RTDB needs to know where to find the configuration file(s).
You should set environment variable RTDB_CONFIG_PATH.

## Also included

- Comm: process used to broadcast data between agents in the team
- Two compressors available (Lz4, zstd)
- Tool: dictionary generator (to update the dictionary for compression)
- Tool: rtop (provides realtime info of all items in RtDB) and many more command-line utilities

## License

RtDB is licensed under Apache 2.0

