/*
 * Structures.h
 *
 *  Created on: 1/4/2017
 *      Author: utnso
 */

#ifndef COMMONS_STRUCTURES_H_
#define COMMONS_STRUCTURES_H_

#include <commons/collections/list.h>
#include <parser/metadata_program.h>

typedef struct{
	char* ip_kernel;
	char* puerto_kernel;

	char* ip_memory;
	char* puerto_memory;
}t_cpu;

typedef struct{
	int page;
	int off;
	int size;
} t_position;


typedef struct{
	t_dictionary* vars;
	size_t argCount;
	t_position args[10]; // pueden haber de 0 a 9 args. por valor para no necesitar allocar y dellocar
	int retPos;
	t_position* retVar;
} t_indiceDelStack;


typedef struct{
	int pid;
	size_t pc;
	size_t cantPagsCodigo;
	size_t indiceDeCodigoCant;
	t_intructions * indiceDeCodigo;
	size_t indiceDeEtiquetasCant;
	char * indiceDeEtiquetas;
	t_list * indiceDeStack; // pila hecha con lista
	t_dictionary * processFileTable;
	size_t stackPosition;
	size_t maxStackPosition;
	int exitCode;
}t_pcb;



#endif /* COMMONS_STRUCTURES_H_ */
