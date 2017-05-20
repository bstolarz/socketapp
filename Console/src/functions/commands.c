#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
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
#include <signal.h>
#include <commons/string.h>
#include <commons/temporal.h>
#include "../commons/structures.h"
#include "../commons/declarations.h"
#include "../threads/program.h"

void command_start(){
	printf("[SISTEMA] - Ingrese el path al archivo o el accesos directo al mismo:\n");

	size_t cantidad = 200;
	char* path = malloc(sizeof(char)*cantidad);

	size_t cantLeida = getline(&path, &cantidad, stdin);
	path[cantLeida-1]='\0';

	t_program * program = malloc(sizeof(t_program));
	program->pathProgram = string_new();
	program->pid = 0;

	if(strcmp(path, "facil")==0){
		string_append(&program->pathProgram, "../../programas-ejemplo/facil.ansisop");
	}else if(strcmp(path, "for")==0){
		string_append(&program->pathProgram, "../../programas-ejemplo/for.ansisop");
	}else if(strcmp(path, "productor")==0){
		string_append(&program->pathProgram, "../../programas-ejemplo/productor.ansisop");
	}else if(strcmp(path, "stackoverflow")==0){
		string_append(&program->pathProgram, "../../programas-ejemplo/stackoverflow.ansisop");
	}else if(strcmp(path, "completo")==0){
		string_append(&program->pathProgram, "../../programas-ejemplo/completo.ansisop");
	}else if(strcmp(path, "consumidor")==0){
		string_append(&program->pathProgram, "../../programas-ejemplo/consumidor.ansisop");
	}else{
		string_append(&program->pathProgram, path);
	}
	
	program->stats = malloc(sizeof(t_statistics));
	program->stats->stringInicioEjecucion = temporal_get_string_time();
	struct timeval tv;
	gettimeofday(&tv, NULL);
	program->stats->timestampInicio = tv.tv_sec;
	program->stats->cantImpresionesPantalla = 0;
	
	free(path);

	pthread_create(&(program->thread),NULL,thread_program, (void*)program);

	return;
}

void command_finish(){
	printf("[SISTEMA] - Ingrese el PID del programa:\n");
	size_t cantidad = 50;
	char* pidS = malloc(sizeof(char)*cantidad);

	size_t cantLeida = getline(&pidS, &cantidad, stdin);
	pidS[cantLeida-1]='\0';

	int pid = atoi(pidS);

	bool _buscarProgramaPID(t_program* programa){
		return programa->pid==pid;
	}
	t_program* program = list_remove_by_condition(programs, (void*)_buscarProgramaPID);

	if(program != NULL){
		thread_program_destroy(program, 0);
	}else{
		printf("[SISTEMA] - El PID ingresado no corresponde con ningun  programa.\n");
	}

	return;
}

void command_clear(){
	system("clear");
}

void command_disconnect(){
	log_info(logConsole,"[SISTEMA] - Todos los programas seran abortados.");

	void _destroyProgram(t_program* program){
		thread_program_destroy(program, 0);
	}
	list_clean_and_destroy_elements(programs, (void*)_destroyProgram);
	return;
}

