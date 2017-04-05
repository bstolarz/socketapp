/*
 ============================================================================
 Name        : Console.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include "commons/structures.h"
#include "commons/declarations.h"
#include "functions/config.h"
#include "libSockets/client.h"
#include "libSockets/send.h"
#include "libSockets/recv.h"

int main(int argc, char* argv[]) {
	if(argc!=2){
		printf("Missing config path\n");
		return -1;
	}

	configConsole = malloc(sizeof(t_console));
	config_read(argv[1]);
	//config_read("/home/utnso/git/tp-2017-1c-SocketApp/console");
	config_print();

	int serverSocket=0;
	socket_client_create(&serverSocket, "127.0.0.1", "6667");
	if(serverSocket){
		socket_send_string(serverSocket, "MEM");
		char* mensaje = "";
		while(1){
			if(socket_recv_string(serverSocket, &mensaje)>0){
				printf("%s\n", mensaje);
			}else{
				return -1;
			}
		}
	}

	return EXIT_SUCCESS;
}
