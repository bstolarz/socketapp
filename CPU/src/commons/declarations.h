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

t_cpu* configCPU;
t_log* logCPU;
pthread_t pthreadMemory;
pthread_t pthreadKernel;
#endif /* COMMONS_DECLARATIONS_H_ */
