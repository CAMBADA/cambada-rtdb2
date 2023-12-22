#
# FindLZ4.cmake
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
# Finds the LZ4 library. This module defines:
#   - LZ4_INCLUDE_DIR, directory containing headers
#   - LZ4_LIBRARIES, the LZ4 library path
#   - LZ4_FOUND, whether LZ4 has been found

if(NOT LZ4_FIND_VERSION)
    if(NOT LZ4_FIND_VERSION_MAJOR)
        set(LZ4_FIND_VERSION_MAJOR 0)
    endif(NOT LZ4_FIND_VERSION_MAJOR)
    if(NOT LZ4_FIND_VERSION_MINOR)
        set(LZ4_FIND_VERSION_MINOR 0)
    endif(NOT LZ4_FIND_VERSION_MINOR)
    if(NOT LZ4_FIND_VERSION_PATCH)
        set(LZ4_FIND_VERSION_PATCH 0)
    endif(NOT LZ4_FIND_VERSION_PATCH)

    set(LZ4_FIND_VERSION "${LZ4_FIND_VERSION_MAJOR}.${LZ4_FIND_VERSION_MINOR}.${LZ4_FIND_VERSION_PATCH}")
endif(NOT LZ4_FIND_VERSION)

# Find header files
if(LZ4_SEARCH_HEADER_PATHS)
    find_path(
            LZ4_INCLUDE_DIR lz4.h
            PATHS ${LZ4_SEARCH_HEADER_PATHS}
            NO_DEFAULT_PATH
    )
else()
    find_path(LZ4_INCLUDE_DIR lz4.h)
endif()

# Find library
if(LZ4_SEARCH_LIB_PATH)
    find_library(
            LZ4_LIBRARIES NAMES lz4
            PATHS ${LZ4_SEARCH_LIB_PATH}$
            NO_DEFAULT_PATH
    )
else()
    find_library(LZ4_LIBRARIES NAMES lz4)
endif()

if(LZ4_INCLUDE_DIR AND LZ4_LIBRARIES)
    file(READ "${LZ4_INCLUDE_DIR}/lz4.h" _lz4_version_header)

    string(REGEX MATCH "define[ \t]+LZ4_VERSION_MAJOR[ \t]+([0-9]+)" _lz4_major_version_match "${_lz4_version_header}")
    set(LZ4_MAJOR_VERSION "${CMAKE_MATCH_1}")
    string(REGEX MATCH "define[ \t]+LZ4_VERSION_MINOR[ \t]+([0-9]+)" _lz4_minor_version_match "${_lz4_version_header}")
    set(LZ4_MINOR_VERSION "${CMAKE_MATCH_1}")
    string(REGEX MATCH "define[ \t]+LZ4_VERSION_RELEASE[ \t]+([0-9]+)" _lz4_release_version_match "${_lz4_version_header}")
    set(LZ4_RELEASE_VERSION "${CMAKE_MATCH_1}")

    set(LZ4_VERSION ${LZ4_MAJOR_VERSION}.${LZ4_MINOR_VERSION}.${LZ4_RELEASE_VERSION})
    if(${LZ4_VERSION} VERSION_LESS ${LZ4_FIND_VERSION})
        set(LZ4_VERSION_OK FALSE)
    else(${LZ4_VERSION} VERSION_LESS ${LZ4_FIND_VERSION})
        set(LZ4_VERSION_OK TRUE)
    endif(${LZ4_VERSION} VERSION_LESS ${LZ4_FIND_VERSION})

    if(NOT LZ4_VERSION_OK)
        message(STATUS "Lz4 version ${LZ4_VERSION} found in ${LZ4_INCLUDE_DIR}, "
                "but at least version ${LZ4_FIND_VERSION} is required")
    else()
        message(STATUS "Found LZ4: ${LZ4_LIBRARIES} with version ${LZ4_VERSION}")
        set(LZ4_FOUND TRUE)
    endif(NOT LZ4_VERSION_OK)
else()
    set(LZ4_FOUND FALSE)
endif()

if(LZ4_FIND_REQUIRED AND NOT LZ4_FOUND)
    message(FATAL_ERROR "Could not find the LZ4 library.")
endif()
