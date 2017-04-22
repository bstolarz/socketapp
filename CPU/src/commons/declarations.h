/*
 * declarations.h
 *
 *  Created on: 5/4/2017
 *      Author: utnso
 */

#include "structures.h"
#include <commons/log.h>
#ifndef COMMONS_DECLARATIONS_H_
#define COMMONS_DECLARATIONS_H_
#include <parser/metadata_program.h>
#include <parser/parser.h>
#include <parser/metadata_program.h>

t_cpu* configCPU;
t_log* logCPU;
pthread_t pthreadMemory;
pthread_t pthreadKernel;
AnSISOP_funciones* funciones;
AnSISOP_kernel* kernel;
t_pcb* pcb;
int pageSize;
int serverMemory;
int serverKernel;
#endif /* COMMONS_DECLARATIONS_H_ */
