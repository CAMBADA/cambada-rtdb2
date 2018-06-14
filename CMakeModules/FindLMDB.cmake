# Try to find the LMBD libraries and headers
#  LMDB_FOUND - system has LMDB lib
#  LMDB_INCLUDE_DIR - the LMDB include directory
#  LMDB_LIBRARIES - Libraries needed to use LMDB

if(NOT LMDB_FIND_VERSION)
    if(NOT LMDB_FIND_VERSION_MAJOR)
        set(LMDB_FIND_VERSION_MAJOR 0)
    endif(NOT LMDB_FIND_VERSION_MAJOR)
    if(NOT LMDB_FIND_VERSION_MINOR)
        set(LMDB_FIND_VERSION_MINOR 0)
    endif(NOT LMDB_FIND_VERSION_MINOR)
    if(NOT LMDB_FIND_VERSION_PATCH)
        set(LMDB_FIND_VERSION_PATCH 0)
    endif(NOT LMDB_FIND_VERSION_PATCH)

    set(LMDB_FIND_VERSION "${LMDB_FIND_VERSION_MAJOR}.${LMDB_FIND_VERSION_MINOR}.${LMDB_FIND_VERSION_PATCH}")
endif(NOT LMDB_FIND_VERSION)

find_path(LMDB_INCLUDE_DIR NAMES  lmdb.h PATHS "$ENV{LMDB_DIR}/include")
find_library(LMDB_LIBRARIES NAMES lmdb   PATHS "$ENV{LMDB_DIR}/lib" )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LMDB DEFAULT_MSG LMDB_INCLUDE_DIR LMDB_LIBRARIES)

if(LMDB_FOUND)
    file(READ "${LMDB_INCLUDE_DIR}/lmdb.h" _LMDB_version_header)

    string(REGEX MATCH "define[ \t]+MDB_VERSION_MAJOR[ \t]+([0-9]+)" _LMDB_major_version_match "${_LMDB_version_header}")
    set(LMDB_MAJOR_VERSION "${CMAKE_MATCH_1}")
    string(REGEX MATCH "define[ \t]+MDB_VERSION_MINOR[ \t]+([0-9]+)" _LMDB_minor_version_match "${_LMDB_version_header}")
    set(LMDB_MINOR_VERSION "${CMAKE_MATCH_1}")
    string(REGEX MATCH "define[ \t]+MDB_VERSION_PATCH[ \t]+([0-9]+)" _LMDB_release_version_match "${_LMDB_version_header}")
    set(LMDB_RELEASE_VERSION "${CMAKE_MATCH_1}")

    set(LMDB_VERSION ${LMDB_MAJOR_VERSION}.${LMDB_MINOR_VERSION}.${LMDB_RELEASE_VERSION})
    if(${LMDB_VERSION} VERSION_LESS ${LMDB_FIND_VERSION})
        set(LMDB_VERSION_OK FALSE)
    else(${LMDB_VERSION} VERSION_LESS ${LMDB_FIND_VERSION})
        set(LMDB_VERSION_OK TRUE)
    endif(${LMDB_VERSION} VERSION_LESS ${LMDB_FIND_VERSION})

    if(NOT LMDB_VERSION_OK)
        message(STATUS "LMDB version ${LMDB_VERSION} found in ${LMDB_INCLUDE_DIR}, "
                "but at least version ${LMDB_FIND_VERSION} is required")
        set(LMDB_FOUND FALSE)
    else()
        message(STATUS "Found lmdb (include: ${LMDB_INCLUDE_DIR}, library: ${LMDB_LIBRARIES}) with version ${LMDB_VERSION}")
        mark_as_advanced(LMDB_INCLUDE_DIR LMDB_LIBRARIES)
        set(LMDB_FOUND TRUE)
    endif(NOT LMDB_VERSION_OK)
endif()