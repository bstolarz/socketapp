/*
 * recv.h
 *
 *  Created on: 1/4/2017
 *      Author: utnso
 */

#ifndef SOCKETS_RECV_H_
#define SOCKETS_RECV_H_

int socket_recv(int clientSocket, void** buffer, int reserveSpace);
int socket_recv_string(int clientSocket, char** text);
int socket_recv_int(int clientSocket, int* value);

#endif /* SOCKETS_RECV_H_ */
