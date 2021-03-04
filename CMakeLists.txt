CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT( CambadaRtDB2 )

SET( V_MAJOR 2 )
SET( V_MINOR 0 )
SET( V_PATCH 0 )

# http://www.cmake.org/cmake/help/cmake-2-8-docs.html#section_Policies
IF(COMMAND cmake_policy)
  	cmake_policy(SET CMP0003 NEW)
ENDIF(COMMAND cmake_policy)

IF(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
   SET(CMAKE_BUILD_TYPE Release)
ENDIF(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -ftemplate-depth=1200")

SET( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -W" )
SET( CMAKE_C_FLAGS_RELEASE "-O3" )
SET( CMAKE_C_FLAGS_DEBUG "-g3" )

SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall" )
SET( CMAKE_CXX_FLAGS_RELEASE "-O3" )
SET( CMAKE_CXX_FLAGS_DEBUG "-g3" )

# Set the top directory of the source code (the first CMakeLists of the project, this one actually)
SET( BASE_DIR ${CMAKE_SOURCE_DIR} )
SET( CAMBADA_CONFIG_DIR ${BASE_DIR}/config )
SET( CAMBADA_SRC_DIR ${BASE_DIR}/src )

# Set output directories for libraries and executables
SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BASE_DIR}/lib )
SET( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BASE_DIR}/lib )
SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BASE_DIR}/bin )

LINK_DIRECTORIES( ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} )

# Define where to find modules
SET( CMAKE_MODULE_PATH ${CMAKE_MODULES_PATH} ${BASE_DIR}/cmake )

# Add sources directory
ADD_SUBDIRECTORY( ${CAMBADA_SRC_DIR} )

# ---
MESSAGE ("\n==> Building Options")
MESSAGE (STATUS "Building as [${CMAKE_BUILD_TYPE}]")
MESSAGE ("\n==> Building Instructions")
MESSAGE (STATUS "Compile CAMBADA RtDB2 project using: make")
MESSAGE ("\n")

