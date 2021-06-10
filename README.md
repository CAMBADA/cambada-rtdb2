# RtDB v3

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

Modification by Rob:
- ... description

TODO: cleanup this header section

## Requirements

This project uses some 3rd-party libraries:
- LZ4
- zstd
- LMDB
- Msgpack
- Xerces-c
- xsdcxx

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

## Developer

Create rtdb client for agent 1 that reads/writes data to the default database as specified in the default configuration file:

```
RtDB2Context ctx = RtDB2Context::Builder(1).build();
RtDB2 rtdb(ctx);
```

Create rtdb client for agent 1 that gives access to the data that remote agent 2 shares in the default database as specified in the default configuration file:

```
RtDB2Context ctx = RtDB2Context::Builder(1).build();
RtDB2 rtdbRemote2(ctx, 2);
```

## Configuration

RtDB configuration consists of these independent sections:

* Networks              (M) this section is used by the comm process
* Databases             (M) this section is used by the client process
* InterfacePriorityList (O) in case 'auto' is used as Interface, then given sequence is used to prioritize
* InterfaceBlackList    (O) in case 'auto' is used as Interface, then given interfaces are ignored

### Networks

The Networks section contains the individual Network definitions. Each Network definition must have a unique name.

M: mandatory element/atrribute  
O: optional element/attribute

Network attributes:

* name             (O) unique name of the Network (default 'default')
* database         (O) database to shared on this Network (default 'default')
* loopback         (O) whether or not to receive own messages (default 'false')
* send             (O) ?

Network elements:

* MulticastAddress (M) assigned group address according to MSL rules
* MulticastPort    (M) port to use
* Interface        (O) interface overrule, 'auto' (default) will resolve to available adapter, thereby taking into account the InterfacePriorityList and InterfaceBlackList when specified
* Frequency        (M) transmitter frequency (Hz)

### Databases

The Databases section contains the individual Database definitions. Each Database definition must have a unique name.

Database attributes:

* name             (O) unique name of the Network (default 'default')

Database elements:

* Compression      (O) enables compression
* KeyDefaults      (O) specifies the default values for the Key attributes for this database. when omitted the system defaults are used.
* Keys             (M) specifies the keys for this database.

Compression attributes:

* type             (O) type of compression: 'zstd' or 'lz4' (default: 'zstd')

Compression elements:

* UseDictionary    (O) enables use of dictionary

Key/KeyDefaults attributes:

* shared           (O) specifies if the object is shared or local to real-time database (default rue').
* period           (O) specifies the sharing interval (default '1' indicating each interval).
* phase            (O) specifies in which phase of a period an item is shared (default '0' indicating first slot of a period).
* timeout          (O) stop sending items after their age exceeds timeout in seconds (default '0' indicating no timeout).

Key attributes:

* id               (M) unique string identification of this key.

Example configuration:

```xml
<RtDB2Configuration>
  <Networks>
    <Network loopback="false" send="true">
      <MulticastAddress>224.16.32.74</MulticastAddress>
      <MulticastPort>8001</MulticastPort>
      <Interface>auto</Interface>
      <Frequency>30</Frequency>
    </Network>
  </Networks>
  <Databases>
    <Database>
      <Compression type="zstd"/>
      <KeyDefaults shared="false" period="1" phase="0" timeout="1.0"/>
      <Keys>
        <Key id="EXAMPLE_ITEM"/>
        <Key id="EXAMPLE_ITEM_MAPPED" shared="false"/>
      </Keys>
    </Database>
  </Databases>
  <InterfacePriorityList>wlo1 enp3s0 eth0</InterfacePriorityList>
  <InterfaceBlackList>enp0s31f6 lo</InterfaceBlackList>
</RtDB2Configuration>
```

## Dependencies

The table lists the RtDB binaries and libraries and their dependencies.

| Name                 | Type   | Dependencies |
| -------------------- | ------ | - |
| RtDB::config         | static | - |
| RtDB::definitions    | static | - |
| RtDB::utils          | shared | RtDB::definitions |
| RtDB::rtdb           | static | RtDB::definitions RtDB::utils RtDB::config |
| RtDB::comm           | shared | RtDB::rtdb RtDB::utils |
| rtdb2_dictionary_gen | binary | RtDB::rtdb |
| comm                 | binary | RtDB::comm RtDB::utils |

## Also included

- Comm: process used to broadcast data between agents in the team
- Two compressors available (Lz4, zstd)
- Tool: dictionary generator (to update the dictionary for compression)
- Tool: rtop (provides realtime info of all items in RtDB) and many more command-line utilities

## License

RtDB is licensed under Apache 2.0

