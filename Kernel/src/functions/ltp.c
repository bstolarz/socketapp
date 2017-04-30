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

#include "../functions/memory.h"
#include "../functions/program.h"

int planificador_largo_plazo(){

	pthread_mutex_lock(&queueNewPrograms->mutex);


	if(list_size(queueNewPrograms->list)>0){
		bool _filtrarCPUsOcupados(t_cpu* cpu){
			return cpu->program != NULL;
		}

		int cantCorriendo = list_size(queueReadyPrograms->list) + list_size(list_filter(queueCPUs->list, (void*)_filtrarCPUsOcupados));

		while(cantCorriendo < configKernel->grado_multiprog && list_size(queueNewPrograms->list)>0){
			t_program* program = (t_program*)list_remove(queueNewPrograms->list, 0);
			int programStatus;
			if((programStatus = program_to_ready(program)) != 0){
				program->pcb->exitCode = programStatus;
				program_finish(program);
			}else{
				pthread_mutex_lock(&queueReadyPrograms->mutex);
				list_add(queueReadyPrograms->list, program);
				printf("El programa %i se agrego a la lista de ready\n", program->pcb->pid);
				pthread_mutex_unlock(&queueReadyPrograms->mutex);

				cantCorriendo++;
			}
		}
	}

	pthread_mutex_unlock(&queueNewPrograms->mutex);

	return 0;
}
