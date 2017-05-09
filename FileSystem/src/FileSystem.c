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

	int serverSocket=0;
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
