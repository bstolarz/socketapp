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
	t_position* args[10];
	int retPos;
	t_position retVar;
	int pos;
} t_indiceDelStack;


typedef struct{
	int pid;
	int pc;
	int cantPagsCodigo;
	int indiceDeCodigoCant;
	t_intructions * indiceDeCodigo;
	int indiceDeEtiquetasCant;
	char * indiceDeEtiquetas;
	t_list * indiceDeStack;
	t_dictionary * processFileTable;
	int stackPosition;
	int maxStackPosition;
	int exitCode;
}t_pcb;



#endif /* COMMONS_STRUCTURES_H_ */
