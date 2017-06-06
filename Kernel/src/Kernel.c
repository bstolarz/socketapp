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
#include "functions/console.h"
#include "functions/log.h"
#include "interface/memory.h"
#include "interface/filesystem.h"

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

	memory_connect();
	//filesystem_connect();

	FILE_DESCRIPTOR_PERMISSION_READ = "r";
	FILE_DESCRIPTOR_PERMISSION_WRITE = "w";
	FILE_DESCRIPTOR_PERMISSION_CREATE = "c";

	//Inicio lista nueva
	queueNewPrograms = malloc(sizeof(t_queue));
	queueNewPrograms->list = list_create();
	pthread_mutex_init(&(queueNewPrograms->mutex),NULL);

	//Inicio lista listos
	queueReadyPrograms = malloc(sizeof(t_queue));
	queueReadyPrograms->list = list_create();
	pthread_mutex_init(&(queueReadyPrograms->mutex),NULL);

	//Inicio lista bloqueados
	queueBlockedPrograms = malloc(sizeof(t_queue));
	queueBlockedPrograms->list = list_create();
	pthread_mutex_init(&(queueBlockedPrograms->mutex),NULL);

	//Inicio lista terminados
	queueFinishedPrograms = malloc(sizeof(t_queue));
	queueFinishedPrograms->list = list_create();
	pthread_mutex_init(&(queueFinishedPrograms->mutex),NULL);

	//Inicio lista cpus
	queueCPUs = malloc(sizeof(t_queue));
	queueCPUs->list = list_create();
	pthread_mutex_init(&(queueCPUs->mutex),NULL);

	//Inicio lista FD global
	globalFileDescriptors = malloc(sizeof(t_queue));
	globalFileDescriptors->list = list_create();
	pthread_mutex_init(&(globalFileDescriptors->mutex),NULL);

	pthread_create(&selectProgramThread,NULL,select_program_thread_launcher, NULL);
	pthread_create(&selectCPUThread,NULL,select_cpu_thread_launcher, NULL);

	size_t cantidad = 50;
	char* comando = malloc(sizeof(char)*cantidad);

	while(1){
		printf("[SISTEMA] - Ingrese un comando:\n");
		size_t cantLeida = getline(&comando, &cantidad, stdin);
		comando[cantLeida-1]='\0';

		if(strcmp(comando, "1") == 0){
			console_process_list();
		}else if(strcmp(comando, "2") == 0){\
			console_get_process_stats();
		}else if(strcmp(comando, "3") == 0){
			console_get_global_file_table();
		}else if(strcmp(comando, "4") == 0){
			console_multiprogram_degree();
		}else if(strcmp(comando, "5") == 0){
			console_finish_process();
		}else if(strcmp(comando, "6") == 0){
			console_stop_planning();
		}else if(strcmp(comando, "7") == 0){
			console_start_planning();
		}else{
			printf("[SISTEMA] - 1) Listado de procesos del sistema.\n");
			printf("[SISTEMA] - 2) Obtener estadistica de proceso.\n");
			printf("[SISTEMA] - 3) Obtener tabla global de archivos.\n");
			printf("[SISTEMA] - 4) Modificar grado de multiprogramacion.\n");
			printf("[SISTEMA] - 5) Finalizar proceso.\n");
			printf("[SISTEMA] - 6) Detener planificacion.\n");
			printf("[SISTEMA] - 7) Iniciar planificacion.\n");

		}
	}

	pthread_join(selectProgramThread, NULL);
	pthread_join(selectCPUThread, NULL);
	return EXIT_SUCCESS;
}
