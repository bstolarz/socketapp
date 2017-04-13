#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>
#include <sys/socket.h>

#include <commons/collections/list.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/log.h>

#include "../../libSockets/send.h"
#include "../../libSockets/recv.h"

#include "../../commons/structures.h"
#include "../../commons/declarations.h"

int program_generate_id(){
	programID++;
	return programID;
}

void program_process_new(fd_set* master, int socket){
	t_program * program = malloc(sizeof(program));
	program->socket = socket;

	program->pcb = malloc(sizeof(t_pcb));

	program->pcb->pid=program_generate_id();
	program->pcb->pc=0;
	program->pcb->processFileTable=dictionary_create();
	program->pcb->stackPosition=0;
	program->pcb->cantPagsCodigo=0;

	if(socket_send_int(program->socket, program->pcb->pid)<=0){
		log_info(logKernel,"No se pudo conectar con el programa %i para informarle su PID\n", program->pcb->pid);
		FD_CLR(program->socket, master);
		close(program->socket);
		free(program->pcb);
		free(program);
		return;
	}

	char * code = 0;
	int fileSize;
	if((fileSize=socket_recv(program->socket, (void**)&code, 1))<=0){
		log_info(logKernel,"No se pudo conectar con el programa %i para obtener su codigo\n", program->pcb->pid);
		FD_CLR(program->socket, master);
		close(program->socket);
		free(program->pcb);
		free(program);
		return;
	}

	/* CODIGO PARA TESTEAR EL ENVIO DEL PROGRAMA
	 * int i=0;
	 * for(i=0; i<fileSize; i++){
	 * 	printf("%c", code[i]);
	 * }
	 * printf("\n");
	 */

	printf("Se agrego a %i a la lista de programas\n", program->pcb->pid);

	list_add(listNewPrograms, program);
	return;
}
