#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <stdbool.h>
#include "string.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>
#include <commons/string.h>
#include "../commons/structures.h"
#include "../threads/program.h"

void command_start(){
	printf("Ingrese el path al archivo:\n");

	size_t cantidad = 200;
	char* path = malloc(sizeof(char)*cantidad);

	size_t cantLeida = getline(&path, &cantidad, stdin);
	path[cantLeida-1]='\0';

	t_program * programa = malloc(sizeof(t_program));
	programa->pathProgram = string_duplicate(path);
	free(path);

	pthread_create(&(programa->thread),NULL,thread_program, (void*)programa );
}

void command_finish(){
	printf("Se ingreso: finalizar\n");
}

void command_clear(){
	printf("Se ingreso: clear\n");
}

void command_disconnect(){
	printf("Se ingreso: desconectar\n");
}
