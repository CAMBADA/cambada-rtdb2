cmake_minimum_required(VERSION 3.5.1)

# name of generated source file
set(RTDB_CONFIGURATION_XSD ${CMAKE_CURRENT_SOURCE_DIR}/rtdb_configuration.xsd)
set(RTDB_CONFIGURATION_H rtdb_configuration_generated.h)
set(RTDB_CONFIGURATION_CPP rtdb_configuration_generated.cpp)

set(SOURCE_FILES
  ${RTDB_CONFIGURATION_CPP}
)

add_library(rtdbconfig
  STATIC
  ${SOURCE_FILES}
)

add_library(RtDB::config ALIAS rtdbconfig)

set_target_properties(rtdbconfig PROPERTIES COMPILE_FLAGS "-fPIC")

add_dependencies(rtdbconfig
  rtdbconfig_xsd
)

add_custom_command(OUTPUT ${RTDB_CONFIGURATION_CPP}
  COMMAND xsdcxx cxx-tree
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

