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
	int offset_inicio;
	int offset_fin;
}t_indiceDeCodigo;

typedef struct{

}t_indiceDeEtiquetas;
typedef struct{
	int page;
	int off;
	int size;
}t_position;

typedef struct{
	t_nombre_variable ID;
	t_position* pos;
}t_var;


typedef struct{
	t_dictionary* vars;
	t_dictionary* args;
	int retPos;
	t_var* retVar;
	int pos;
}t_indiceDelStack;


typedef struct{
	int pid;
	int pc;
	int cantPagsCodigo;
	t_metadata_program* metadata;
	t_indiceDeCodigo * indiceDeCodigo;
	t_dictionary * indiceDeEtiquetas;
	t_list * indiceDeStack; // cambiar por diccionario t_dictionary vars;
	t_dictionary * processFileTable;
	int exitCode;
	int stackPosition;
}t_pcb;



#endif /* COMMONS_STRUCTURES_H_ */
