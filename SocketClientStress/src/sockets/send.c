#include <sys/types.h>
 #include <sys/socket.h>

int recvBufferParaWrite(int clientSocket, void* buffer, int size){

	char* bufAuxiliar = malloc(size);
	if(size <= 32768){
		recv(clientSocket, bufAuxiliar, size, 0);
		memcpy(buffer, bufAuxiliar, size);
		free(bufAuxiliar);
	}else{
		div_t divisionEnPartes = div(size, 32768);
		int vecesQueHayQueHacerRecv = divisionEnPartes.quot;

		int i;
		int desplazam = 0;
		for(i=0 ; i<vecesQueHayQueHacerRecv; i++){
			char* bufferDe32kb = malloc(32768*sizeof(char));
			recv(clientSocket, bufferDe32kb, 32768, 0);
			memcpy(bufAuxiliar+desplazam, bufferDe32kb, 32768);
			desplazam += 32768;

			free(bufferDe32kb);
		}
		if(divisionEnPartes.rem > 0){
			char* bufferDelRestoDeBytes = malloc((divisionEnPartes.rem)*sizeof(char));
			recv(clientSocket, bufferDelRestoDeBytes, divisionEnPartes.rem, 0);
			memcpy(bufAuxiliar+desplazam, bufferDelRestoDeBytes, divisionEnPartes.rem);
			free(bufferDelRestoDeBytes);
		}
		memcpy(buffer, bufAuxiliar, size);
	}

	return 1;
}
