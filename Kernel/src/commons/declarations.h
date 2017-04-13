/*
 * definitions.h
 *
 *  Created on: 2/4/2017
 *      Author: utnso
 */

#include <commons/collections/list.h>
#include "structures.h"
#include <commons/log.h>

#ifndef COMMONS_DEFINITIONS_H_
#define COMMONS_DEFINITIONS_H_

int programID;

pthread_t selectProgramThread;

t_kernel* configKernel;
t_log* logKernel;

t_list* listNewPrograms;
t_list* listReadyPrograms;
t_list* listExecutingPrograms;
t_list* listFinishedpPrograms;

#endif /* COMMONS_DEFINITIONS_H_ */
