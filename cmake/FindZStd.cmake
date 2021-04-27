#
# FindZSTD.cmake
#
#
# The MIT License
#
# Copyright (c) 2016 MIT and Intel Corporation
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
# Finds the Zstandard library. This module defines:
#   - ZSTD_INCLUDE_DIR, directory containing headers
#   - ZSTD_LIBRARIES, the Zstandard library path
#   - ZSTD_FOUND, whether Zstandard has been found

if(NOT ZStd_FIND_VERSION)
    if(NOT ZStd_FIND_VERSION_MAJOR)
        set(ZStd_FIND_VERSION_MAJOR 0)
    endif(NOT ZStd_FIND_VERSION_MAJOR)
    if(NOT ZStd_FIND_VERSION_MINOR)
        set(ZStd_FIND_VERSION_MINOR 0)
    endif(NOT ZStd_FIND_VERSION_MINOR)
    if(NOT ZStd_FIND_VERSION_PATCH)
        set(ZStd_FIND_VERSION_PATCH 0)
    endif(NOT ZStd_FIND_VERSION_PATCH)

    set(ZStd_FIND_VERSION "${ZStd_FIND_VERSION_MAJOR}.${ZStd_FIND_VERSION_MINOR}.${ZStd_FIND_VERSION_PATCH}")
endif(NOT ZStd_FIND_VERSION)

# Find header files
if(ZSTD_SEARCH_HEADER_PATHS)
    find_path(
            ZSTD_INCLUDE_DIR zstd.h
            PATHS ${ZSTD_SEARCH_HEADER_PATHS}
            NO_DEFAULT_PATH
    )
else()
    find_path(ZSTD_INCLUDE_DIR zstd.h)
endif()

# Find library
if(ZSTD_SEARCH_LIB_PATH)
    find_library(
            ZSTD_LIBRARIES NAMES zstd
            PATHS ${ZSTD_SEARCH_LIB_PATH}$
            NO_DEFAULT_PATH
    )
else()
    find_library(ZSTD_LIBRARIES NAMES zstd)
endif()

if(ZSTD_INCLUDE_DIR AND ZSTD_LIBRARIES)
    file(READ "${ZSTD_INCLUDE_DIR}/zstd.h" _ZStd_version_header)

    string(REGEX MATCH "define[ \t]+ZSTD_VERSION_MAJOR[ \t]+([0-9]+)" _ZStd_major_version_match "${_ZStd_version_header}")
    set(ZStd_MAJOR_VERSION "${CMAKE_MATCH_1}")
    string(REGEX MATCH "define[ \t]+ZSTD_VERSION_MINOR[ \t]+([0-9]+)" _ZStd_minor_version_match "${_ZStd_version_header}")
    set(ZStd_MINOR_VERSION "${CMAKE_MATCH_1}")
    string(REGEX MATCH "define[ \t]+ZSTD_VERSION_RELEASE[ \t]+([0-9]+)" _ZStd_release_version_match "${_ZStd_version_header}")
    set(ZStd_RELEASE_VERSION "${CMAKE_MATCH_1}")

    set(ZStd_VERSION ${ZStd_MAJOR_VERSION}.${ZStd_MINOR_VERSION}.${ZStd_RELEASE_VERSION})
    if(${ZStd_VERSION} VERSION_LESS ${ZStd_FIND_VERSION})
        set(ZStd_VERSION_OK FALSE)
    else(${ZStd_VERSION} VERSION_LESS ${ZStd_FIND_VERSION})
        set(ZStd_VERSION_OK TRUE)
    endif(${ZStd_VERSION} VERSION_LESS ${ZStd_FIND_VERSION})

    if(NOT ZStd_VERSION_OK)
        message(STATUS "ZStd version ${ZStd_VERSION} found in ${ZSTD_INCLUDE_DIR}, "
                "but at least version ${ZStd_FIND_VERSION} is required")
        set(ZSTD_FOUND FALSE)
    else()
        message(STATUS "Found Zstandard (include: ${ZSTD_INCLUDE_DIR}, library: ${ZSTD_LIBRARIES}) with version ${ZStd_VERSION}")
        mark_as_advanced(ZSTD_INCLUDE_DIR ZSTD_LIBRARIES)
        set(ZSTD_FOUND TRUE)
    endif(NOT ZStd_VERSION_OK)
else()
    set(ZSTD_FOUND FALSE)
endif()

if(ZSTD_FIND_REQUIRED AND NOT ZSTD_FOUND)
    message(FATAL_ERROR "Could not find the Zstandard library.")
endif()
