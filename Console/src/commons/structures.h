/*
 * Structures.h
 *
 *  Created on: 1/4/2017
 *      Author: utnso
 */

#ifndef COMMONS_STRUCTURES_H_
#define COMMONS_STRUCTURES_H_
#include <commons/log.h>
#include <pthread.h>

typedef struct{
	char* ip_kernel;
	char* puerto_kernel;
}t_console;

typedef struct{
	int pid;
	int socketKernel;
	pthread_t thread;
	char* pathProgram;
	int die;
}t_program;

#endif /* COMMONS_STRUCTURES_H_ */
