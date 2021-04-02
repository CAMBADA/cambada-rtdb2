cmake_minimum_required(VERSION 3.5.1)

project(RtDBcomm)

# system dependencies
find_package(Boost REQUIRED COMPONENTS system program_options filesystem thread QUIET)

# library
add_library(RtDBcomm SHARED
    comm.cpp
    timer.cpp
    statistics.cpp
    MulticastSocket.cpp
)
add_library(RtDB::comm ALIAS RtDBcomm)
target_link_libraries(RtDBcomm
    RtDB::rtdb
    RtDB::utils
)
target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)
set_target_properties(RtDBcomm PROPERTIES COMPILE_FLAGS "-fPIC")

# binary
add_executable(comm main.cpp)
target_link_libraries(comm
    RtDB::comm
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_THREAD_LIBRARY}
)

