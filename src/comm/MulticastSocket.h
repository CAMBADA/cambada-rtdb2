#ifndef _CAMBADA_MULTICAST_COMM_
#define _CAMBADA_MULTICAST_COMM_

#define DFL_MULTICAST_PORT 2000
#define DFL_MULTICAST_GROUP "224.16.32.39"

#define RECEIVE_OUR_DATA 1

#include <arpa/inet.h>

class MulticastSocket
{
public:
	MulticastSocket();
	~MulticastSocket();

	int openSocket(const char* interface, const char* group = DFL_MULTICAST_GROUP, short port = DFL_MULTICAST_PORT);
	int closeSocket();
	int sendData(void* data, int dataSize);
	int receiveData(void* buffer, int bufferSize);

	inline int getSocket() 
	{
		return multiSocket;
	}

private:
	const char* multicast_group;
	short multicast_port;

	int multiSocket;

	struct sockaddr_in destAddress;
	
	int if_NameToIndex(const char *ifname, char *address);
};

#endif
