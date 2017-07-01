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

const char* ruta_programas = "../../programas-ejemplo/";
const char* programNames[] = {
	"facil",
	"for",
	"productor",
	"consumidor",
	"stackoverflow",
	"completo",
	"testfuncall",
	"heap",
	"fs_basico",
	"fs_cursor",
	"fs_leer",
	"fs_escribir",
	"fs_borrar",
	"copiarArchivo",
	"imprimirArchivo",
	"heap_a_archivo",
	"escribirFS",
	"leerFS",
};
size_t programsCount = sizeof(programNames) / sizeof(char*);

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

	char* programPath = NULL;

	size_t i;

	for (i = 0; i != programsCount; ++i)
	{
		if (strcmp(buffer, programNames[i]) == 0)
		{
			programPath = string_from_format("%s%s.ansisop", ruta_programas, programNames[i]);
			break;
		}
	}

	if (programPath == NULL)
		programPath = string_duplicate(buffer);
	
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
		program_create(string_from_format("%sfor.ansisop", ruta_programas));
		//sleep(3);
	}
}
