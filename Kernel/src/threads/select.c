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
#include "../functions/handShakeWithCPU.h"
#include "../commons/declarations.h"
#include "../libSockets/server.h"
#include "../libSockets/recv.h"
#include "../libSockets/send.h"
#include "../functions/userInterface.h"

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

		int tamanioProgramaAnsisop;
		socket_recv_int(socket,&tamanioProgramaAnsisop);
		//printf("Tamanio programa: %d bytes\n",tamanioProgramaAnsisop);
		char* programa=malloc(tamanioProgramaAnsisop*sizeof(char));
		socket_recv_string(socket, &programa);

		//printf("Message from socket %d: %s\n", socket, programa);
		void _enviarMensaje(int* i){
			socket_send_string(*i, programa);
		}
		list_iterate(clientes, (void*)_enviarMensaje);
	}else if(package[0]=='M'&& package[1]=='E' && package[2]=='M'){
		printf("Memory connected on socket: %d\n",socket);
		memorySocket=socket;
	}else if(package[0]=='C'&& package[1]=='P' && package[2]=='U'){
		printf("CPU connected on socket %d\n",socket);
		agregarCPU(socket);
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
