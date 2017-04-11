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
#include "threads/select.h"
#include "commons/structures.h"

int main(int argc, char* argv[]) {
	logKernel=log_create("LogKernel","Kernel",false,LOG_LEVEL_DEBUG);
	if(argc!=2){
		printf("Missing config path\n");
		return -1;
	}

	configKernel=malloc(sizeof(t_kernel));
	config_read(argv[1]);
	config_print();
	list_PCBs=list_create();
	CPUs=list_create();
	clientes = list_create();
	pthread_create(&selectThread,NULL,selectThreadLauncher, NULL);

	//Reenviar mensaje al resto de los clientes


	pthread_join(selectThread, NULL);
	return EXIT_SUCCESS;
}
