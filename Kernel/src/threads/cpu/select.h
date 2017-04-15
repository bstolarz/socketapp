/*
 * select.h
 *
 *  Created on: 13/4/2017
 *      Author: utnso
 */

#ifndef THREADS_CPU_SELECT_H_
#define THREADS_CPU_SELECT_H_

void select_cpu_socket_connection_lost(fd_set* master, int socket, int nbytes);
void select_cpu_socket_recive_package(fd_set* master, int socket, int nbytes, char* package);
void* select_cpu_thread_launcher(void* arg);


#endif /* THREADS_CPU_SELECT_H_ */
