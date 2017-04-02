#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>

int socket_recv(int clientSocket, void* buffer){
	char* sizeStr = malloc(sizeof(char)*11);
	if (recv(clientSocket, sizeStr, 11,  0) == 11){
		int size = atoi(sizeStr);
		free(sizeStr);

		int magicSocketNumber = 32768;
		char* bufAuxiliar = malloc(size);

		if(size <= magicSocketNumber){
			recv(clientSocket, bufAuxiliar, size, 0);
			memcpy(buffer, bufAuxiliar, size);
			free(bufAuxiliar);
		}else{
			int desplazamientoBuffer = 0;

			div_t divisionEnPartes = div(size, magicSocketNumber);
			int nPartes = divisionEnPartes.quot;
			int resto = divisionEnPartes.rem;

			int i;
			for(i=0 ; i<nPartes; i++){
				char* bufferMagicNumber = malloc(magicSocketNumber*sizeof(char));

				recv(clientSocket, bufferMagicNumber, magicSocketNumber, 0);
				memcpy(bufAuxiliar+desplazamientoBuffer, bufferMagicNumber, magicSocketNumber);
				desplazamientoBuffer += magicSocketNumber;

				free(bufferMagicNumber);
			}

			if(resto > 0){
				char* bufferResto = malloc(resto*sizeof(char));
				recv(clientSocket, bufferResto, resto, 0);
				memcpy(bufAuxiliar+desplazamientoBuffer, bufferResto, resto);
				free(bufferResto);
			}
			memcpy(buffer, bufAuxiliar, size);
		}

	}else{
		free(sizeStr);
	}

	return 1;
}

