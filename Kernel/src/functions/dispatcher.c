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

#include "../libSockets/send.h"
#include "../libSockets/recv.h"

#include "../commons/structures.h"
#include "../commons/declarations.h"

t_program* planificar(){
	t_program* program = NULL;

	if(strcmp(configKernel->algoritmo, "FIFO") == 0){
		pthread_mutex_lock(&(queueReadyPrograms->mutex));
		if(list_size(queueReadyPrograms->list)>0){
			program = list_remove(queueReadyPrograms->list, 0);
		}
		pthread_mutex_unlock(&(queueReadyPrograms->mutex));
	}else if(strcmp(configKernel->algoritmo, "RR") == 0)
	{
		//Round robin
	}

	return program;
}
