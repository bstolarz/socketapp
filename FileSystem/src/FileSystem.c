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

void hacerLoQueCorresponda(char* mensajeDeOperacion);

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

	unmountSadica();
	log_destroy(logs);
	return EXIT_SUCCESS;
}

//Falta hacer las condiciones de que si el path no se encontro para obtener y guardar, que retorne un error de archivo no encontrado
void hacerLoQueCorresponda(char* unMensajeDeOperacion) {
	char* path = "";
	int offset;
	int size;
	int resultado;

	socket_recv_string(serverSocket, &path);
	log_info(logs, "Recibi el path: %s", path);

	if (strcmp(unMensajeDeOperacion, "VALIDAR") == 0) {
		log_info(logs, "Llamo a la funcion validar");
		resultado = validar(path);
	} else if (string_equals_ignore_case(unMensajeDeOperacion, "CREAR")) {
		log_info(logs, "Llamo a la funcion crear");
		resultado = crear(path);
	} else if (string_equals_ignore_case(unMensajeDeOperacion, "BORRAR")) {
		log_info(logs, "Llamo a la funcion borrar");
		resultado = borrar(path);
	} else if (string_equals_ignore_case(unMensajeDeOperacion,"OBTENERDATOS")) {
		socket_recv_int(serverSocket, &offset);
		socket_recv_int(serverSocket, &size);
		log_info(logs, "Recibi el offset: %d", offset);
		log_info(logs, "Recibi el size: %d", size);

		log_info(logs, "Llamo a la funcion obtenerDatos");
		resultado = obtenerDatos(path, (off_t) offset, (size_t) size);
	} else if (string_equals_ignore_case(unMensajeDeOperacion,"GUARDARDATOS")) {
		void* buffer;

		socket_recv_int(serverSocket, &offset);
		socket_recv_int(serverSocket, &size);
		socket_recv(serverSocket, &buffer, 1);
		log_info(logs, "Recibi el offset: %d", offset);
		log_info(logs, "Recibi el size: %d", size);
		log_info(logs, "Recibi el buffer: %s", buffer);

		log_info(logs, "Llamo a la funcion guardarDatos");
		resultado = guardarDatos(path, (off_t) offset, (size_t) size, buffer);
	}

	socket_send_int(serverSocket, resultado);

	free(path);
}
