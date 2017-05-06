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
#include "handler.h"

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
	printf("%s\n", package);
	if(strcmp(package, "NewCPU") == 0){
		log_info(logKernel,"New CPU connected on socket %d\n",socket);
		printf("New CPU connected on socket %d\n",socket);
		cpu_process_new(socket);
	}else if(strcmp(package, "interruption") == 0){
		t_cpu* cpu = cpu_find(socket);
		cpu_interruption(cpu);
	}else if(strcmp(package, "still_burst") == 0){
		t_cpu* cpu = cpu_find(socket);
		cpu_still_burst(cpu);
	}else if(strcmp(package, "end_burst") == 0){
		t_cpu* cpu = cpu_find(socket);
		cpu_end_burst(cpu);
	}else if (strcmp(package, "getSharedVariable")==0){
		t_cpu* cpu = cpu_find(socket);
		handle_cpu_get_shared_variable(cpu);
	}else if (strcmp(package, "setSharedVariable")==0){
		t_cpu* cpu = cpu_find(socket);
		handle_cpu_set_shared_variable(cpu);
	}else if (strcmp(package, "finish")==0){
		t_cpu* cpu = cpu_find(socket);
		handle_cpu_finish(cpu);
	}else if (strcmp(package, "wait")==0){
		t_cpu* cpu = cpu_find(socket);
		handle_cpu_wait(cpu);
	}else if (strcmp(package, "signal")==0){
		t_cpu* cpu = cpu_find(socket);
		handle_cpu_signal(cpu);
	}else if (strcmp(package, "alocar")==0){
		t_cpu* cpu = cpu_find(socket);
		handle_cpu_allocar(cpu);
	}else if (strcmp(package, "liberar")==0){
		t_cpu* cpu = cpu_find(socket);
		handle_cpu_liberar(cpu);
	}else if (strcmp(package, "abrir")==0){
		t_cpu* cpu = cpu_find(socket);
		handle_cpu_abrir(cpu);
	}else if (strcmp(package, "borrar")==0){
		t_cpu* cpu = cpu_find(socket);
		handle_cpu_borrar(cpu);
	}else if (strcmp(package, "cerrar")==0){
		t_cpu* cpu = cpu_find(socket);
		handle_cpu_cerrar(cpu);
	}else if (strcmp(package, "moverCursor")==0){
		t_cpu* cpu = cpu_find(socket);
		handle_cpu_mover_cursor(cpu);
	}else if (strcmp(package, "escribir")==0){
		t_cpu* cpu = cpu_find(socket);
		handle_cpu_escribir(cpu);
	}else if (strcmp(package, "leer")==0){
		t_cpu* cpu = cpu_find(socket);
		handle_cpu_leer(cpu);
	}else{
		log_info(logKernel, "Error, mensaje no identificado: %s", package);
		printf("Error, mensaje no identificado: %s\n", package);
	}
}

void* select_cpu_thread_launcher(void* arg){
	socket_server_select(configKernel->puerto_cpu, *select_cpu_socket_connection_lost, *select_cpu_socket_recive_package);
	return arg;
}
