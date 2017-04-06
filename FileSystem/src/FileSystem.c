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
	socket_client_create(&serverSocket, "127.0.0.1", "6667");
	if(serverSocket<=0){
		printf("No se pudo conectar con el server\n");
		close(serverSocket);
		config_free();
		return EXIT_FAILURE;
	}

	if(socket_send_string(serverSocket, "FSY")<=0){
		printf("No se pudo enviar el mensaje\n");
		close(serverSocket);
		config_free();
		return EXIT_FAILURE;
	}
	char* mensaje = "";
	while(1){
		if(socket_recv_string(serverSocket, &mensaje)>0){
			printf("%s\n", mensaje);
		}else{
			printf("No se pudo recibir el mensaje\n");
			close(serverSocket);
			config_free();
			return EXIT_FAILURE;
		}
	}


	return EXIT_SUCCESS;
}
