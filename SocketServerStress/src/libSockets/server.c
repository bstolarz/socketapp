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
#include "recv.h"

#define BACKLOG 5

void socket_server_create(int* listenningSocket, char* port){

	struct addrinfo hints;
	struct addrinfo *serverInfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;

	getaddrinfo(NULL, port, &hints, &serverInfo);
	*listenningSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);

	bind(*listenningSocket,serverInfo->ai_addr, serverInfo->ai_addrlen);

	freeaddrinfo(serverInfo);

	listen(*listenningSocket, BACKLOG);
}

int socket_server_accept_connection(int listenningSocket){
	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);

	return accept(listenningSocket, (struct sockaddr *) &addr, &addrlen);
}

void socket_server_select(char* port, void(*socket_select_connection_lost)(fd_set*, int, int), void(*socket_select_recv_package)(fd_set*, int, int, char*)){

	int listeningSocket;
	socket_server_create(&listeningSocket, port);

	char *package;

	//Inicializo el select
	fd_set master;		// conjunto maestro de descriptores de fichero
	fd_set read_fds;	// conjunto temporal de descriptores de fichero para select()
	int fdmax;			// número máximo de descriptores de fichero
	int newfd;			// descriptor de socket de nueva conexión aceptada
	int i;
	int nbytes;


	FD_ZERO(&master);					// borra los conjuntos maestro y temporal
	FD_ZERO(&read_fds);
	FD_SET(listeningSocket, &master);	// añadir listener al conjunto maestro
	fdmax = listeningSocket; 			// seguir la pista del descriptor de fichero mayor, por ahora es este

	//Me mantengo en el bucle para asi poder procesar cambios en los sockets
	while(1) {
		//Copio los sockets y me fijo si alguno tiene cambios, si no hay itinero de vuelta
		read_fds = master; // cópialo
		if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
			perror("select");
			exit(1);
		}

		//Recorro los sockets con cambios
		for(i = 0; i <= fdmax; i++) {
			//Detecto si hay datos en un socket especifico
			if (FD_ISSET(i, &read_fds)) {
				//Si es el socket de escucha proceso el nuevo socket
				if (i == listeningSocket) {
					if ((newfd = socket_server_accept_connection(listeningSocket)) == -1){
						perror("accept");
					} else {
						FD_SET(newfd, &master); // Añado el nuevo socket al  select
						//Actualizo la cantidad
						if (newfd > fdmax) {
							fdmax = newfd;
						}
					}
				} else {
					if ((nbytes = socket_recv(i, &package, 1)) <= 0) {
						socket_select_connection_lost(&master, i, nbytes);
					} else {
						if (nbytes != 0){
							socket_select_recv_package(&master, i, nbytes, package);
						}
					}
				}
			}
		}
	}

	close(listeningSocket);
}


