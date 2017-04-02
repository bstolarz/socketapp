#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdint.h>
#include <pthread.h>

int socket_recv(int clientSocket, void** buffer, int reserveSpace){
	char* sizeStr = malloc(sizeof(char)*11);
	if (recv(clientSocket, sizeStr, 11,  0) == 11){
		int size = atoi(sizeStr);
		free(sizeStr);

		if(reserveSpace){
			*buffer = malloc(size);
		}

		int magicSocketNumber = 32768;
		char* bufAuxiliar = malloc(size);

		if(size <= magicSocketNumber){
			if(recv(clientSocket, bufAuxiliar, size, 0)==size){
				memcpy(*buffer, bufAuxiliar, size);
				free(bufAuxiliar);
				return size;
			}

			free(bufAuxiliar);
			return -1;
		}else{
			int desplazamientoBuffer = 0;

			div_t divisionEnPartes = div(size, magicSocketNumber);
			int nPartes = divisionEnPartes.quot;
			int resto = divisionEnPartes.rem;

			int i;
			for(i=0 ; i<nPartes; i++){
				char* bufferMagicNumber = malloc(magicSocketNumber*sizeof(char));

				if(recv(clientSocket, bufferMagicNumber, magicSocketNumber, 0) == magicSocketNumber){
					memcpy(bufAuxiliar+desplazamientoBuffer, bufferMagicNumber, magicSocketNumber);
					desplazamientoBuffer += magicSocketNumber;

					free(bufferMagicNumber);
				}else{
					free(bufferMagicNumber);
					return -1;
				}
			}

			if(resto > 0){
				char* bufferResto = malloc(resto*sizeof(char));
				if(recv(clientSocket, bufferResto, resto, 0) == resto){
					memcpy(bufAuxiliar+desplazamientoBuffer, bufferResto, resto);
					free(bufferResto);
				}else{
					free(bufferResto);
					return -1;
				}
			}
			memcpy(*buffer, bufAuxiliar, size);
			free(bufAuxiliar);
			return size;
		}

	}else{
		free(sizeStr);
		return -1;
	}


}

int socket_recv_string(int clientSocket, char* text){
	return socket_recv(clientSocket, *text, 0);
}

int socket_recv_int(int clientSocket, int* value){
	return socket_recv(clientSocket, &value, 0);
}

