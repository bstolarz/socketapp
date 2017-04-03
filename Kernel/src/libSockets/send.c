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
#include <commons/string.h>

int socket_send(int clientSocket, void* parameter, int size){
	char* sizeStr=malloc(sizeof(char)*11);
	sprintf(sizeStr,"%i",size);
	if(send(clientSocket, sizeStr, 11, 0) == 11){
		if(send(clientSocket, parameter, size, 0) == size){
			free(sizeStr);
			return size;
		}else{
			free(sizeStr);
		}
	}else{
		free(sizeStr);
	}
	return -1;
}

int socket_send_string(int clientSocket, char* parameter){
	return socket_send(clientSocket, parameter, string_length(parameter));
}

int socket_send_int(int clientSocket, int value){
	char* sizeStr=malloc(sizeof(char)*11);
	sprintf(sizeStr,"%i",value);
	if(send(clientSocket, sizeStr, 11, 0) == 11){
		free(sizeStr);
		return sizeof(int);
	}else{
		free(sizeStr);
	}
	return -1;
}
