#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//#include <commons/string.h>
//#include <commons/log.h>

#include "commons/structures.h"
#include "commons/declarations.h"

#include "functions/config.h"
#include "functions/log.h"
#include "functions/commands.h"
#include "threads/program.h"

int main(int argc, char* argv[]) {
	printf("El nombre del log es: logConsole%d\n", getpid());

	if(argc!=2){
		printf("Missing config path\n");
		return -1;
	}

	configConsole = malloc(sizeof(t_console));
	config_read(argv[1]);
	//config_read("/home/utnso/git/tp-2017-1c-SocketApp/console");

	logConsole = log_create_file();
	log_config();

	programs = list_create();

	size_t cantidad = 50;
	char* comando = malloc(sizeof(char)*cantidad);

	while(1){
		printf("[SISTEMA] - Ingrese un comando:\n");
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
			printf("[SISTEMA] - El comando ingresado no existe.\n");
			printf("[SISTEMA] - Los comandos permitidos son:\n");
			printf("[SISTEMA] - 	start			Permite ejecutar un nuevo programa.\n");
			printf("[SISTEMA] - 	finish			Aborta la ejecucion de un programa.\n");
			printf("[SISTEMA] - 	clear 			Borra la  informacion en pantalla.\n");
			printf("[SISTEMA] - 	disconnect 		Aborta la ejecucion de todos los programas.\n");

		}
	}

	return EXIT_SUCCESS;
}
