#include <sys/types.h>
 #include <sys/socket.h>

int socket_send(int clientSocket, void* parameter, int size){
	char* sizeStr=malloc(sizeof(char)*11);
	sprintf(sizeStr,"%i",size);
	if(send(clientSocket, sizeStr, 11, 0) == 11){
		if(send(clientSocket, parameter, size, 0) == size){
			free(sizeStr);
			return 1;
		}else{
			free(sizeStr);
		}
	}else{
		free(sizeStr);
	}
	return 0;
}
