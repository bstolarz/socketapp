#ifndef SOCKETLIB_H_
#define SOCKETLIB_H_

void socket_server_create(int* listenningSocket, char* port);
int socket_server_accept_connection(int listenningSocket);
void socket_server_select(char* port, void(*socket_select_connection_lost)(fd_set*, int, int), void(*socket_select_recv_package)(fd_set*, int, int, char*));

#endif /* SOCKETLIB_H_ */
