/*
 * Structures.h
 *
 *  Created on: 1/4/2017
 *      Author: utnso
 */

#ifndef COMMONS_STRUCTURES_H_
#define COMMONS_STRUCTURES_H_

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

}t_indiceDelStack;

typedef struct{
	int pid;
	int pc;
	int cantPagsCodigo;
	t_indiceDeCodigo * indiceDeCodigo;
	t_indiceDeEtiquetas * indiceDeEtiquetas;
	t_indiceDelStack * indiceDeStack;
	t_dictionary * processFileTable;
	int exitCode;
	int stackPosition;
}t_pcb;



#endif /* COMMONS_STRUCTURES_H_ */
