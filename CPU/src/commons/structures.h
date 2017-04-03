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
	int puerto_kernel;
}t_kernel;

typedef struct{
	char* ip_memory;
	int puerto_memory;
}t_memory;

#endif /* COMMONS_STRUCTURES_H_ */
