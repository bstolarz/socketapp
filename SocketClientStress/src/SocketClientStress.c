/*
 ============================================================================
 Name        : SocketClientStress.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdint.h>
#include <pthread.h>
#include "libSockets/client.h"
#include "libSockets/send.h"


int main(void) {
	int clientSocket;
	char* ip = "127.0.0.1";
	char* port="6667";
	socket_client_create(&clientSocket, ip, port);

	socket_send(clientSocket, "Primer mensaje por sockets23", 27);
	socket_send_int(clientSocket, 23);
	socket_send_string(clientSocket, "Segundo mensaje por sockets");

	close(clientSocket);

	return 0;
}
