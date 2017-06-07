#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <commons/config.h>
#include <commons/string.h>
#include "../commons/structures.h"
#include "../commons/declarations.h"
#include "../libSockets/client.h"
#include "../libSockets/server.h"
#include "../libSockets/send.h"
#include "../libSockets/recv.h"
#include "bitmap.h"
#include "auxiliares.h"
#include "operaciones.h"

//Falta hacer las condiciones de que si el path no se encontro para obtener y guardar, que retorne un error de archivo no encontrado
void hacerLoQueCorresponda(char* unMensajeDeOperacion) {
	char* path = string_new();
	int offset;
	int size;
	int resultado;

	socket_recv_string(serverSocket, &path);
	log_info(logs, "Recibi el path de kernel: %s", path);
	path = armarPathArchivo(path);

	if (strcmp(unMensajeDeOperacion, "VALIDAR") == 0) {
		resultado = validar(path);

	} else if (strcmp(unMensajeDeOperacion, "CREAR") == 0) {
		resultado = crear(path);

	} else if (strcmp(unMensajeDeOperacion, "BORRAR") == 0) {
		resultado = borrar(path);

	} else if (strcmp(unMensajeDeOperacion,"OBTENERDATOS") == 0) {
		socket_recv_int(serverSocket, &offset);
		socket_recv_int(serverSocket, &size);
		log_info(logs, "Recibi el offset: %d", offset);
		log_info(logs, "Recibi el size: %d", size);

		char* buffer = malloc(0);
		resultado = obtenerDatos(path, offset, size, &buffer);

	} else if (strcmp(unMensajeDeOperacion,"GUARDARDATOS") == 0) {
		void* buffer;

		socket_recv_int(serverSocket, &offset);
		socket_recv_int(serverSocket, &size);
		socket_recv(serverSocket, &buffer, 1);
		log_info(logs, "Recibi el offset: %d", offset);
		log_info(logs, "Recibi el size: %d", size);
		log_info(logs, "Recibi el buffer: %s", buffer);

		resultado = guardarDatos(path, offset, size, buffer);
	}

	socket_send_int(serverSocket, resultado);

	free(path);
}
