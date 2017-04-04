#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include "commons/structures.h"
#include "commons/declarations.h"
#include "functions/config.h"
#include "libSockets/client.h"
#include "libSockets/send.h"
#include "libSockets/recv.h"

int main(int argc, char* argv[]){

	if (argc != 2){
		printf("Missing config path\n");
		return -1;
	}

	configMemory = malloc(sizeof(t_memory));
	config_read(argv[1]);
	config_print();

	int serverSocket=0;
	socket_client_create(&serverSocket, "127.0.0.1", "6667");
	if(serverSocket){
		socket_send_string(serverSocket, "MEM");
		char* mensaje = "";
		while(1){
			socket_recv_string(serverSocket, &mensaje);
			printf("%s\n", mensaje);
		}
	}

	return EXIT_SUCCESS;
}
