#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdint.h>
#include <pthread.h>

#ifndef SOCKETS_SELECT_H_
#define SOCKETS_SELECT_H_

void socket_select_connection_lost(fd_set* master, int socket, int nbytes);
void socket_select_recive_package(fd_set* master, int socket, int nbytes, char* package);

#endif /* SOCKETS_SELECT_H_ */
