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
#include <signal.h>
#include <pthread.h>
#include <errno.h>
#include "../commons/structures.h"


void generarListaDeSemaforos(t_config* config, t_console* console){
	char** s=config_get_array_value(config,"SEM_IDS");
	char** vs=config_get_array_value(config,"SEM_INIT");
	console->semaforos=list_create();
	while(*s!=NULL && *vs!= NULL){
					t_semaforo* sem = malloc(sizeof(t_semaforo));
					sem->id=*s;
					sem->initialValue=atoi(*vs);
					list_add(console->semaforos, s);
					s++;
					vs++;
					printf("%s: %d\n", sem->id, sem->initialValue);
	}

}
void generarVectorDeVariablesCompartidas(t_config* config,t_console* console){
	char** vc=config_get_array_value(config,"SHARED_VARS");
	console->shared_vars=list_create();
	while(*vc!=NULL){
		t_sharedVars* sv=malloc(sizeof(t_sharedVars));
		sv->nombre=*vc;
		list_add(console->shared_vars,sv);
		vc++;
		printf("%s\n", sv->nombre);

	}
}
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
	generarListaDeSemaforos(config,console);
	generarVectorDeVariablesCompartidas(config,console);
	}
