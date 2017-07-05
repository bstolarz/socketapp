/*
 * program.h
 *
 *  Created on: 30/4/2017
 *      Author: utnso
 */

#ifndef FUNCTIONS_PROGRAM_H_
#define FUNCTIONS_PROGRAM_H_

int program_generate_id();
void program_process_new(fd_set* master, int socket);
int program_to_ready(t_program* program);
void program_finish(t_program* program);
void program_interrup(int socket, int interruptionCode, int overrideInterruption);
void program_unblock(t_semaforo* sem);
int get_memory_leaks(t_program* program);

#endif /* FUNCTIONS_PROGRAM_H_ */
