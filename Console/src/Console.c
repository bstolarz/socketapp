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
#include "libSockets/client.h"
#include "libSockets/send.h"
#include "libSockets/recv.h"

void leerArchivoConfiguracion(char* path){

	t_config* config=config_create(path);
	t_console* console=malloc(sizeof(t_console));
	console->ip_kernel=config_get_string_value(config,"IP_KERNEL");
	console->puerto_kernel=config_get_int_value(config,"PUERTO_KERNEL");
	printf("La ip del kernel es: %s\n",console->ip_kernel);
	printf("El puerto del kernel es: %d\n",console->puerto_kernel);
}

int main(int arg, char* argv[]) {
	if(arg!=2){
		printf("Te falta poner el path! %d\n", arg);
		return 1;
	}
	leerArchivoConfiguracion(argv[1]);
	//leerArchivoConfiguracion("/home/utnso/git/tp-2017-1c-SocketApp/console");

	int serverSocket=0;
	socket_client_create(&serverSocket, "127.0.0.1", "6667");
	if(serverSocket){
		socket_send_string(serverSocket, "Hola, aca  estoy :D");
		char* mensaje = "";
		while(1){
			socket_recv_string(serverSocket, &mensaje);
			printf("%s\n", mensaje);
		}
	}

	return EXIT_SUCCESS;
}
