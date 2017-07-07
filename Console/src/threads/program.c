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
		log_info(logConsole,"[%i] - El programa fue abortado.", program->pid);

		if(socket_send_string(program->socketKernel, "Finished")<=0){
			log_info(logConsole,"[%i] - El programa informa que fue finalizado desde la consola.", program->pid);
		}

		pthread_cancel(program->thread);
	}
	
	// BEGIN - Imprimo estadisticas
	printf("[%i] - Fecha inicio del programa: %s\n", program->pid, program->stats->stringInicioEjecucion);
	printf("[%i] - Fecha fin del programa: %s\n", program->pid, temporal_get_string_time());
	printf("[%i] - Cantidad de impresiones en pantalla: %i\n", program->pid, program->stats->cantImpresionesPantalla);

	time_t finEjecucion;
	time(&finEjecucion);
	printf("[%i] - Duracion del programa: %i\n", program->pid, (int)(finEjecucion - program->stats->timestampInicio));
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
		free(code);
		return params;
	}
	free(code);

	char* printMessage = 0;
	while(1){
		if(socket_recv_string(program->socketKernel,&printMessage)<=0){
			printf("[%i] - Fallo recepcion de mensaje.\n", program->pid);
			thread_program_destroy(program, 1);
			return params;
		}

		if(strcmp(printMessage, "FinEjecucion")==0){
			int motivo;
			if(socket_recv_int(program->socketKernel,&motivo)<=0){
				printf("[%i] - Fallo recepcion del motivo.\n", program->pid);
				thread_program_destroy(program, 1);
				free(printMessage);
				return params;
			}
			switch(motivo){
				case 0:
					printf("[%i] - Motivo: Finalizo exitosamente.\n", program->pid);
					break;
				case -1:
					printf("[%i] - Motivo: No se pudieron reservar recursos para ejecutar el programa.\n", program->pid);
					break;
				case -2:
					printf("[%i] - Motivo: El programa intento leer un archivo que no existe.\n", program->pid);
					break;
				case -3:
					printf("[%i] - Motivo: El programa intento leer un archivo sin permisos.\n", program->pid);
					break;
				case -4:
					printf("[%i] - Motivo: El programa intento escribir un archivo sin permisos.\n", program->pid);
					break;
				case -5:
					printf("[%i] - Motivo: Excepcion de memoria.\n", program->pid);
					break;
				case -6:
					printf("[%i] - Motivo: Finalizado a travez de desconexion de consola.\n", program->pid);
					break;
				case -7:
					printf("[%i] - Motivo: Finalizado a travez del comando finalizar programa de la consola.\n", program->pid);
					break;
				case -8:
					printf("[%i] - Motivo: Se intento reservar mas memoria que el tamaño de una pagina.\n", program->pid);
					break;
				case -9:
					printf("[%i] - Motivo: No se pueden asignar mas paginas al proceso.\n", program->pid);
					break;
				case -10:
					printf("[%i] - Motivo: Se intento borrar un archivo abierto por varios procesos.\n", program->pid);
					break;
				case -11:
					printf("[%i] - Motivo: File descriptor inexistente.\n", program->pid);
					break;
				case -12:
					printf("[%i] - Motivo: Se intento abrir un archivo inexistente.\n", program->pid);
					break;
				case -13:
					printf("[%i] - Motivo: No se pudo borrar un archivo en FS.\n", program->pid);
					break;
				case -28:
					printf("[%i] - Motivo: No hay espacio libre en FS.\n", program->pid);
					break;
				case -14:
					printf("[%i] - Motivo: Semaforo inexistente.\n", program->pid);
					break;
				case -15:
					printf("[%i] - Motivo: Shared variable inexistente.\n", program->pid);
					break;
				case -16:
					printf("[%i] - Motivo: El cpu se desconecto y dejo el programa en un estado inconsistente.\n", program->pid);
					break;
				case -17:
					printf("[%i] - Motivo: Finalizado a travez del comando finalizar programa de la consola del kernel.\n", program->pid);
					break;
				case -20:
					printf("[%i] - Motivo: Error sin definicion.\n", program->pid);
					break;
			}
			thread_program_destroy(program, 1);
			return params;
		}else if(strcmp(printMessage, "imprimir")==0){
			if(socket_recv_string(program->socketKernel,&printMessage)<=0){
				printf("[%i] - Fallo recepcion de mensaje a imprimir.\n", program->pid);
				thread_program_destroy(program, 1);
				free(printMessage);
				return params;
			}

			program->stats->cantImpresionesPantalla++;
			printf("[%i] - Imprimo: %s\n", program->pid, printMessage);
			free(printMessage);
		}else{
			printf("[%i] - No se entendio el mensaje: %s\n", program->pid, printMessage);
			thread_program_destroy(program, 1);
			free(printMessage);
			return params;
		}
	}

	return params;
}
