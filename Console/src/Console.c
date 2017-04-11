/*
 ============================================================================
 Name        : Console.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include "functions/hiloPrograma.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <commons/config.h>
#include <commons/string.h>
#include "commons/structures.h"
#include "commons/declarations.h"
#include "functions/config.h"
#include "libSockets/client.h"
#include "libSockets/send.h"
#include "libSockets/recv.h"
#include <commons/log.h>


int main(int argc, char* argv[]) {
	logConsole=log_create("logConsole.txt","Console",false,LOG_LEVEL_DEBUG);
	if(argc!=2){
		printf("Missing config path\n");
		return -1;
	}

	configConsole = malloc(sizeof(t_console));
	config_read(argv[1]);
	//config_read("/home/utnso/git/tp-2017-1c-SocketApp/console");
	config_print();


	int serverSocket=0;
	socket_client_create(&serverSocket, configConsole->ip_kernel, configConsole->puerto_kernel);
	if(serverSocket<=0){
		log_info(logConsole,"No logre conectarme al server\n");
		close(serverSocket);
		config_free();
		return EXIT_FAILURE;
	}
	size_t cantidad = 50;
	char *str=malloc(sizeof(char)*cantidad);
	while(1){
		log_info(logConsole,"Esperando comando\n");
		//printf("Ingrese un mensaje:\n");
		size_t cantLeida = getline(&str, &cantidad, stdin);
		char* path=malloc(sizeof(char)*cantLeida);
		str[cantLeida-1]='\0';
		char* ansisop=string_new();
		switch(str[0]){
			case 'c':
				log_info(logConsole,"Clean Screen\n");
				system("clear");
				break;
			case 'i':
				path=string_duplicate(string_substring_from(str,2));
				log_info(logConsole,"Se pide iniciar programa con path %s\n",path);
				//printf("Path: %s",path);
				FILE* f=fopen(path,"r");
				log_info(logConsole, "Se abre el archivo %s\n", path);
				if (f==NULL){
					log_info(logConsole,"Error abriendo archivos\n");
				}
				if(iniciarProgramaAnsisop(f,ansisop, serverSocket)==EXIT_SUCCESS){
					log_info(logConsole,"Iniciar hilo programa para el programa ubicado en %s\n",path);
				};
				break;
			case 'f':
				log_info(logConsole,"Se pide finalizar el programa ubicado en %s\n",path);
				path=string_duplicate(string_substring_from(str,2));
				finalizarPrograma(path);
				break;
		}
	}
	return EXIT_SUCCESS;
}
