/*
 * Structures.h
 *
 *  Created on: 1/4/2017
 *      Author: utnso
 */

#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>
#include <parser/metadata_program.h>
#include <unistd.h>
#include <sys/types.h>
#ifndef COMMONS_STRUCTURES_H_
#define COMMONS_STRUCTURES_H_

typedef struct{
	char* puerto_prog;
	char* puerto_cpu;
	char* ip_memoria;
	char* puerto_memoria;
	char* ip_fs;
	char* puerto_fs;
	int quantum;
	int quantum_sleep;
	char* algoritmo;
	int grado_multiprog;
	t_list* semaforos;
	t_list* shared_vars;
	int stack_size;
}t_kernel;

typedef struct{
	char* nombre;
	int value;
	pthread_mutex_t mutex;
}t_semaforo;

typedef struct{
	char* nombre;
	int value;
	pthread_mutex_t mutex;
}t_sharedVar;

typedef struct{

}t_indiceDelStack;

typedef struct{
	int pid;
	int pc;
	int cantPagsCodigo;
	int indiceDeCodigoCant;
	t_intructions * indiceDeCodigo;
	int indiceDeEtiquetasCant;
	char * indiceDeEtiquetas;
	t_indiceDelStack * indiceDeStack;
	t_dictionary * processFileTable;
	int stackPosition;
	int maxStackPosition;
	int exitCode;
}t_pcb;

typedef struct{
	int socket;
	t_pcb * pcb;
	int interruptionCode;
	int codeSize;
	void* code;
}t_program;

typedef struct{
	int socket;
	t_program* program;
}t_cpu;

typedef struct{
	pthread_mutex_t mutex;
	t_list * list;
}t_queue;

typedef struct{
	int socket;
	pthread_mutex_t mutex;
}t_socket;

#endif /* COMMONS_STRUCTURES_H_ */
