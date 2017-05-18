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
		sem->nombre=string_duplicate(*s);
		sem->value=atoi(*vs);
		pthread_mutex_init(&sem->mutex, NULL);

		list_add(configKernel->semaforos, sem);

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
		sv->value = 0;
		pthread_mutex_init(&sv->mutex, NULL);

		list_add(configKernel->shared_vars,sv);

		vc++;
	}
}
void config_read(char* path){
	t_config* config=config_create(path);

	configKernel->algoritmo			= string_duplicate(config_get_string_value(config,"ALGORITMO"));
	configKernel->grado_multiprog	= config_get_int_value(config,"GRADO_MULTIPROG");
	configKernel->ip_fs				= string_duplicate(config_get_string_value(config,"IP_FS"));
	configKernel->ip_memoria		= string_duplicate(config_get_string_value(config,"IP_MEMORIA"));
	configKernel->puerto_cpu		= string_duplicate(config_get_string_value(config,"PUERTO_CPU"));
	configKernel->puerto_fs			= string_duplicate(config_get_string_value(config,"PUERTO_FS"));
	configKernel->puerto_memoria	= string_duplicate(config_get_string_value(config,"PUERTO_MEMORIA"));
	configKernel->puerto_prog		= string_duplicate(config_get_string_value(config,"PUERTO_PROG"));
	configKernel->quantum			= config_get_int_value(config,"QUANTUM");
	configKernel->quantum_sleep		= config_get_int_value(config,"QUANTUM_SLEEP");
	configKernel->stack_size		= config_get_int_value(config,"STACK_SIZE");

	config_read_semaforos(config);
	config_read_shared_vars(config);

	config_destroy(config);
}
