#include <stdio.h>
#include <stdlib.h>
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
		//return 1;
	}

	configFileSystem=malloc(sizeof(t_fileSystem));
	config_read("/home/utnso/git/");
	config_print();

	int serverSocket=0;
	socket_client_create(&serverSocket, "127.0.0.1", "6667");
	if(serverSocket){
		socket_send_string(serverSocket, "FSY");
		char* mensaje = "";
		while(1){
			if(socket_recv_string(serverSocket, &mensaje)>0){
				printf("%s\n", mensaje);
			}else{
				return -1;
			}
		}
	}

	return EXIT_SUCCESS;
}
