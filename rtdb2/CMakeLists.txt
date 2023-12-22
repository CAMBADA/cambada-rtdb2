cmake_minimum_required(VERSION 3.5.1)

project(rtdb2)
# TODO below listing could use some cleanup / modernizing


### Looking for necessary system dependencies
find_package(Boost COMPONENTS system thread filesystem REQUIRED QUIET)


set(CMAKE_BUILD_TYPE Release)
#set(CMAKE_BUILD_TYPE Debug)


include(ExternalProject)

### MsgPack Compilation
find_package(MsgPack 2.1.1 REQUIRED)
IF (MSGPACK_FOUND)
    include_directories(SYSTEM ${MSGPACK_INCLUDE_DIRS})
ENDIF()

### LMDB Compilation
find_package(LMDB 0.9.17 REQUIRED)
if(LMDB_FOUND)
    include_directories(SYSTEM ${LMDB_INCLUDE_DIR})
endif()

### LZ4 Compilation
find_package(LZ4 1.7.1 REQUIRED)
if(LZ4_FOUND)
    include_directories(SYSTEM ${LZ4_INCLUDE_DIR})
endif()

### zstd Compilation
find_package(ZStd 1.1.4 REQUIRED)
if(ZSTD_FOUND)
    include_directories(SYSTEM ${ZSTD_INCLUDE_DIR})
endif()

### RtDB2 Compilation
set(RTDB2_TARGET_DEPENDENCIES lmdb msgpack lz4 zstd)
# Library target sources (cpp/h)
set(RTDB2_LIBRARY_SOURCES
        RtDB2.cpp RtDB2.h
        RtDB2Item.h
        RtDB2Store.cpp RtDB2Store.h
        serializer/RtDB2Serializer.h compressor/RtDB2Compressor.h storage/RtDB2Storage.h
        RtDB2Configuration.cpp RtDB2Configuration.h
        RtDB2ErrorCode.h RtDB2ErrorCode.cpp
        storage/RtDB2StorageLMDB.cpp storage/RtDB2StorageLMDB.h
        compressor/RtDB2CompressorLZ4.cpp compressor/RtDB2CompressorLZ4.h
        compressor/RtDB2CompressorZstd.cpp compressor/RtDB2CompressorZstd.h)
# Library target dependencies (.so/.a)
set(RTDB2_LIBRARY_DEPENDENCIES
        ${LMDB_LIBRARIES}
        ${MSGPACK_LIBRARIES}
        ${LZ4_LIBRARIES} ${ZSTD_LIBRARIES}
        ${Boost_LIBRARIES}
        xerces-c pthread)
### Adding RTDB2 as Library
add_library(rtdb2 ${RTDB2_LIBRARY_SOURCES})
target_link_libraries(rtdb2 RtDB::definitions RtDB::utils)
target_link_libraries(rtdb2 ${RTDB2_LIBRARY_DEPENDENCIES})
# If the interface has a public include of the dependency (e.g. LMDB), it must be packed with the RtDB2 library
target_include_directories(rtdb2
        PUBLIC ${MSGPACK_INCLUDE_DIRS}
        PUBLIC ${LMDB_INCLUDE_DIR})

set_target_properties(rtdb2 PROPERTIES COMPILE_FLAGS "-fPIC")

### Some programs used to debug RtDB2

### Call the CMake inside dictionary_generator
#TODO add_subdirectory(dictionary_generator)

