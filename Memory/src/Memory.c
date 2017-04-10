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
#include "functions/memory.h"


int main(int argc, char* argv[]){

	if (argc != 2){
		printf("Missing config path\n");
		return -1;
	}

	memoryLog = log_create("memory_log.txt", "Memory", false, LOG_LEVEL_DEBUG);

	// config init
	configMemory = malloc(sizeof(t_memory));
	config_read(argv[1]);
	config_print();

	memory_init();
	
	// test_program_init_end();
	// test_read_write();

	// return 0;

	int serverSocket=0;
	socket_client_create(&serverSocket, "127.0.0.1", "6667");
	
	if(serverSocket){
		socket_send_string(serverSocket, "MEM");
		char* mensaje = "";

		while(1) {
			if(socket_recv_string(serverSocket, &mensaje)>0) {

				int programByteSize;
				switch (mensaje[0]){
				//Recibir 'i' significa "iniciar un programa"
					case 'i':

						socket_recv_int(serverSocket, &programByteSize);
						printf("El tamanio del programa es %d bytes\n", programByteSize);

						// usleep(configMemory->responseDelay * 1000);

						int* programPages = get_continguous_frames(bytes_to_pages(programByteSize));

						if (programPages != NULL) {
							printf("Hay espacio en memoria para ejecutar el programa\n");
							socket_send_int(serverSocket,1);
						}
						printf("inicializar progrma");
						socket_send_int(serverSocket,1);

						free(programPages);
						break;

						
				}
			}else{
				log_destroy(memoryLog);
				return -1;
			}
		}

	}
	
	log_destroy(memoryLog);
	return EXIT_SUCCESS;
}
