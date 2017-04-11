/*
 * Structures.h
 *
 *  Created on: 1/4/2017
 *      Author: utnso
 */

#ifndef COMMONS_STRUCTURES_H_
#define COMMONS_STRUCTURES_H_
#include <commons/log.h>

typedef struct{
	char* ip_kernel;
	char* puerto_kernel;
}t_console;

t_log* logConsole;
#endif /* COMMONS_STRUCTURES_H_ */
