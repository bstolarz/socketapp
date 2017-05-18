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
#include "../../functions/serialization.h"


void cpu_send_pcb(t_cpu* cpu){
	log_info(logKernel,"socket is: %d\n",cpu->socket);
	socket_send_string(cpu->socket,"PCB");
	log_info(logKernel,"Envio la PCB del programa con PID: %d\n",cpu->program->pcb->pid);

	t_dataBlob serializedPcb = pcb_serialize(cpu->program->pcb);
	int sentSize = socket_send(cpu->socket, serializedPcb.data, serializedPcb.size);

	if (sentSize == -1){
		log_error(logKernel, "no pude mandar el pcb del proceso: %d\n",cpu->program->pcb->pid);
	}

	free(serializedPcb.data);
}

t_pcb* cpu_recv_pcb(t_cpu* cpu){
	char* message;

	log_debug(logKernel, "[recv PCB] esperando ...");
	socket_recv_string(cpu->socket, &message);

	if (string_equals_ignore_case("PCB", message))
	{
		log_debug(logKernel, "[recv PCB] llego mensaje PCB ...");

		t_dataBlob serializedPcb;
		int recvSize = socket_recv(cpu->socket, (void*)&serializedPcb.data, 1);

		if (recvSize == -1)
		{
			log_error(logKernel, "[PCB recv] obtuve size del pcb = -1");
			return NULL;
		}
		else
		{
			log_debug(logKernel, "[recv PCB] llego bien el PCB");
			serializedPcb.size = recvSize;
			return pcb_deserialize(serializedPcb);
		}
	}

	log_error(logKernel, "[PCB recv] nunca llego el mensaje PCB");
	return NULL;
}

void cpu_destroy_pcb(t_cpu* cpu){
	//TODO
}

t_cpu* cpu_find(int socket){
	bool _buscarProgramaSocketInCPUs(t_cpu* cpu){
		return cpu->socket==socket;
	}

	return list_find(queueCPUs->list, (void*)_buscarProgramaSocketInCPUs);
}

void cpu_process_new(int socket){
	t_cpu* cpu = malloc(sizeof(t_cpu));
	cpu->socket = socket;
	list_add(queueCPUs->list,cpu);
	log_info(logKernel,"New CPU added to list\n");
	if(list_size(queueReadyPrograms->list)>0){
		cpu->program = planificar();
		if(cpu->program != NULL){
			cpu_send_pcb(cpu);
		}
	}else{
		cpu->program=NULL;
	}
	//TODO send quantum
}

void cpu_interruption(t_cpu * cpu){
	if(socket_send_int(cpu->socket, cpu->program->interruptionCode)<=0){
		exit(EXIT_FAILURE);
	}
}

void cpu_still_burst(t_cpu* cpu){
	int burst = 1;
	if(socket_send_int(cpu->socket, burst)<=0){
		exit(EXIT_FAILURE);
	}
}

void cpu_end_burst(t_cpu* cpu){
	t_program* program = cpu->program;
	program->pcb = cpu_recv_pcb(cpu);

	int termino = 0;
	if(socket_recv_int(cpu->socket, &termino)<=0){
		//TODO Eliminar cpu de la lista de cpus
		exit(EXIT_FAILURE);
	}

	if(termino == 1){
		program_finish(program);
	}else{
		if(program->waiting == 1){
			pthread_mutex_lock(&queueBlockedPrograms->mutex);
			list_add(queueBlockedPrograms->list, program);
			pthread_mutex_unlock(&queueBlockedPrograms->mutex);
		}else{
			pthread_mutex_lock(&queueReadyPrograms->mutex);
			list_add(queueReadyPrograms->list, program);
			pthread_mutex_unlock(&queueReadyPrograms->mutex);
		}

	}

	cpu->program = planificar();
	if(cpu->program != NULL){
		cpu_send_pcb(cpu);
	}
}

