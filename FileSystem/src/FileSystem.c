#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <commons/config.h>
#include "commons/structures.h"
#include "commons/declarations.h"
#include "functions/config.h"
#include "libSockets/client.h"
#include "libSockets/send.h"
#include "libSockets/recv.h"

int main(int arg, char* argv[]) {
	if(arg!=2){
		printf("Path missing! %d\n", arg);
		return 1;
	}

	configFileSystem=malloc(sizeof(t_fileSystem));
	config_read(argv[1]);
	config_print();

	serverSocket = 0;
	socket_server_create(&serverSocket, "127.0.0.1", configFileSystem->puerto);

	if(serverSocket<=0){
		printf("No se pudo conectar con el server\n");
		close(serverSocket);
		config_free();
		return EXIT_FAILURE;
	}


	char* mensajeDeOperacion = "";
	while(1){
		if(socket_recv_string(serverSocket, &mensajeDeOperacion)>0){
			hacerLoQueCorresponda(mensajeDeOperacion);
		}else{
			printf("No se pudo recibir el mensaje\n");
			close(serverSocket);
			config_free();
			return EXIT_FAILURE;
		}
	}


	return EXIT_SUCCESS;
}

//Falta hacer las condiciones de que si el path no se encontro para obtener y guardar, que retorne un error de archivo no encontrado
void hacerLoQueCorresponda(char* unMensajeDeOperacion){
	char* path = "";
	off_t offset;
	size_t size;

	if(string_equals_ignore_case(unMensajeDeOperacion, "VALIDAR")){
		socket_recv_string(serverSocket, &path);
		int resultado = validar(path);

		socket_send_int(serverSocket, resultado);
	}
	else if(string_equals_ignore_case(unMensajeDeOperacion, "CREAR")){
		socket_recv_string(serverSocket, &path);
		int resultado = crear(path);
	}
	else if(string_equals_ignore_case(unMensajeDeOperacion, "BORRAR")){
		socket_recv_string(serverSocket, &path);
		int resultado = borrar(path);
	}
	else if(string_equals_ignore_case(unMensajeDeOperacion, "OBTENERDATOS")){
		socket_recv_string(serverSocket, &path);
		socket_recv_int(serverSocket, &offset);
		socket_recv_int(serverSocket, &size);
		size_t cantidadBytes = obtenerDatos(path, offset, size);

		//Le devuelvo al Kernel la cantidad de bytes definidos por el size
		socket_send_int(serverSocket, cantidadBytes);
	}
	else if(string_equals_ignore_case(unMensajeDeOperacion, "GUARDARDATOS")){
		void* buffer;

		socket_recv_string(serverSocket, &path);
		socket_recv_int(serverSocket, &offset);
		socket_recv_int(serverSocket, &size);

		//Recibir buffer, con recv_string? recv_int? socket_recv?
		int resultado = guardarDatos(path, offset, size, buffer);
	}



	free(path);
}
