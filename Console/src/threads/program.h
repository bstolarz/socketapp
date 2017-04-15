/*
 * program.h
 *
 *  Created on: 11/4/2017
 *      Author: utnso
 */
#include "../commons/structures.h"


#ifndef THREADS_PROGRAM_H_
#define THREADS_PROGRAM_H_

void thread_program_destroy(t_program* program, int insideThread);
void* thread_program(void * params);

#endif /* THREADS_PROGRAM_H_ */
