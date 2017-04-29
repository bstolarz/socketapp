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

void planificadorLargoPlazo(){

	pthread_mutex_lock(&queueNewPrograms->mutex);
	pthread_mutex_lock(&queueReadyPrograms->mutex);
	pthread_mutex_lock(&queueCPUs->mutex);

	if(list_size(queueNewPrograms->list)>0){
		bool _filtrarCPUsOcupados(t_cpu* cpu){
			return cpu->program != NULL;
		}

		int cantCorriendo = list_size(queueReadyPrograms->list) + list_size(list_filter(queueCPUs->list, (void*)_filtrarCPUsOcupados));
		printf("Cantidad de programas corriendo: %i\n", cantCorriendo);
		if(cantCorriendo < configKernel->grado_multiprog){
			t_program* program = (t_program*)list_remove(queueNewPrograms->list, 0);
			int frameSize = 0;

			pthread_mutex_lock(&memoryServer.mutex);

			socket_send_string(memoryServer.socket, "frame_size");
			socket_recv_int(memoryServer.socket, &frameSize);
			printf("el FrameSize es:%i\n", frameSize);
			//TODO

			pthread_mutex_unlock(&memoryServer.mutex);

		}
	}

	pthread_mutex_lock(&queueCPUs->mutex);
	pthread_mutex_lock(&queueReadyPrograms->mutex);
	pthread_mutex_lock(&queueNewPrograms->mutex);
}
