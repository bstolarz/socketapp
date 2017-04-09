#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/config.h>
#include "commons/structures.h"
#include "commons/declarations.h"
#include "functions/config.h"
#include "libSockets/client.h"
#include "libSockets/send.h"
#include "libSockets/recv.h"

int hayMemoriaDisponible(int memorySize, int num){
	return memorySize>=num;
}

int main(int argc, char* argv[]){

	if (argc != 2){
		printf("Missing config path\n");
		return -1;
	}

	configMemory = malloc(sizeof(t_memory));
	config_read(argv[1]);
	config_print();
	int memorySize= (int)malloc(sizeof(configMemory->frameSize*configMemory->frameCount));
	t_invertedPageTable* table=(t_invertedPageTable*)malloc(sizeof(t_invertedPageTable));
//	memcpy(memorySize,table,sizeof(t_invertedPageTable));
	printf("The free space is: %d - %d= %d\n",memorySize, sizeof(t_invertedPageTable),memorySize-sizeof(t_invertedPageTable));
	printf("The memory size is: %d bytes\n",memorySize);
	int serverSocket=0;
	socket_client_create(&serverSocket, "127.0.0.1", "6667");
	if(serverSocket){
		socket_send_string(serverSocket, "MEM");
		char* mensaje = "";
		int num;
		while(1){
			if(socket_recv_string(serverSocket, &mensaje)>0){
				switch (mensaje[0]){
				//Recibir 'i' significa "iniciar un programa"
					case 'i':
						socket_recv_int(serverSocket,&num);
						printf("El tamanio del programa es %d bytes\n", num);
						usleep(configMemory->responseDelay*1000);
						if(hayMemoriaDisponible(memorySize,num)){
							printf("Hay espacio en memoria para ejecutar el programa\n");
							socket_send_int(serverSocket,1);
						}
				}
			}else{
				return -1;
			}
		}

	}
	return EXIT_SUCCESS;
}
