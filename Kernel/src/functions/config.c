/*
 * config.c
 *
 *  Created on: 3/4/2017
 *      Author: utnso
 */
#include <stdio.h>
#include <stdlib.h>
#include <commons/collections/list.h>
#include <commons/config.h>
#include <commons/string.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>

#include "../commons/declarations.h"


void config_read_semaforos(t_config* config){
	char** s=config_get_array_value(config,"SEM_IDS");
	char** vs=config_get_array_value(config,"SEM_INIT");

	configKernel->semaforos=list_create();
	while(*s!=NULL && *vs!= NULL){
		t_semaforo* sem = malloc(sizeof(t_semaforo));
		sem->id=string_duplicate(*s);
		sem->initialValue=atoi(*vs);

		list_add(configKernel->semaforos, s);

		s++;
		vs++;
	}

}
void config_read_shared_vars(t_config* config){
	char** vc=config_get_array_value(config,"SHARED_VARS");

	configKernel->shared_vars=list_create();

	while(*vc!=NULL){
		t_sharedVar* sv=malloc(sizeof(t_sharedVar));
		sv->nombre=string_duplicate(*vc);

		list_add(configKernel->shared_vars,sv);

		vc++;
	}
}
void config_read(char* path){
	t_config* config=config_create(path);

	configKernel->algoritmo=string_duplicate(config_get_string_value(config,"ALGORITMO"));
	configKernel->grado_multiprog=config_get_int_value(config,"GRADO_MULTIPROG");
	configKernel->ip_fs=string_duplicate(config_get_string_value(config,"IP_FS"));
	configKernel->ip_memoria=string_duplicate(config_get_string_value(config,"IP_MEMORIA"));
	configKernel->puerto_cpu=config_get_int_value(config,"PUERTO_CPU");
	configKernel->puerto_fs=config_get_int_value(config,"PUERTO_FS");
	configKernel->puerto_memoria=config_get_int_value(config,"PUERTO_MEMORIA");
	configKernel->puerto_prog=config_get_int_value(config,"PUERTO_PROG");
	configKernel->quantum=config_get_int_value(config,"QUANTUM");
	configKernel->quantum_sleep=config_get_int_value(config,"QUANTUM_SLEEP");
	configKernel->stack_size=config_get_int_value(config,"STACK_SIZE");

	config_read_semaforos(config);
	config_read_shared_vars(config);

	config_destroy(config);
}

void config_print(char* path){

	printf("Algoritmo: %s\n", configKernel->algoritmo);
	printf("Grado multip: %d\n",configKernel->grado_multiprog);
	printf("IP FS: %s\n", configKernel->ip_fs);
	printf("IP MEMORIA: %s\n", configKernel->ip_memoria);
	printf("PUERTO CPU: %d\n", configKernel->puerto_cpu);
	printf("PUERTO FS: %d\n", configKernel->puerto_fs);
	printf("PUERTO MEMORIA: %d\n", configKernel->puerto_memoria);
	printf("PUERTO PROG: %d\n", configKernel->puerto_prog);
	printf("QUANTUM: %d\n", configKernel->quantum);
	printf("QUANTUM SLEEP: %d\n", configKernel->quantum_sleep);
	printf("STACK SIZE: %d\n", configKernel->stack_size);

	void _printSemaforo(t_semaforo* semaforo){
		printf("Semaforo: %s, Valor: %i\n", semaforo->id, semaforo->initialValue);
	}
	list_iterate(configKernel->semaforos, (void*)_printSemaforo);

	void _printShareVar(t_sharedVar* sharevar){
		printf("Share var: %s\n", sharevar->nombre);
	}
	list_iterate(configKernel->shared_vars, (void*)_printShareVar);
}
