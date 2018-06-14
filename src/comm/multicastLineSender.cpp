#include "MulticastSocket.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
	if (argc != 5)
	{
		fprintf(stderr, "USAGE: multicastReceiver interface multicast-ip port file-to-send\n");
		exit(EXIT_FAILURE);
	}

	const char* interface = argv[1];
	const char* ip = argv[2];
	short port = short(atoi(argv[3]));
	const char* fname = argv[4];

	/* open communication socket */
	MulticastSocket sock;
	assert(sock.openSocket(interface, ip, port) != -1);

	/* open file to be sent */
	FILE *fin;
	assert((fin = fopen(fname, "r")) != NULL);

	char buf[1500];
	while (fgets(buf, 1500, fin) != NULL)
	{
		printf("«««««\n");
		printf("%s", buf);
		printf("»»»»»\n");
		sock.sendData(buf, strlen(buf));
		usleep(500 * 1000);
	}

	fclose(fin);
	sock.closeSocket();
}
