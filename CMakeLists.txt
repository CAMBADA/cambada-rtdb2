cmake_minimum_required(VERSION 3.5.1)

project(RtDB LANGUAGES CXX)

option(RTDB_TESTS_ENABLED "Build all tests" ON)

if (RTDB_TESTS_ENABLED)
  enable_testing()
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -ftemplate-depth=1200 -Wall -Werror -Wvla")
# TODO this is apparently not a modern way to deal with compiler flags

# system dependencies
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_CURRENT_LIST_DIR}/cmake)

# components
add_subdirectory(comm)
add_subdirectory(config)
add_subdirectory(definitions) # TODO only for making msgpack macros available -> rename to "serialize"?
#add_subdirectory(dictionary_generator) # broken
add_subdirectory(rtdb2) # NOTE: the 2 postfix was introduced by Cambada next to their legacy rtdb API - maybe best to keep peculiar naming as is?
add_subdirectory(utils)

if (RTDB_TESTS_ENABLED)
  add_subdirectory(test)
endif()
