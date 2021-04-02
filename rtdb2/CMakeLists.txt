cmake_minimum_required(VERSION 3.5.1)

project(RtDBrtdb)
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

### xerces-c
find_package(XercesC 3.2.0 REQUIRED)
if(XercesC_FOUND)
    include_directories(SYSTEM ${XercesC_INCLUDE_DIRS})
endif()

### RtDB2 Compilation
set(RTDB2_TARGET_DEPENDENCIES lmdb msgpack lz4 zstd)
# Library target sources (cpp/h)
set(RTDB2_LIBRARY_SOURCES
        RtDB2.cpp RtDB2.h
        RtDB2Context.cpp RtDB2Context.h
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
    ${LZ4_LIBRARIES}
    ${ZSTD_LIBRARIES}
    ${Boost_LIBRARIES}
    xerces-c
    pthread
)

### Library rtdb (static)
add_library(${PROJECT_NAME} STATIC ${RTDB2_LIBRARY_SOURCES})
add_library(RtDB::rtdb ALIAS ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME}
    RtDB::definitions
    RtDB::config
    RtDB::utils
    ${RTDB2_LIBRARY_DEPENDENCIES}
)
target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)
set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-fPIC")

### Some programs used to debug RtDB2

### Call the CMake inside dictionary_generator
#TODO add_subdirectory(dictionary_generator)
add_subdirectory(configuration)
