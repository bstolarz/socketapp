#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <commons/config.h>
#include <commons/string.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <commons/config.h>
#include <commons/string.h>
#include <commons/temporal.h>

#include "../libSockets/client.h"
#include "../libSockets/recv.h"
#include "../libSockets/send.h"

#include "../functions/config.h"
#include "../commons/structures.h"
#include "../commons/declarations.h"

void thread_program_destroy(t_program* program, int insideThread){
	if(program->pid != 0){
		bool _buscarProgramaPID(t_program* auxProgram){
			return auxProgram->pid==program->pid;
		}
		list_remove_by_condition(programs, (void*)_buscarProgramaPID);
	}

	if(insideThread == 0){
		pthread_cancel(program->thread);
		log_info(logConsole,"[%i] - El programa fue abortado.", program->pid);
	}

	if(socket_send_string(program->socketKernel, "Finished")<=0){
		log_info(logConsole,"[%i] - El programa informa que fue finalizado desde la consola.", program->pid);
	}
	
	// BEGIN - Imprimo estadisticas
	printf(strcat("[SISTEMA] - Fecha inicio del programa: ", program->stats->stringInicioEjecucion));
	printf(strcat("[SISTEMA] - Fecha fin del programa: ", temporal_get_string_time()));
	printf(strcat("[SISTEMA] - Cantidad de impresiones en pantalla: ", program->stats->cantImpresionesPantalla));

	time_t finEjecucion;
	time(&finEjecucion);
	printf("[SISTEMA] - Duracion del programa: %i", finEjecucion - program->stats->timestampInicio);
	//END - Imprimo estadisticas
	
	close(program->socketKernel);
	free(program->pathProgram);
	free(program);
	return;
}

void* thread_program(void * params){
	t_program * program = params;
	printf("[SISTEMA] - Se inicio el programa: %s\n", program->pathProgram);
	log_info(logConsole,"[SISTEMA] - Se inicio el programa: %s", program->pathProgram);

	//Me conecto con el kernel
	socket_client_create(&(program->socketKernel), configConsole->ip_kernel, configConsole->puerto_kernel);
	if(program->socketKernel<=0){
		log_info(logConsole,"[SISTEMA] - El programa '%s' no logro conectarse con el Kernel.", program->pathProgram);
		thread_program_destroy(program, 1);
		return params;
	}

	//Le informo que soy un programa nuevo
	if(socket_send_string(program->socketKernel, "NewProgram")<=0){
		log_info(logConsole,"[SISTEMA] - El programa '%s' no pudo hacer el handshake con el Kernel.", program->pathProgram);
		thread_program_destroy(program, 1);
		return params;
	}

	if(socket_recv_int(program->socketKernel,&(program->pid))<=0){
		log_info(logConsole,"[SISTEMA] - El programa '%s' no pudo obtener un PID del kernel.", program->pathProgram);
		thread_program_destroy(program, 1);
		return params;
	}
	log_info(logConsole,"[%i] - Programa '%s' iniciado.", program->pid, program->pathProgram);
	list_add(programs, program);

	//Levanto el archivo  del programa
	char* code = 0;
	FILE *f = fopen(program->pathProgram, "rb");
	int fileSize;

	if (f){
	  fseek (f, 0, SEEK_END);
	  fileSize = ftell (f);

	  fseek (f, 0, SEEK_SET);
	  code = malloc(fileSize);

	  if (code){
	    fread (code, 1, fileSize, f);
	  }
	  fclose (f);
	}else{
		log_info(logConsole,"[%i] - El archivo '%s' no existe o no puede ser abierto.", program->pid, program->pathProgram);
		thread_program_destroy(program, 1);
		return params;
	}

	//Envio el archivo del programa
	if(socket_send(program->socketKernel, code, fileSize) != fileSize){
		log_info(logConsole,"[%i] - Fallo el envio del codigo fuente.", program->pid);
		thread_program_destroy(program, 1);
		return params;
	}


	char* printMessage = 0;
	while(1){
		if(socket_recv_string(program->socketKernel,&printMessage)<=0){
			log_info(logConsole,"[%i] - Fallo recepcion de mensaje.", program->pid);
			thread_program_destroy(program, 1);
			return params;
		}

		if(strcmp(printMessage, "FinEjecucion")==0){
			//TODO Aca va a ir la info al finalizar la ejecucion.
		}else{
			printf("[%i] - %s\n", program->pid, printMessage);
			free(printMessage);
		}
	}

	return params;
}
