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
#include "../../functions/program.h"


void select_program_socket_connection_lost(fd_set* master, int socket, int nbytes){
	program_interrup(socket, -6, 0);
	FD_CLR(socket, master);
}

void select_program_socket_recive_package(fd_set* master, int socket, int nbytes, char* package){
	if(strcmp(package, "NewProgram") == 0){
		program_process_new(master, socket);
	}else if(strcmp(package, "Finished") == 0){
		program_interrup(socket, -7, 0);
	}else{
		log_info(logKernel, "Error, mensaje no identificado: %s", package);
		printf("Error, mensaje no identificado: %s\n", package);
	}
}

void* select_program_thread_launcher(void* arg){
	socket_server_select(configKernel->puerto_prog, *select_program_socket_connection_lost, *select_program_socket_recive_package);
	return arg;
}
