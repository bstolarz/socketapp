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

//void soloParaProbarLasOperaciones();

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
	config_print();

	metadataFS_read("./mnt/SADICA_FS/Metadata/Metadata.bin");
	metadataFS_print();

	initSadica();

	serverSocket = 0;
	socket_server_create(&serverSocket, configFileSystem->puerto);

	while (1) {
		socketKernel = socket_server_accept_connection(serverSocket);

		if (socketKernel <= 0) {
			printf("No se pudo conectar con el server\n");
			close(socketKernel);
			config_free();
			return EXIT_FAILURE;
		}

		//Handshake
		char* identificador = string_new();
		socket_recv_string(socketKernel, &identificador);
		if (strcmp(identificador, "KERNEL") == 0) {
			printf("Se conecto Kernel. Esperando mensajes.\n");
			free(identificador);

			char* mensajeDeOperacion = string_new();
			while (1) {
				if (socket_recv_string(socketKernel, &mensajeDeOperacion) > 0) {
					printf("Se recibio el mensaje del kernel: %s\n", mensajeDeOperacion);
					log_info(logs, "Se recibio el mensaje de kernel: %s", mensajeDeOperacion);
					hacerLoQueCorresponda(mensajeDeOperacion);
				} else {
					printf("Se desconecto el kernel.\n");
					log_info(logs, "Se desconecto el kernel.");
					close(socketKernel);
					break;
				}
			}

			free(mensajeDeOperacion);
		} else {
			//Cierro el socket y vuelvo al while (vuelvo a abrir el socket para escuchar)
			free(identificador);
			printf("Se conecto alguien que no es kernel. Te equivocaste de barrio papu!\n");
			close(socketKernel);
		}
	}


	log_destroy(logs);
	return EXIT_SUCCESS;
}

/*void soloParaProbarLasOperaciones(){
	int resultado = -1;
	while(1){
		size_t cantidad = 10;
		char* comando = malloc(sizeof(char)*cantidad);

		size_t cantidadPath = 10;
		char* path = malloc(sizeof(char)*cantidad);

		printf("----------------------------------------------\n");
		printf("[Filesystem] - Los comandos permitidos son:\n");
		printf("[Filesystem] - 	VALIDAR\t\tValida si existe un archivo.\n");
		printf("[Filesystem] - 	CREAR\t\tCrea un archivo y le asigna un bloque.\n");
		printf("[Filesystem] - 	BORRAR\t\tBorra un archivo y libera sus bloques.\n");
		printf("[Filesystem] - 	OBTENERDATOS\tLee el contenido de los bloques de un archivo.\n");
		printf("[Filesystem] - 	GUARDARDATOS\tGuarda contenido en un archivo.\n");
		printf("[Filesystem] - 	exit\t\tSalir del programa.\n");

		printf("Ingrese un comando:\n");
		size_t cantLeida = getline(&comando, &cantidad, stdin);
		comando[cantLeida-1]='\0';

		if(strcmp(comando, "exit") == 0){
			break;
		}

		printf("Ingrese el path del archivo como lo mandaria el kernel:\n");
		size_t cantLeidaPath = getline(&path, &cantidadPath, stdin);
		path[cantLeidaPath-1]='\0';

		path = armarPathArchivo(path);

		if(strcmp(comando, "VALIDAR") == 0){
			resultado = validar(path);
			if(resultado > 0){
				printf("Se pudo %s archivo satisfactoriamente\n", comando);
			}else{
				printf("El archivo no existe\n");
			}
		}else if(strcmp(comando, "CREAR") == 0){
			resultado = crear(path);
			if(resultado == 1){
				printf("Se pudo %s archivo satisfactoriamente\n", comando);
			}else{
				printf("El archivo no existe\n");
			}
		}else if(strcmp(comando, "BORRAR") == 0){
			resultado = borrar(path);
			if(resultado == 1){
				printf("Se pudo %s archivo satisfactoriamente\n", comando);
			}else{
				printf("El archivo no existe\n");
			}
		}else if(strcmp(comando, "OBTENERDATOS") == 0){
			printf("Ingrese el offset:\n");
			int offset;
			scanf("%d", &offset);

			printf("Ingrese el size:\n");
			int size;
			scanf("%d", &size);

			char* buf = malloc(0);
			resultado = obtenerDatos(path,offset,size,&buf);

			if(resultado == 1){
				printf("Se pudo %s archivo satisfactoriamente\n", comando);
				log_info(logs, "Bytes leidos: %d", resultado);
				log_info(logs, "Buffer leido:");
				log_info(logs, "%s", buf);
			}
		}else if(strcmp(comando, "GUARDARDATOS") == 0){


				printf("Ingrese el offset:\n");
				int offset;
				scanf("%d", &offset);


				printf("Ingrese el size:\n");
				int size;
				scanf("%d", &size);

				char * contenido = string_new();
				char * str = string_repeat('a', size);
				string_append(&contenido, str);

				log_info(logs, "%s", contenido);


				resultado = guardarDatos(path, offset, size, contenido);


				if(resultado == 1){
					printf("Se pudo %s archivo satisfactoriamente\n", comando);
					log_info(logs, "Buffer grabado:");
					log_info(logs, "%s", contenido);
				}

				free(contenido);

			}else if (resultado == -1){
				printf("Pusiste un offset mayor al tamanioArchivo => no leo nada\n");
			}
			else{
				printf("El archivo no existe\n");
			}
		free(path);
		free(comando);

	}*/
