#ifndef SOCKETLIB_H_
#define SOCKETLIB_H_

void socket_server_create(int* listenningSocket, char* port);
void socket_server_accept_connection(int listenningSocket, int* clientSocket);

#endif /* SOCKETLIB_H_ */
