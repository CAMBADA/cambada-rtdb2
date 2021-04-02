cmake_minimum_required(VERSION 3.5.1)

project(RtDBconfig)

# make sure xsdxcc is installed
find_package(XSDCXX REQUIRED)

# name of generated source file
set(RTDB_CONFIGURATION_XSD ${CMAKE_CURRENT_SOURCE_DIR}/rtdb_configuration.xsd)
set(RTDB_CONFIGURATION_H rtdb_configuration_generated.h)
set(RTDB_CONFIGURATION_CPP rtdb_configuration_generated.cpp)

### Define and create a directory for generated header files
set(GENERATED_INC_DIR ${CMAKE_CURRENT_BINARY_DIR}/include)
file(MAKE_DIRECTORY ${GENERATED_INC_DIR})
include_directories(${GENERATED_INC_DIR})

set(SOURCE_FILES
  ${RTDB_CONFIGURATION_CPP}
)

# library (static)
add_library(${PROJECT_NAME}
  STATIC
  ${SOURCE_FILES}
)
add_library(RtDB::config ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${GENERATED_INC_DIR}>
)
set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-fPIC")

add_dependencies(${PROJECT_NAME}
  rtdbconfig_xsd
)

# https://www.codesynthesis.com/projects/xsd/documentation/cxx/tree/guide/
add_custom_command(OUTPUT ${RTDB_CONFIGURATION_CPP}
  COMMAND ${XSDCXX} cxx-tree
    --std c++11
    --namespace-map =rtdbconfig
    --generate-serialization
    --hxx-suffix _generated.h
    --cxx-suffix _generated.cpp
    ${RTDB_CONFIGURATION_XSD}
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/${RTDB_CONFIGURATION_H} ${GENERATED_INC_DIR}/${RTDB_CONFIGURATION_H}
  DEPENDS ${RTDB_CONFIGURATION_XSD}
)

add_custom_target(rtdbconfig_xsd
  DEPENDS ${RTDB_CONFIGURATION_CPP}
)
