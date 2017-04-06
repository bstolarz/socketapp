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
#include <unistd.h>
#include <commons/config.h>
#include <commons/string.h>
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
	if(serverSocket<=0){
		printf("No logre conectarme al serve\n");
		close(serverSocket);
		config_free();
		return EXIT_FAILURE;
	}

	char *str=string_new();
	while(1){
		printf("Ingrese un mensaje:\n");
		scanf("%s", str);
		if(socket_send_string(serverSocket, "CON")<=0){
			printf("No se pudo enviar el mensaje\n");
			close(serverSocket);
			config_free();
			return EXIT_FAILURE;
		}

		if(socket_send_string(serverSocket, str)<=0){
			printf("No se pudo enviar el mensaje\n");
			close(serverSocket);
			config_free();
			return EXIT_FAILURE;
		}
	}


	return EXIT_SUCCESS;
}
