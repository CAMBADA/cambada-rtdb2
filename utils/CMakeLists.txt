cmake_minimum_required(VERSION 3.5.1)

project(RtDButils)

# system dependencies
find_package(Boost REQUIRED COMPONENTS system filesystem thread QUIET)

# libraries
add_library(${PROJECT_NAME} SHARED src/rtime.cpp src/tprintf.cpp)
add_library(RtDB::utils ALIAS ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} RtDB::definitions)

target_include_directories(${PROJECT_NAME}
    PUBLIC 
        #$<INSTALL_INTERFACE:include> # TODO?
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

