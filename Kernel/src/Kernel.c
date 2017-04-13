/*
 ============================================================================
 Name        : Kernel.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include "functions/config.h"
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
#include "commons/structures.h"
#include "threads/program/select.h"
#include "threads/cpu/select.h"

int main(int argc, char* argv[]) {
	logKernel=log_create("LogKernel","Kernel",false,LOG_LEVEL_DEBUG);
	if(argc!=2){
		printf("Missing config path\n");
		return -1;
	}

	configKernel=malloc(sizeof(t_kernel));
	config_read(argv[1]);
	config_print();

	programID = 0;
	listNewPrograms=list_create();
	listReadyPrograms=list_create();
	listExecutingPrograms=list_create();
	listFinishedpPrograms=list_create();

	listCPUs=list_create();

	pthread_create(&selectProgramThread,NULL,select_program_thread_launcher, NULL);
	pthread_create(&selectCPUThread,NULL,select_cpu_thread_launcher, NULL);

	pthread_join(selectProgramThread, NULL);
	pthread_join(selectCPUThread, NULL);
	return EXIT_SUCCESS;
}
