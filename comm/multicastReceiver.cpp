#include "MulticastSocket.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
	if (argc != 4)
	{
		fprintf(stderr, "USAGE: interface multicastReceiver multicast_ip port\n");
		exit(EXIT_FAILURE);
	}

	const char* interface = argv[1];
	const char* group = argv[2];
	short port = short(atoi(argv[3]));

	MulticastSocket sock;
	assert(sock.openSocket(interface, group, port) != -1);

	char buf[1501];
	int n;
	while (1)
	{
		//memset(buf, 'a', 1500);
		printf("Vou ler\n");
		n = sock.receiveData(buf, 1500);
		buf[n] = '\0';
		printf("«««««««««««««««\n");
		printf("%s\n", buf);
		printf("»»»»»»»»»»»»»»»\n");
		fflush(stdout);
	}

	sock.closeSocket();
}
