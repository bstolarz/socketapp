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

#include "../../libSockets/server.h"
#include "../../libSockets/recv.h"
#include "../../libSockets/send.h"

#include "../../commons/declarations.h"

#include "functions.h"

void select_program_socket_connection_lost(fd_set* master, int socket, int nbytes){
	if (nbytes == 0) {
		printf("selectserver: socket %d hung up\n", socket);
	} else {
		perror("recv");
	}
	close(socket);
	FD_CLR(socket, master); // eliminar del conjunto maestro
}

void select_program_socket_recive_package(fd_set* master, int socket, int nbytes, char* package){
	if(strcmp(package, "NewProgram") == 0){
		program_process_new(master, socket);
	}else{
		printf("Error, mensaje no identificado: %s\n", package);
	}
}

void* select_program_thread_launcher(void* arg){
	socket_server_select(configKernel->puerto_prog, *select_program_socket_connection_lost, *select_program_socket_recive_package);
	return arg;
}
