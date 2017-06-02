#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <commons/config.h>
#include <commons/string.h>
#include "commons/structures.h"
#include "commons/declarations.h"
#include <commons/log.h>
#include "functions/config.h"
#include "functions/operaciones.h"
#include "init.h"
#include "libSockets/client.h"
#include "libSockets/server.h"
#include "libSockets/send.h"
#include "libSockets/recv.h"
#include "functions/auxiliares.h"
#include "functions/handler.h"

void soloParaProbarLasOperaciones();

int main(int arg, char* argv[]) {
	if (arg != 2) {
		printf("Path missing! %d\n", arg);
		return 1;
	}
	remove("logFileSystem");
	logs = log_create("logFileSystem", "FileSystem", 0, LOG_LEVEL_TRACE);
	log_info(logs, "Inicia logueo");

	configFileSystem = malloc(sizeof(t_fileSystem));
	configMetadata = malloc(sizeof(t_metadata));

	config_read(argv[1]);
	log_info(logs, "antes de config_print");
	config_print();
	log_info(logs, "antes de initSadica");

	initSadica();

	soloParaProbarLasOperaciones();
/*

	serverSocket = 0;
	socket_server_create(&serverSocket, configFileSystem->puerto);
	int socketKernel;
	while (1) {
		socketKernel = socket_server_accept_connection(serverSocket);

		if (socketKernel <= 0) {
			printf("No se pudo conectar con el server\n");
			close(socketKernel);
			config_free();
			return EXIT_FAILURE;
		}

		//Handshake
		char* identificador = "";
		socket_recv_string(socketKernel, &identificador);
		if (strcmp(identificador, "KERNEL") == 0) {
			break;
		} else {
			//Cierro el socket y vuelvo al while (vuelvo a abrir el socket para escuchar)

			printf("Se conecto alguien que no es kernel. Te equivocaste de barrio papu!\n");
			close(socketKernel);
		}
		break;
	}

	char* mensajeDeOperacion = "";
	while (1) {
		if (socket_recv_string(socketKernel, &mensajeDeOperacion) > 0) {
			log_info(logs, "Se recibio el mensaje de kernel: %s", mensajeDeOperacion);
			hacerLoQueCorresponda(mensajeDeOperacion);
		} else {
			log_info(logs, "Se desconecto el kernel.");
			close(socketKernel);
			config_free();
			return EXIT_FAILURE;
		}
	}

	unmountSadica();*/
	log_destroy(logs);
	return EXIT_SUCCESS;
}

void soloParaProbarLasOperaciones(){
	size_t cantidad = 10;
	char* comando = malloc(sizeof(char)*cantidad);

	size_t cantidadPath = 10;
	char* path = malloc(sizeof(char)*cantidad);

	int resultado = -1;
	while(1){
		printf("----------------------------------------------\n");
		printf("[Filesystem] - Los comandos permitidos son:\n");
		printf("[Filesystem] - 	VALIDAR		Valida si existe un archivo.\n");
		printf("[Filesystem] - 	CREAR		Crea un archivo y le asigna un bloque.\n");
		printf("[Filesystem] - 	BORRAR 		Borra un archivo y libera sus bloques.\n");
		printf("[Filesystem] - 	exit 		Salir del programa.\n");

		printf("Ingrese un comando:\n");
		size_t cantLeida = getline(&comando, &cantidad, stdin);
		comando[cantLeida-1]='\0';

		printf("Ingrese el path del archivo como lo mandaria el kernel:\n");
		size_t cantLeidaPath = getline(&path, &cantidadPath, stdin);
		path[cantLeidaPath-1]='\0';

		path = armarPathArchivo(path);
		if(strcmp(comando, "VALIDAR") == 0){
			log_info(logs, "Llamo al validar");
			resultado = validar(path);
		}else if(strcmp(comando, "CREAR") == 0){
			log_info(logs, "Llamo al crear");
			resultado = crear(path);
		}else if(strcmp(comando, "BORRAR") == 0){
			log_info(logs, "Llamo al borrar");
			resultado = borrar(path);
		}else if(strcmp(comando, "exit") == 0){
			break;
		}

		if(resultado == 1){
			printf("Se pudo %s archivo satisfactoriamente\n", comando);
		}else{
			printf("El archivo no existe\n");
		}
	}
}
