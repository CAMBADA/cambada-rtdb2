### Looking for necessary system dependencies
find_package(Boost COMPONENTS system filesystem REQUIRED)

### Private Dependencies
IF(COMMAND cmake_policy)
    cmake_policy(SET CMP0022 NEW)
    IF(POLICY CMP0038)
        cmake_policy(SET CMP0038 OLD)
    ENDIF()
    IF(POLICY CMP0046)
        cmake_policy(SET CMP0046 OLD)
    ENDIF()
ENDIF(COMMAND cmake_policy)

include(ExternalProject)

### MsgPack Compilation
find_package(MsgPack 2.1.1 REQUIRED)
IF (MSGPACK_FOUND)
    include_directories(SYSTEM ${MSGPACK_INCLUDE_DIRS})
ENDIF()

### LMDB Compilation
find_package(LMDB 0.9.19 REQUIRED)
if(LMDB_FOUND)
    include_directories(SYSTEM ${LMDB_INCLUDE_DIR})
endif()

### LZ4 Compilation
find_package(LZ4 1.7.5 REQUIRED)
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
        serializer/RtDB2Serializer.h compressor/RtDB2Compressor.h storage/RtDB2Storage.h
        RtDB2Configuration.cpp RtDB2Configuration.h
        RtDB2Definitions.h
        RtDB2ErrorCode.h RtDB2ErrorCode.cpp
        storage/RtDB2StorageLMDB.cpp storage/RtDB2StorageLMDB.h
        compressor/RtDB2CompressorLZ4.cpp compressor/RtDB2CompressorLZ4.h
        compressor/RtDB2CompressorZstd.cpp compressor/RtDB2CompressorZstd.h)
# Library target dependencies (.so/.a)
set(RTDB2_LIBRARY_DEPENDENCIES
        ${LMDB_LIBRARIES}
        ${MSGPACK_LIBRARIES}
        ${LZ4_LIBRARIES} ${ZSTD_LIBRARIES}
        ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY}
        xerces-c -pthread)
### Adding RTDB2 as Library
add_library(rtdb2 STATIC ${RTDB2_LIBRARY_SOURCES})
target_link_libraries(rtdb2 ${RTDB2_LIBRARY_DEPENDENCIES})
# If the interface has a public include of the dependency (e.g. LMDB), it must be packed with the RtDB2 library
target_include_directories(rtdb2
        PUBLIC ${MSGPACK_INCLUDE_DIRS}
        PUBLIC ${LMDB_INCLUDE_DIR})

add_dependencies(rtdb2 ${RTDB2_TARGET_DEPENDENCIES})
set_target_properties(rtdb2 PROPERTIES COMPILE_FLAGS "-fPIC")

### Some programs used to debug RtDB2

### Call the CMake inside dictionary_generator
add_subdirectory(dictionary_generator)

