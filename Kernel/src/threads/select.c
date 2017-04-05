/*
 * select.c
 *
 *  Created on: 2/4/2017
 *      Author: utnso
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
#include <commons/string.h>

#include "../commons/declarations.h"
#include "../libSockets/server.h"
#include "../libSockets/recv.h"
#include "../libSockets/send.h"

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
	if(package[0]=='C' && package[1]=='O' && package[2]=='N'){
		char* mensaje = string_new();
		socket_recv_string(socket, &mensaje);
		void _enviarMensaje(int* i){
			socket_send_string(*i, mensaje);
		}
		list_iterate(clientes, (void*)_enviarMensaje);
	}else{
		FD_CLR(socket, master);
		int* i = malloc(sizeof(int));
		*i = socket;
		list_add(clientes, i);
	}
}

void* selectThreadLauncher(void* arg){
	socket_server_select("6667", *socket_select_connection_lost, *socket_select_recive_package);
	return arg;
}
