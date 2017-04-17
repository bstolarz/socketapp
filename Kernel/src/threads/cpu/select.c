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

void select_cpu_socket_connection_lost(fd_set* master, int socket, int nbytes){
	if (nbytes == 0) {
		printf("selectserver: socket %d hung up\n", socket);
	} else {
		perror("recv");
	}
	close(socket);
	FD_CLR(socket, master); // eliminar del conjunto maestro
}

void select_cpu_socket_recive_package(fd_set* master, int socket, int nbytes, char* package){
	if(strcmp(package, "NewCPU") == 0){
		log_info(logKernel,"New CPU connected on socket %d\n",socket);
		cpu_process_new(socket);
	}else if(strcmp(package, "FinishedQuantum") == 0){
		cpu_process_finished_quantum(socket);
	}else{
		log_info(logKernel, "Error, mensaje no identificado: %s", package);
		printf("Error, mensaje no identificado: %s\n", package);
	}
}

void* select_cpu_thread_launcher(void* arg){
	socket_server_select(configKernel->puerto_cpu, *select_cpu_socket_connection_lost, *select_cpu_socket_recive_package);
	return arg;
}
