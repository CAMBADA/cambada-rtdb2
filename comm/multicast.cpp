#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <unistd.h>
#include <linux/if_ether.h>

#include "comm.h"
#include "multicast.h"
#include "zlib.h"

#define PERRNO(txt) \
	printf("ERROR: (%s / %s): " txt ": %s\n", __FILE__, __FUNCTION__, strerror(errno))

#define PERR(txt, par...) \
	printf("ERROR: (%s / %s): " txt "\n", __FILE__, __FUNCTION__, ## par)

#ifdef DEBUG
#define PDEBUG(txt, par...) \
	printf("DEBUG: (%s / %s): " txt "\n", __FILE__, __FUNCTION__, ## par)
#else
#define PDEBUG(txt, par...)
#endif

int if_NameToIndex(char *ifname, char *address)
{
	int	fd;
	struct ifreq if_info;
	int if_index;

	memset(&if_info, 0, sizeof(if_info));
	strncpy(if_info.ifr_name, ifname, IFNAMSIZ-1);

	if ((fd=socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		PERRNO("socket");
		return -1;
	}
	if (ioctl(fd, SIOCGIFINDEX, &if_info) == -1)
	{
		PERRNO("ioctl");
		close(fd);
		return -1;
	}
	if_index = if_info.ifr_ifindex;

	if (ioctl(fd, SIOCGIFADDR, &if_info) == -1)
	{
		PERRNO("ioctl");
		close(fd);
		return -1;
	}
	
	close(fd);

	sprintf(address, "%d.%d.%d.%d\n",
		(int) ((unsigned char *) if_info.ifr_hwaddr.sa_data)[2],
		(int) ((unsigned char *) if_info.ifr_hwaddr.sa_data)[3],
		(int) ((unsigned char *) if_info.ifr_hwaddr.sa_data)[4],
		(int) ((unsigned char *) if_info.ifr_hwaddr.sa_data)[5]);

	printf("**** Using device %s -> Ethernet %s\n ****", if_info.ifr_name, address);

	return if_index;
}


//	*************************
//  Open Socket
//
int openSocket(multiSocket_t* multiSocket, NetworkConfig* nw_config)
{
	struct sockaddr_in multicastAddress;
	struct ip_mreqn mreqn;
	struct ip_mreq mreq;
	int opt;
	char address[16]; //IPV4: xxx.xxx.xxx.xxx\0

	bzero(&multicastAddress, sizeof(struct sockaddr_in));
	multicastAddress.sin_family = AF_INET;
	multicastAddress.sin_port = htons(nw_config->port);
	multicastAddress.sin_addr.s_addr = INADDR_ANY;

	bzero(&multiSocket->destAddress, sizeof(struct sockaddr_in));
	multiSocket->destAddress.sin_family = AF_INET;
	multiSocket->destAddress.sin_port = htons(nw_config->port);
	multiSocket->destAddress.sin_addr.s_addr = inet_addr(nw_config->multicast_ip);

	if((multiSocket->socketID = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
	{
		PERRNO("socket");
		return -1;
	}

	memset((void *) &mreqn, 0, sizeof(mreqn));
	mreqn.imr_ifindex=if_NameToIndex(nw_config->iface, address);
	if((setsockopt(multiSocket->socketID, SOL_IP, IP_MULTICAST_IF, &mreqn, sizeof(mreqn))) == -1)
	{
		PERRNO("setsockopt 1");
		return -1;
	}

	opt = 1;
	if((setsockopt(multiSocket->socketID, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) == -1)
	{
		PERRNO("setsockopt 2");
		return -1;
	}

	memset((void *) &mreq, 0, sizeof(mreq));
	mreq.imr_multiaddr.s_addr = inet_addr(nw_config->multicast_ip);
	mreq.imr_interface.s_addr = inet_addr(address);
	//fprintf(stderr, "Index: %d (port %d, %s / %s)\n", multiSocket->socketID, nw_config->port, nw_config->multicast_ip, address);

	if((setsockopt(multiSocket->socketID, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq))) == -1)
	{
		PERRNO("setsockopt 3");
		return -1;
	}

	/* Disable reception of our own multicast */
	opt = RECEIVE_OUR_DATA;
	if((setsockopt(multiSocket->socketID, IPPROTO_IP, IP_MULTICAST_LOOP, &opt, sizeof(opt))) == -1)
	{
		PERRNO("setsockopt");
		return -1;
	}

	if(bind(multiSocket->socketID, (struct sockaddr *) &multicastAddress, sizeof(struct sockaddr_in)) == -1)
	{
		PERRNO("bind");
		return -1;
	}

	return 0;
}


//	*************************
//  Close Socket
//
void closeSocket(multiSocket_t* multiSocket)
{
	if(multiSocket->socketID != -1)
		shutdown(multiSocket->socketID, SHUT_RDWR);
}



//	*************************
//  Send Data
//
int sendData(multiSocket_t* multiSocket, void* data, int dataSize)
{
	if(multiSocket->compressedData)
	{
		char cdata[(int) (1.1*BUFFER_SIZE+12)];
		unsigned long cdataSize;
		char totdata[BUFFER_SIZE+sizeof(unsigned short)]; // including 2 bytes for original data size
		unsigned short norg;
		int nsent, totdataSize;

		//	original data size (assume 2 bytes)
		norg=(unsigned int) dataSize;

#ifdef MULTICAST_DEBUG
		printf("sendData: original data size = %d\n", norg);
#endif

		//	compress data
		cdataSize=1.1*BUFFER_SIZE+12;
		compress((Bytef*) cdata, &cdataSize, (Bytef*)data, (unsigned long) dataSize);

#ifdef MULTICAST_DEBUG
		printf("   compressed data size = %d\n", cdataSize);
#endif

		//	total data = original data size + compressed data
		memcpy(totdata, &norg, sizeof(unsigned short));
		memcpy(totdata+sizeof(unsigned short), cdata, cdataSize);

		//	send total data
		totdataSize=cdataSize+sizeof(unsigned short);

#ifdef MULTICAST_DEBUG
		printf("   totdataSize = %d\n", totdataSize);
#endif

		nsent=sendto(multiSocket->socketID, totdata, totdataSize, 0, (struct sockaddr *)&multiSocket->destAddress, sizeof (struct sockaddr));

		if ( nsent==totdataSize ) {
#ifdef MULTICAST_DEBUG
			printf("  nsent = %d bytes\n", nsent);
#endif
			return norg;
		}
	}else{ // Do not compress
		return sendto(multiSocket->socketID, data, dataSize, 0, (struct sockaddr *)&multiSocket->destAddress, sizeof (struct sockaddr));
	}
	return 0;
}



//	*************************
//  Receive Data
//
int receiveData(multiSocket_t* multiSocket, void* buffer, int bufferSize, std::string* src_ip) {
	struct sockaddr src_addr;
	socklen_t addr_len;

	int code;
	if(multiSocket->compressedData)
	{
		int totdataSize, nrecv;
		char totdata[BUFFER_SIZE+sizeof(unsigned short)];
		unsigned short norg;
		char cdata[BUFFER_SIZE];
		unsigned long cdataSize;
		unsigned long dataSize;

		//	read total data
		totdataSize=BUFFER_SIZE+sizeof(unsigned short);
		nrecv=recvfrom(multiSocket->socketID, totdata, totdataSize, 0, &src_addr, &addr_len);
		if ( nrecv==0 ) { return 0; }

#ifdef MULTICAST_DEBUG
		printf("receiveData: nrecv = %d bytes\n", nrecv);
#endif

		//	total data = original data size + compressed data
		cdataSize=nrecv-sizeof(unsigned short);

#ifdef MULTICAST_DEBUG
		printf("   compressed data size = %d\n", cdataSize);
#endif

		memcpy(&norg, totdata, sizeof(unsigned short));
		memcpy(cdata, totdata+sizeof(unsigned short), cdataSize);

		//	uncompress data
		if ( norg>bufferSize ) { norg=bufferSize; }

#ifdef MULTICAST_DEBUG
		printf("   original data size = %d\n", norg);
#endif

		dataSize=(unsigned long) norg;
		uncompress((Bytef*)buffer, &dataSize, (Bytef*)cdata, cdataSize);

		code = norg;
	}else{ // Do not uncompress
		code =  recvfrom(multiSocket->socketID, buffer, bufferSize, 0, &src_addr, &addr_len);
	}

	char * data = inet_ntoa(((struct sockaddr_in*) &src_addr)->sin_addr);
	if (src_ip != NULL) {
		src_ip->assign(data);
	}
	return code;
}
