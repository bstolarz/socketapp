/*
 * program.h
 *
 *  Created on: 13/4/2017
 *      Author: utnso
 */



#ifndef FUNCTIONS_PROGRAM_H_
#define FUNCTIONS_PROGRAM_H_

int program_generate_id();
void program_process_new(fd_set* master, int socket);
void program_interrup(int socket, int interruptionCode, int overrideInterruption);

#endif /* FUNCTIONS_PROGRAM_H_ */
