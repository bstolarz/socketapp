/*
 * select.h
 *
 *  Created on: 2/4/2017
 *      Author: utnso
 */

#ifndef THREADS_SELECTPROGRAM_H_
#define THREADS_SELECTPROGRAM_H_


void select_program_socket_connection_lost(fd_set* master, int socket, int nbytes);
void select_program_socket_recive_package(fd_set* master, int socket, int nbytes, char* package);
void* select_program_thread_launcher(void* arg);

#endif /* THREADS_SELECTPROGRAM_H_ */
