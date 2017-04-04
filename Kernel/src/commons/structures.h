/*
 * Structures.h
 *
 *  Created on: 1/4/2017
 *      Author: utnso
 */

#ifndef COMMONS_STRUCTURES_H_
#define COMMONS_STRUCTURES_H_
#include <commons/collections/list.h>
typedef struct{
	int puerto_prog;
	int puerto_cpu;
	char* ip_memoria;
	int puerto_memoria;
	char* ip_fs;
	int puerto_fs;
	int quantum;
	int quantum_sleep;
	char* algoritmo;
	int grado_multiprog;
	t_list* semaforos;
	t_list* shared_vars;
	int stack_size;
}t_console;
typedef struct{
	char* id;
	int initialValue;
}t_semaforo;
typedef struct{
	char* nombre;
}t_sharedVars;
#endif /* COMMONS_STRUCTURES_H_ */
