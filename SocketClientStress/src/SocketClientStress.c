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
#include "sockets/client.h"
#include "sockets/send.h"

int main(void) {
	int clientSocket;
	char* ip = "127.0.0.1";
	char* port="6667";
	socket_client_create(&clientSocket, ip, port);

	char* mensaje = "Primer mensaje por sockets";

	int* len = 22;
	send(clientSocket, mensaje, 22, 0);
}
