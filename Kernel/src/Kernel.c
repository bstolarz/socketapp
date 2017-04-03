/*
 ============================================================================
 Name        : Kernel.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <commons/collections/list.h>
#include <commons/config.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>

#include "commons/structures.h"
//#include "commons/definitions.h"
//#include "threads/select.h"

#include "commons/definitions.h"
#include "threads/select.h"
#include "libSockets/send.h"


void leerConfiguracionDeKernel(char* path){
	t_config* config=config_create(path);
	t_console* console=malloc(sizeof(t_console));
	console->algoritmo=config_get_string_value(config,"ALGORITMO");
	printf("Algoritmo: %s\n", console->algoritmo);
	console->grado_multiprog=config_get_int_value(config,"GRADO_MULTIPROG");
	printf("Grado multip: %d\n",console->grado_multiprog);
	console->ip_fs=config_get_string_value(config,"IP_FS");
	printf("IP FS: %s\n", console->ip_fs);
	console->ip_memoria=config_get_string_value(config,"IP_MEMORIA");
	printf("IP MEMORIA: %s\n", console->ip_memoria);
	console->puerto_cpu=config_get_int_value(config,"PUERTO_CPU");
	printf("PUERTO CPU: %d\n", console->puerto_cpu);
	console->puerto_fs=config_get_int_value(config,"PUERTO_FS");
	printf("PUERTO FS: %d\n", console->puerto_fs);
	console->puerto_memoria=config_get_int_value(config,"PUERTO_MEMORIA");
	printf("PUERTO MEMORIA: %d\n", console->puerto_memoria);
	console->puerto_prog=config_get_int_value(config,"PUERTO_PROG");
	printf("PUERTO PROG: %d\n", console->puerto_prog);
	console->quantum=config_get_int_value(config,"QUANTUM");
	printf("QUANTUM: %d\n", console->quantum);
	console->quantum_sleep=config_get_int_value(config,"QUANTUM_SLEEP");
	printf("QUANTUM SLEEP: %d\n", console->quantum_sleep);
	console->stack_size=config_get_int_value(config,"STACK_SIZE");
	printf("STACK SIZE: %d\n", console->stack_size);
	char** vec =config_get_array_value(config,"SEM_IDS");
	int i;
	for (i=0;i<string_length(*vec)-1;i++){
		printf("SEM %d ID: %s\n",i, vec[i]);
	}

}
int main(int args, char* argv[]) {

	//clientes = list_create();
	//pthread_create(&selectThread,NULL,selectThreadLauncher, NULL);
	//pthread_join(selectThread, NULL);
	leerConfiguracionDeKernel(argv[1]);

	clientes = list_create();
//	pthread_create(&selectThread,NULL,selectThreadLauncher, NULL);


	char str[100];

	void _send_message_clients(int* i){
		socket_send_string(*i, str);
	}

	while(1){
		scanf("%s", str);
		list_iterate(clientes, (void*)_send_message_clients);
	}


//	pthread_join(selectThread, NULL);
	return EXIT_SUCCESS;
}
