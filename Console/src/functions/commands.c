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

char* ruta_programas = "../../programas-ejemplo/";

void program_create(char* path_copy){
	t_program * program = malloc(sizeof(t_program));
	program->pathProgram = path_copy;
	program->pid = 0;

	program->stats = malloc(sizeof(t_statistics));
	program->stats->stringInicioEjecucion = temporal_get_string_time();
	struct timeval tv;
	gettimeofday(&tv, NULL);
	program->stats->timestampInicio = tv.tv_sec;
	program->stats->cantImpresionesPantalla = 0;

	pthread_create(&(program->thread),NULL,thread_program, (void*)program);
}

char* input_program_path()
{
	printf("[SISTEMA] - Ingrese el path al archivo o el accesos directo al mismo:\n");

	size_t cantidad = 200;
	char* buffer = malloc(sizeof(char)*cantidad);

	size_t cantLeida = getline(&buffer, &cantidad, stdin);
	buffer[cantLeida-1]='\0';

	char* programPath = string_new();

	if(strcmp(buffer, "facil")==0){
		string_append(&programPath, "../../programas-ejemplo/facil.ansisop");
	}else if(strcmp(buffer, "for")==0){
		string_append(&programPath, "../../programas-ejemplo/for.ansisop");
	}else if(strcmp(buffer, "productor")==0){
		string_append(&programPath, "../../programas-ejemplo/productor.ansisop");
	}else if(strcmp(buffer, "stackoverflow")==0){
		string_append(&programPath, "../../programas-ejemplo/stackoverflow.ansisop");
	}else if(strcmp(buffer, "completo")==0){
		string_append(&programPath, "../../programas-ejemplo/completo.ansisop");
	}else if(strcmp(buffer, "consumidor")==0){
		string_append(&programPath, "../../programas-ejemplo/consumidor.ansisop");
	}else{
		string_append(&programPath, buffer);
	}
	
	free(buffer);

	return programPath;
}

void command_start(){
	char* path = input_program_path();
	printf("path = %s\n", path);
	program_create(path);

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

void command_start_several()
{
	int i;

	for (i = 0; i != 5; ++i)
	{
		program_create(string_from_format("%sfor%s", ruta_programas, ".ansisop"));
		sleep(3);
	}
}
