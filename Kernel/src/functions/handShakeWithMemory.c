/*
 * handShakeWithMemory.c
 *
 *  Created on: 9/4/2017
 *      Author: utnso
 */
#include <stdio.h>
#include <stdlib.h>
#include "../commons/structures.h"
#include "../libSockets/send.h"
#include "../libSockets/recv.h"
int checkMemorySpace(int fileSize){
	int handShake;
	printf("About to check memory to socket %d\n",memorySocket);
	printf("File size: %d\n", fileSize);
	if (socket_send_string(memorySocket,"i")>0){
		socket_send_int(memorySocket,fileSize);
	};
	return socket_recv_int(memorySocket,&handShake);
}
void aumentarContadorDePaginas(t_pcb* PCB){
	PCB->cantPagsCodigo+=1;
}
int sendDataToMemory(t_pcb* PCB, char* path){
	printf("Path: %s\n",path);
	FILE *f;
	f=fopen(path,"r");
	fseek(f, 0L, SEEK_END);
	int fileSize=ftell(f);
	printf("%s ocupa %li bytes\n",path, ftell(f));
	if (checkMemorySpace(fileSize)){
		aumentarContadorDePaginas(PCB);
		return 1;
	}else{
		return -1;
	}
}


