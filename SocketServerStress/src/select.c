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
#include "libSockets/recv.h"

void socket_select_connection_lost(fd_set* master, int socket, int nbytes){
	if (nbytes == 0) {
		printf("selectserver: socket %d hung up\n", socket);
	} else {
		perror("recv");
	}
	close(socket);
	FD_CLR(socket, master); // eliminar del conjunto maestro
}

void socket_select_recive_package(fd_set* master, int socket, int nbytes, char* package){
	printf("Llego %s\n", package);
	int value=0;
	socket_recv_int(socket, &value);
	printf("Llego %i\n", value);
	char* text="";
	socket_recv_string(socket, &text);
	printf("Llego %s\n", text);
}
