#ifndef SOCKETLIB_H_
#define SOCKETLIB_H_

void socket_server_create(int* listenningSocket, char* port);
void socket_server_accept_connection(int listenningSocket, int* clientSocket);
void socket_server_select(char* port, int packageSize, void(*socket_select_connection_lost)(fd_set*, int, int), void(*socket_select_recv_package)(fd_set*, int, int, char*));

#endif /* SOCKETLIB_H_ */
