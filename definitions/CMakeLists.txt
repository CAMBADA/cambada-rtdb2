cmake_minimum_required(VERSION 3.5.1)

project(RtDBdefinitions)

# system dependencies
find_package(msgpack)

# this sub-library exports a single header
add_library(${PROJECT_NAME} INTERFACE)
add_library(RtDB::definitions ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
    INTERFACE
        ${PROJECT_SOURCE_DIR}/include
)

