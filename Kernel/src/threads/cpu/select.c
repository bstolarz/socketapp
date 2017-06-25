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

#include "../../functions/cpu.h"

#include "../../commons/declarations.h"
#include "handler.h"

void select_cpu_socket_connection_lost(fd_set* master, int socket, int nbytes){
	pthread_mutex_lock(&queueCPUs->mutex);

	t_cpu* cpu = cpu_find(socket);

	bool _removeCPUBySocket(t_cpu* cpu){
		return cpu->socket==socket;
	}
	list_remove_by_condition(queueCPUs->list, (void*)_removeCPUBySocket);

	if(cpu->program != NULL){
		cpu->program->interruptionCode = -16;
		cpu->program->pcb->exitCode = -16;
		program_finish(cpu->program);
	}

	FD_CLR(cpu->socket, master);
	close(cpu->socket);
	free(cpu);

	pthread_mutex_unlock(&queueCPUs->mutex);
}

void select_cpu_socket_recive_package(fd_set* master, int socket, int nbytes, char* package){
	pthread_mutex_lock(&queueCPUs->mutex);

	log_info(logKernel,"[CPU] %s", package);
	if(strcmp(package, "NewCPU") == 0){
		handle_new_cpu(socket);
	}else if(strcmp(package, "interruption") == 0){
		t_cpu* cpu = cpu_find(socket);
		handle_interruption(cpu);
	}else if(strcmp(package, "still_burst") == 0){
		t_cpu* cpu = cpu_find(socket);
		handle_still_burst(cpu);
	}else if(strcmp(package, "end_burst") == 0){
		t_cpu* cpu = cpu_find(socket);
		handle_end_burst(cpu);
	}else if (strcmp(package, "getSharedVariable")==0){
		t_cpu* cpu = cpu_find(socket);
		handle_cpu_get_shared_variable(cpu);
	}else if (strcmp(package, "setSharedVariable")==0){
		t_cpu* cpu = cpu_find(socket);
		handle_cpu_set_shared_variable(cpu);
	}else if (strcmp(package, "wait")==0){
		t_cpu* cpu = cpu_find(socket);
		handle_cpu_wait(cpu);
	}else if (strcmp(package, "signal")==0){
		t_cpu* cpu = cpu_find(socket);
		handle_cpu_signal(cpu);
	}else if (strcmp(package, "alocar")==0){
		t_cpu* cpu = cpu_find(socket);
		handle_cpu_alocar(cpu);
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
		log_warning(logKernel, "[CPU] %s", package);
	}
	log_info(logKernel,"[CPU] fin %s", package);


	bool _findDisconnectedCPUs(t_cpu* cpu){
		return cpu->disconnected==1;
	}
	void _destroyDisconnectedCPU(t_cpu* cpu){
		cpu->program->interruptionCode = -16;
		cpu->program->pcb->exitCode = -16;
		program_finish(cpu->program);
		FD_CLR(cpu->socket, master);
		close(cpu->socket);
		free(cpu);
		log_info(logKernel, "Murio el CPU");
	}
	list_remove_and_destroy_by_condition(queueCPUs->list, (void*)_findDisconnectedCPUs,(void*)_destroyDisconnectedCPU);

	pthread_mutex_unlock(&queueCPUs->mutex);
}

void* select_cpu_thread_launcher(void* arg){
	socket_server_select(configKernel->puerto_cpu, *select_cpu_socket_connection_lost, *select_cpu_socket_recive_package);
	return arg;
}
