#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>
#include <commons/collections/list.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/log.h>
#include "commons/declarations.h"
#include "libSockets/send.h"
#include "libSockets/client.h"
#include "commons/structures.h"
#include "threads/program/select.h"
#include "threads/cpu/select.h"
#include "functions/config.h"
#include "functions/log.h"

int main(int argc, char* argv[]) {

	if(argc!=2){
		printf("Missing config path\n");
		return -1;
	}

	configKernel=malloc(sizeof(t_kernel));
	config_read(argv[1]);
	//config_read("/home/utnso/git/tp-2017-1c-SocketApp/kernel");

	logKernel=log_create_file();
	log_config();

	programID = 0;

	pthread_mutex_init(&(memoryServer.mutex),NULL);
	socket_client_create(&memoryServer.socket, configKernel->ip_memoria, configKernel->puerto_memoria);

	//Inicio lista nueva
	queueNewPrograms = malloc(sizeof(t_queue));
	queueNewPrograms->list = list_create();
	pthread_mutex_init(&(queueNewPrograms->mutex),NULL);

	//Inicio lista listos
	queueReadyPrograms = malloc(sizeof(t_queue));
	queueReadyPrograms->list = list_create();
	pthread_mutex_init(&(queueReadyPrograms->mutex),NULL);

	//Inicio lista terminados
	queueFinishedpPrograms = malloc(sizeof(t_queue));
	queueFinishedpPrograms->list = list_create();
	pthread_mutex_init(&(queueFinishedpPrograms->mutex),NULL);

	//Inicio lista cpus
	queueCPUs = malloc(sizeof(t_queue));
	queueCPUs->list = list_create();
	pthread_mutex_init(&(queueCPUs->mutex),NULL);


	pthread_create(&selectProgramThread,NULL,select_program_thread_launcher, NULL);
	pthread_create(&selectCPUThread,NULL,select_cpu_thread_launcher, NULL);

	pthread_join(selectProgramThread, NULL);
	pthread_join(selectCPUThread, NULL);
	return EXIT_SUCCESS;
}
