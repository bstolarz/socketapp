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

int main(int arg, char* argv[]) {
	if(arg!=2){
		printf("Te falta poner el path! %d\n", arg);
		//return -1;
	}

	configConsole = malloc(sizeof(t_console));
	//config_read(argv[1]);
	config_read("/home/utnso/git/tp-2017-1c-SocketApp/console");
	config_print();

	int serverSocket=0;
	socket_client_create(&serverSocket, configConsole->ip_kernel, configConsole->puerto_kernel);
	if(serverSocket){
		socket_send_string(serverSocket, "CON");
		char* mensaje = "";
		while(1){
			socket_recv_string(serverSocket, &mensaje);
			printf("%s\n", mensaje);
		}
	}

	return EXIT_SUCCESS;
}
