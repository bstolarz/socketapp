/*
 ============================================================================
 Name        : SocketClientStress.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "libSockets/client.h"
#include "libSockets/send.h"
#include <sys/socket.h>

int main(void) {
	int clientSocket;
	char* ip = "127.0.0.1";
	char* port="6667";
	socket_client_create(&clientSocket, ip, port);

	socket_send(clientSocket, "Primer mensaje por sockets", 27);
	socket_send(clientSocket, "Segundo mensaje por sockets", 28);
	close(clientSocket);

	return 0;
}
