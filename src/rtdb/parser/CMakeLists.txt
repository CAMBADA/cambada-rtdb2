# src/libs/rtdb/parser

find_package(BISON)
find_package(FLEX)

# Define where is the rtdb_user.h
SET( RTDB_USER_H_FILE ${CMAKE_CURRENT_SOURCE_DIR}/../rtdb_user.h )

# For XRTDB CFLAGS: Include all directories inside source dir AND in include_directories
SET( RTDB_INCLUDE_DIRS )
GET_PROPERTY(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
STRING(LENGTH "${CMAKE_SOURCE_DIR}" srcdir_len)	# get cmake source dir string length
FOREACH(dir ${dirs})
	STRING( LENGTH "${dir}" dir_len)
	IF( ${dir_len} GREATER ${srcdir_len} OR ${dir_len} EQUAL ${srcdir_len} )
		STRING(SUBSTRING ${dir} 0 ${srcdir_len} cmpare_str)
		IF( ${cmpare_str} STREQUAL ${CMAKE_SOURCE_DIR} )
			SET( RTDB_INCLUDE_DIRS "${RTDB_INCLUDE_DIRS} -I${dir}")
		ENDIF()
	ENDIF()
ENDFOREACH()
#MESSAGE( STATUS "out='${RTDB_INCLUDE_DIRS}'")

# Add Eigen3
SET( RTDB_INCLUDE_DIRS "${RTDB_INCLUDE_DIRS} -I${EIGEN3_INCLUDE_DIR}")
CONFIGURE_FILE(
	${CMAKE_CURRENT_SOURCE_DIR}/rtdb_configuration.h.in
	${CMAKE_CURRENT_BINARY_DIR}/rtdb_configuration.h
)

SET( rtdbparser_SRC
	rtdb_errors.c
	rtdb_functions.c
	rtdb_user_creator.c
	rtdb_ini_creator.c
)

BISON_TARGET(xrtdbParser xrtdb.y ${CMAKE_CURRENT_BINARY_DIR}/xrtdb.tab.c )
FLEX_TARGET(xrtdbScanner xrtdb.lex ${CMAKE_CURRENT_BINARY_DIR}/xrtdb.c)
ADD_FLEX_BISON_DEPENDENCY(xrtdbScanner xrtdbParser)


include_directories( ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

ADD_EXECUTABLE ( xrtdb ${BISON_xrtdbParser_OUTPUTS} ${FLEX_xrtdbScanner_OUTPUTS} ${rtdbparser_SRC} )
TARGET_LINK_LIBRARIES(xrtdb
		xerces-c)

