/*
 * hiloPrograma.c
 *
 *  Created on: 9/4/2017
 *      Author: utnso
 */

#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../libSockets/server.h"
#include "../libSockets/recv.h"
#include "../libSockets/send.h"
#include <sys/socket.h>
#include <commons/config.h>
#include <commons/string.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <commons/config.h>
#include <commons/string.h>
#include "config.h"

int finalizarPrograma(char* ansisop){
	return EXIT_SUCCESS;
}
int iniciarProgramaAnsisop(FILE* f, char* ansisop, int serverSocket){
	char c;
	while (feof(f)==0){

					c=fgetc(f);
					//printf("%c",c);
					strncat(ansisop,&c,1);
				}
				ansisop[strlen(ansisop)-1]='\0';
				fseek(f, 0L, SEEK_END);
				int fileSize=ftell(f);
//				printf("%s",ansisop);

				if(socket_send_string(serverSocket, "CON")<=0){
					printf("No se pudo enviar el mensaje 'CON' \n");
					close(serverSocket);
					config_free();
					return EXIT_FAILURE;
				}
				if (socket_send_int(serverSocket,fileSize)<=0){
					printf("No se pudo enviar el mensaje con el tamanio del programa ANSISOP\n");
					close(serverSocket);
					config_free();
					return EXIT_FAILURE;
				}
				if(socket_send_string(serverSocket, ansisop)<=0){
					printf("No se pudo enviar el mensaje con el contenido del programa ANSISOP\n");
					close(serverSocket);
					config_free();
					return EXIT_FAILURE;
				}
return EXIT_SUCCESS;
}
