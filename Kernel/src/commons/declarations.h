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
int pageSize;

pthread_t selectProgramThread;
pthread_t selectCPUThread;

t_kernel* configKernel;
t_log* logKernel;

t_queue* queueNewPrograms;
t_queue* queueReadyPrograms;
t_queue* queueBlockedPrograms;
t_queue* queueFinishedPrograms;

t_queue* queueCPUs;

t_socket memoryServer;
t_socket fileSystemServer;

t_secure_list* globalFileDescriptors;

char* FILE_DESCRIPTOR_PERMISSION_READ;
char* FILE_DESCRIPTOR_PERMISSION_WRITE;
char* FILE_DESCRIPTOR_PERMISSION_CREATE;

fd_set* programMasterRecord;

#endif /* COMMONS_DEFINITIONS_H_ */
