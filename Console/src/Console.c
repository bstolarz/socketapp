#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <commons/config.h>
#include <commons/string.h>
#include <commons/log.h>

#include "commons/structures.h"
#include "commons/declarations.h"

#include "functions/config.h"
#include "functions/commands.h"
#include "threads/program.h"

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

	programs = list_create();

	size_t cantidad = 50;
	char* comando = malloc(sizeof(char)*cantidad);

	while(1){
		printf("Ingrese un comando:\n");
		size_t cantLeida = getline(&comando, &cantidad, stdin);
		comando[cantLeida-1]='\0';

		if(strcmp(comando, "clear") == 0){
			command_clear();
		}else if(strcmp(comando, "start") == 0){\
			command_start();
		}else if(strcmp(comando, "finish") == 0){
			command_finish();
		}else if(strcmp(comando, "disconnect") == 0){
			command_disconnect();
		}else{
			printf("El comando ingresado no existe.\n");
			printf("Los comandos permitidos son:\n");
			printf("	start			Permite ejecutar un nuevo programa.\n");
			printf("	finish			Aborta la ejecucion de un programa.\n");
			printf("	clear 			Borra la  informacion en pantalla.\n");
			printf("	disconnect 		Aborta la ejecucion de todos los programas.\n");

		}

		/*switch(str[0]){
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
		}*/
	}
	return EXIT_SUCCESS;
}
