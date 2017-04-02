/*
 * send.h
 *
 *  Created on: 1/4/2017
 *      Author: utnso
 */

#ifndef SOCKETS_SEND_H_
#define SOCKETS_SEND_H_

int socket_send(int clientSocket, void* parameter, int size);
int socket_send_string(int clientSocket, char* parameter);
int socket_send_int(int clientSocket, int value);

#endif /* SOCKETS_SEND_H_ */
