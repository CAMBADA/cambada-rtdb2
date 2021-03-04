FIND_PACKAGE(Boost COMPONENTS system filesystem REQUIRED)


ADD_LIBRARY( comm STATIC MulticastSocket.cpp )
SET_TARGET_PROPERTIES( comm PROPERTIES COMPILE_FLAGS "-fPIC" )


SET( comm_SRC
	multicast.cpp
	comm.cpp
	NetworkConfig.cpp
)

set ( comm_OBJ cambadaComm )
ADD_EXECUTABLE ( ${comm_OBJ} ${comm_SRC} )
TARGET_LINK_LIBRARIES( ${comm_OBJ} 
    rtdb 
    rtdb2 
    pthread 
    comm 
    z 
    ${Boost_FILESYSTEM_LIBRARY} 
    ${Boost_SYSTEM_LIBRARY}
)

SET_TARGET_PROPERTIES( ${comm_OBJ} PROPERTIES OUTPUT_NAME comm )


