#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>
#include <sys/socket.h>

#include <commons/collections/list.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/log.h>

#include "../../libSockets/send.h"
#include "../../libSockets/recv.h"

#include "../../commons/structures.h"
#include "../../commons/declarations.h"

#include "../../functions/dispatcher.h"

void cpu_send_pcb(t_cpu* cpu){
	//TODO
}

void cpu_recv_pcb(t_cpu* cpu){
	//TODO
}

t_cpu* cpu_find(int socket){
	bool _buscarProgramaSocketInCPUs(t_cpu* cpu){
		return cpu->socket==socket;
	}

	pthread_mutex_lock(&(queueCPUs->mutex));
	t_cpu* cpu = list_find(queueCPUs->list, (void*)_buscarProgramaSocketInCPUs);
	pthread_mutex_unlock(&(queueCPUs->mutex));
	return cpu;
}

void cpu_process_new(int socket){
	t_cpu* cpu = malloc(sizeof(t_cpu));
	cpu->socket = socket;

	cpu->program = planificar();
	cpu_send_pcb(cpu);
	//TODO send quantum
}

void cpu_process_finished_quantum(int socket){
	t_cpu* cpu = cpu_find(socket);
	cpu_recv_pcb(cpu);
	//TODO mover a lista correspondiente el programa
	cpu->program = planificar();
	cpu_send_pcb(cpu);
}


