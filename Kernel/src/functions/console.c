#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <stdbool.h>

#include "../libSockets/server.h"
#include "../libSockets/recv.h"
#include "../libSockets/send.h"

#include "../commons/structures.h"
#include "../commons/declarations.h"

#include "../interface/memory.h"
#include "../planner/ltp.h"
#include "../functions/cpu.h"

void console_process_list(){
	void _printProccessList(t_program * program){
		printf("	%i\n", program->pcb->pid);
	}

	void _printProccessFinishList(t_program * program){
		printf("	%i - %i\n", program->pcb->pid, program->pcb->exitCode);
	}

	void _printProccessExecutingList(t_cpu * cpu){
		if(cpu->program != NULL){
			printf("	%i\n", cpu->program->pcb->pid);
		}
	}

	pthread_mutex_lock(&queueNewPrograms->mutex);
	printf("Lista de procesos nuevos\n");
	list_iterate(queueNewPrograms->list, (void*)_printProccessList);
	pthread_mutex_unlock(&queueNewPrograms->mutex);

	pthread_mutex_lock(&queueCPUs->mutex);
	printf("Lista de procesos ejecutando\n");
	list_iterate(queueCPUs->list, (void*)_printProccessExecutingList);
	pthread_mutex_unlock(&queueCPUs->mutex);

	pthread_mutex_lock(&queueReadyPrograms->mutex);
	printf("Lista de procesos listos\n");
	list_iterate(queueReadyPrograms->list, (void*)_printProccessList);
	pthread_mutex_unlock(&queueReadyPrograms->mutex);

	pthread_mutex_lock(&queueBlockedPrograms->mutex);
	printf("Lista de procesos bloqueados\n");
	list_iterate(queueBlockedPrograms->list, (void*)_printProccessList);
	pthread_mutex_unlock(&queueBlockedPrograms->mutex);

	pthread_mutex_lock(&queueFinishedPrograms->mutex);
	printf("Lista de procesos finalizados\n");
	list_iterate(queueFinishedPrograms->list, (void*)_printProccessFinishList);
	pthread_mutex_unlock(&queueFinishedPrograms->mutex);
}

void console_get_process_stats(){
	//TODO
}

void console_get_global_file_table(){
	//TODO
}

void console_multiprogram_degree(){
	//TODO
}

void console_finish_process(){
	//TODO
}

void console_stop_planning(){
	//TODO
}
