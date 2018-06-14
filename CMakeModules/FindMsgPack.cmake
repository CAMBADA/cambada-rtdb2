if(NOT MsgPack_FIND_VERSION)
    if(NOT MsgPack_FIND_VERSION_MAJOR)
        set(MsgPack_FIND_VERSION_MAJOR 0)
    endif(NOT MsgPack_FIND_VERSION_MAJOR)
    if(NOT MsgPack_FIND_VERSION_MINOR)
        set(MsgPack_FIND_VERSION_MINOR 0)
    endif(NOT MsgPack_FIND_VERSION_MINOR)
    if(NOT MsgPack_FIND_VERSION_PATCH)
        set(MsgPack_FIND_VERSION_PATCH 0)
    endif(NOT MsgPack_FIND_VERSION_PATCH)

    set(MsgPack_FIND_VERSION "${MsgPack_FIND_VERSION_MAJOR}.${MsgPack_FIND_VERSION_MINOR}.${MsgPack_FIND_VERSION_PATCH}")
endif(NOT MsgPack_FIND_VERSION)

include(CheckLibraryExists)

find_path(MSGPACK_INCLUDE_DIR
        NAMES msgpack.h
        )

find_library(MSGPACK_LIBRARY
        NAMES msgpack msgpackc libmsgpack.a libmsgpackc.a
        )

mark_as_advanced(MSGPACK_INCLUDE_DIR MSGPACK_LIBRARY)
set(MSGPACK_LIBRARIES ${MSGPACK_LIBRARY})
set(MSGPACK_INCLUDE_DIRS ${MSGPACK_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Msgpack
        MSGPACK_LIBRARY MSGPACK_INCLUDE_DIR)

if(MSGPACK_FOUND)
    file(READ "${MSGPACK_INCLUDE_DIR}/msgpack/version_master.h" _MSGPACK_version_header)

    string(REGEX MATCH "define[ \t]+MSGPACK_VERSION_MAJOR[ \t]+([0-9]+)" _MSGPACK_major_version_match "${_MSGPACK_version_header}")
    set(MSGPACK_MAJOR_VERSION "${CMAKE_MATCH_1}")
    string(REGEX MATCH "define[ \t]+MSGPACK_VERSION_MINOR[ \t]+([0-9]+)" _MSGPACK_minor_version_match "${_MSGPACK_version_header}")
    set(MSGPACK_MINOR_VERSION "${CMAKE_MATCH_1}")
    string(REGEX MATCH "define[ \t]+MSGPACK_VERSION_REVISION[ \t]+([0-9]+)" _MSGPACK_release_version_match "${_MSGPACK_version_header}")
    set(MSGPACK_RELEASE_VERSION "${CMAKE_MATCH_1}")

    set(MSGPACK_VERSION ${MSGPACK_MAJOR_VERSION}.${MSGPACK_MINOR_VERSION}.${MSGPACK_RELEASE_VERSION})
    if(${MSGPACK_VERSION} VERSION_LESS ${MsgPack_FIND_VERSION})
        set(MSGPACK_VERSION_OK FALSE)
    else(${MSGPACK_VERSION} VERSION_LESS ${MsgPack_FIND_VERSION})
        set(MSGPACK_VERSION_OK TRUE)
    endif(${MSGPACK_VERSION} VERSION_LESS ${MsgPack_FIND_VERSION})

    if(NOT MSGPACK_VERSION_OK)
        message(STATUS "Msgpack version ${MSGPACK_VERSION} found in ${MSGPACK_INCLUDE_DIR}, "
                "but at least version ${MsgPack_FIND_VERSION} is required")
        set(MSGPACK_FOUND FALSE)
    else()
        message(STATUS "Found Msgpack (include: ${MSGPACK_INCLUDE_DIR}, library: ${MSGPACK_LIBRARIES}) with version ${MSGPACK_VERSION}")
        mark_as_advanced(MSGPACK_INCLUDE_DIR MSGPACK_LIBRARIES)
        set(MSGPACK_FOUND TRUE)
    endif(NOT MSGPACK_VERSION_OK)
endif()