/*
 * Structures.h
 *
 *  Created on: 1/4/2017
 *      Author: utnso
 */

#ifndef COMMONS_STRUCTURES_H_
#define COMMONS_STRUCTURES_H_
#include <pthread.h>

typedef struct{
	char* ip_kernel;
	char* puerto_kernel;
}t_console;

typedef struct{
	char* stringInicioEjecucion;
	int timestampInicio;
	int cantImpresionesPantalla;	
}t_statistics;

typedef struct{
	int pid;
	int socketKernel;
	pthread_t thread;
	char* pathProgram;
	int die;
	t_statistics* stats;
}t_program;

#endif /* COMMONS_STRUCTURES_H_ */
