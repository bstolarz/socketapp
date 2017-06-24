/*
 * declarations.h
 *
 *  Created on: 5/4/2017
 *      Author: utnso
 */

#include "structures.h"
#include <commons/log.h>
#include <parser/metadata_program.h>
#include <parser/parser.h>
#include <parser/metadata_program.h>

#ifndef COMMONS_DECLARATIONS_H_
#define COMMONS_DECLARATIONS_H_


t_cpu* configCPU;
t_log* logCPU;

pthread_t pthreadMemory;
pthread_t pthreadKernel;

AnSISOP_funciones* ansisop_funciones;
AnSISOP_kernel* ansisop_funciones_kernel;

t_pcb* pcb;
int pageSize;
int desconectar;
int serverKernel;
int serverMemory;
int isExecuting;
#endif /* COMMONS_DECLARATIONS_H_ */
