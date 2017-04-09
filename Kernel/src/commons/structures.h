/*
 * Structures.h
 *
 *  Created on: 1/4/2017
 *      Author: utnso
 */

#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>
#include <unistd.h>
#include <sys/types.h>
#ifndef COMMONS_STRUCTURES_H_
#define COMMONS_STRUCTURES_H_

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
}t_kernel;

typedef struct{
	int offset_inicio;
	int offset_fin;
}t_indiceDeCodigo;

typedef struct{
	char* id;
	int initialValue;
}t_semaforo;

typedef struct{
	char* nombre;
}t_sharedVar;

typedef struct{

}t_indiceDeEtiquetas;
typedef struct{

}t_indiceDelStack;

typedef struct{
	int PID;
	int PC;
	int cantPagsCodigo;
	t_indiceDeCodigo indiceDeCodigo;
	t_indiceDeEtiquetas indiceDeEtiquetas;
	t_indiceDelStack indiceDeStack;
	t_dictionary* processFileTable;
	int exitCode;
	int stackPosition;
}t_pcb;
typedef struct{

};

#endif /* COMMONS_STRUCTURES_H_ */
