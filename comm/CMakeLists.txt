cmake_minimum_required(VERSION 3.5.1)

project(RtDBcomm)

# system dependencies
find_package(Boost REQUIRED COMPONENTS system filesystem thread QUIET)

# libraries (internal)
add_library(RtDBcomm SHARED comm.cpp timer.cpp statistics.cpp MulticastSocket.cpp)
add_library(RtDB::comm ALIAS RtDBcomm)
target_link_libraries(RtDBcomm rtdb2 RtDButils)
set_target_properties(RtDBcomm PROPERTIES COMPILE_FLAGS "-fPIC")

# binary
add_executable(comm main.cpp)
target_link_libraries(comm
    RtDBcomm
    RtDButils
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_THREAD_LIBRARY}
)

