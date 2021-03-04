# RtDB 2

Second Generation of CAMBADA's Real-time Database

http://robotica.ua.pt/CAMBADA/

## Requirements

This project uses some 3rd-party libraries:
- LZ4
- zstd
- LMDB
- Msgpack

## Instructions

```
build.sh
```

## Also included

- Comm: process used to broadcast data between agents in the team
- Two compressors available (Lz4, zstd)
- Adapter for RtDB v1 - makes it easy to upgrade from RtDB1 to RtDB2. It provides a stub interface to the new one, maintaining the old API.
- Tool: dictionary generator (to update the dictionary for compression)
- Tool: rtdb2top (provides realtime info of all items in RtDB)

