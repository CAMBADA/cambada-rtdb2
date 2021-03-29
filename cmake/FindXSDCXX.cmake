# - Try to find XSDCXX
#
# This module defines
# XSDCXX_INCLUDE_DIR, where to find elements.hxx, etc.
# XSDCXX, xsdcxx executable
# XSDCXX_FOUND, If false, XSDCXX was not found.

find_path(XSDCXX_INCLUDE_DIR xsd/cxx/parser/elements.hxx
  PATHS $ENV{XSDDIR} /usr/local /usr
  PATH_SUFFIXES include libxsd xsd
  NO_CMAKE_FIND_ROOT_PATH
)

find_program(XSDCXX
  NAMES xsdcxx xsd
  PATHS $ENV{XSDDIR}/bin /usr/local/bin /usr/bin
  NO_SYSTEM_ENVIRONMENT_PATH
)

mark_as_advanced( XSDCXX_INCLUDE_DIR XSDCXX_EXECUTABLE )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(XSDCXX
  REQUIRED_VARS XSDCXX XSDCXX_INCLUDE_DIR
)

if(XSDCXX_FOUND)
  message(STATUS "Found XSDCXX (include: ${XSDCXX_INCLUDE_DIR}, executable: ${XSDCXX})")
endif()
