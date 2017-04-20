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
	log_info(logKernel,"socket is: %d\n",cpu->socket);
	socket_send_string(cpu->socket,"PCB");
	log_info(logKernel,"Envio la PCB del programa con PID: %d\n",cpu->program->pcb->pid);

	if(socket_send_int(cpu->socket,cpu->program->pcb->pid)>0){
		log_info(logKernel,"Envio el PID del programa: %d\n",cpu->program->pcb->pid);
	}else{
		log_info(logKernel,"Error enviando el PID del programa con PID: %d\n",cpu->program->pcb->pid);
	}

	if(socket_send_int(cpu->socket,cpu->program->pcb->pc)>0){
		log_info(logKernel,"Envio el PC: %d del programa: %d\n",cpu->program->pcb->pc,cpu->program->pcb->pid);
	}else{
		log_info(logKernel,"Error enviando el PC %d del programa con PID: %d\n",cpu->program->pcb->pc, cpu->program->pcb->pid);
	}

	if(socket_send_int(cpu->socket,cpu->program->pcb->cantPagsCodigo)>0){
		log_info(logKernel,"Envio paginas de codigo: %d del programa: %d\n",cpu->program->pcb->cantPagsCodigo,cpu->program->pcb->pid);
	}else{
		log_info(logKernel,"Error enviando paginas de codigo: %d del programa con PID: %d\n",cpu->program->pcb->cantPagsCodigo, cpu->program->pcb->pid);
	}

	if(socket_send_int(cpu->socket,cpu->program->pcb->indiceDeCodigo->offset_inicio)>0){
		log_info(logKernel,"Envio offset de inicio de indice de codigo: %d del programa: %d\n",cpu->program->pcb->indiceDeCodigo->offset_inicio,cpu->program->pcb->pid);
	}else{
		log_info(logKernel,"Error enviando offset de inicio de indice de codigo: %d del programa con PID: %d\n",cpu->program->pcb->indiceDeCodigo->offset_inicio, cpu->program->pcb->pid);
	}

	if(socket_send_int(cpu->socket,cpu->program->pcb->stackPosition)>0){
		log_info(logKernel,"Envio stack position: %d del programa: %d\n",cpu->program->pcb->stackPosition,cpu->program->pcb->pid);
	}else{
		log_info(logKernel,"Error enviando paginas de codigo: %d del programa con PID: %d\n",cpu->program->pcb->stackPosition, cpu->program->pcb->pid);
	}
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
	list_add(queueCPUs->list,cpu);
	log_info(logKernel,"New CPU added to list\n");
	if(list_size(queueReadyPrograms->list)>0){
		cpu->program = planificar();
			cpu_send_pcb(cpu);

	}else{

	}
	//TODO send quantum
}

void cpu_process_finished_quantum(int socket){
	t_cpu* cpu = cpu_find(socket);
	cpu_recv_pcb(cpu);
	//TODO mover a lista correspondiente el programa
	cpu->program = planificar();
	cpu_send_pcb(cpu);
}


