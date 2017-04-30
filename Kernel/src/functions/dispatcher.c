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
	//TODO
	// Devolver primer item de la lista queue queueReadyPrograms
	// muteando la queue

	t_program* nextProgram;

	if (configKernel->algoritmo == "FIFO"){
		pthread_mutex_lock(&(queueReadyPrograms->mutex));
		nextProgram = list_get(&(queueReadyPrograms->list), 0);
		pthread_mutex_unlock(&(queueReadyPrograms->mutex));
	}else
	{
		//Round robin
	}

	return nextProgram;
}
