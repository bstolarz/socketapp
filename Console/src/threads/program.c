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

#include "../libSockets/client.h"
#include "../libSockets/recv.h"
#include "../libSockets/send.h"

#include "../functions/config.h"
#include "../commons/structures.h"
#include "../commons/declarations.h"

void* thread_program(void * params){
	t_program * program = params;
	printf("Thread creado con %s\n", program->pathProgram);

	//Me conecto con el kernel
	socket_client_create(&(program->socketKernel), configConsole->ip_kernel, configConsole->puerto_kernel);
	if(program->socketKernel<=0){
		log_info(logConsole,"El programa '%s' no logro conectarse con el Kernel.\n", program->pathProgram);
		close(program->socketKernel);
		return params;
	}

	//Le informo que soy un programa nuevo
	if(socket_send_string(program->socketKernel, "NewProgram")<=0){
		log_info(logConsole,"El programa '%s' no pudo hacer el handshake con el Kernel.\n", program->pathProgram);
		close(program->socketKernel);
		return params;
	}

	int pid=0;
	if(socket_recv_int(program->socketKernel,&pid)<=0){
		log_info(logConsole,"El programa '%s' no pudo obtener un PID del kernel.\n", program->pathProgram);
		close(program->socketKernel);
		return params;
	}
	program->pid=pid;
	log_info(logConsole,"Se inicio e l programa '%i'.\n", program->pid);

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
		log_info(logConsole,"El archivo '%s' no existe o no puede ser abierto.\n", program->pathProgram);
		close(program->socketKernel);
		return params;
	}

	//Envio el archivo del programa
	if(socket_send(program->socketKernel, code, fileSize) != fileSize){
		log_info(logConsole,"El programa '%i' no pudo enviar su codigo al kernel.\n", program->pid);
		close(program->socketKernel);
		return params;
	}


	return params;
}
