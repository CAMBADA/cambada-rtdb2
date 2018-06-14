#ifndef MULTICAST_H_
#define MULTICAST_H_

#include "NetworkConfig.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>

// These configurations are now on
// config/comm_network.conf
//#define MULTICAST_IP	"224.16.32.39"
//#define MULTICAST_PORT	2000

#define TTL				64

#define RECEIVE_OUR_DATA 0

typedef struct multiSocket_tag{
	struct sockaddr_in  destAddress;
	int					socketID;
	bool 				compressedData;
}multiSocket_t;

//	*************************
//  Open Socket
//
//	Input:
//		const char* = interface name {eth0, wlan0, ...}
//	Output:
//		multiSocket_t* multiSocket = socket descriptor
//
int openSocket(multiSocket_t* multiSocket, NetworkConfig* config);



//	*************************
//  Close Socket
//
//  Input:
//		multiSocket_t* multiSocket = socket descriptor
//
void closeSocket(multiSocket_t* multiSocket);



//	*************************
//  Send Data
//
//  Input:
//		multiSocket_t* multiSocket = socket descriptor
//		void* data = pointer to buffer with data
//		int dataSize = number of data bytes in buffer
//
int sendData(multiSocket_t* multiSocket, void* data, int dataSize);



//	*************************
//  Receive Data
//
//  Input:
//		multiSocket_t* multiSocket = socket descriptor
//		void* buffer = pointer to buffer
//		int bufferSize = total size of buffer
//
int receiveData(multiSocket_t* multiSocket, void* buffer, int bufferSize, std::string* src_ip = NULL);

#endif
