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
#include <parser/metadata_program.h>

#include "../../libSockets/send.h"
#include "../../libSockets/recv.h"

#include "../../commons/structures.h"
#include "../../commons/declarations.h"
#include "../../functions/ltp.h"

int program_generate_id(){
	programID++;
	return programID;
}

void program_process_new(fd_set* master, int socket){
	t_program * program = malloc(sizeof(t_program));
	program->socket = socket;
	program->interruptionCode = 0;

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

	if((program->codeSize = socket_recv(program->socket, &(program->code), 1))<=0){
		log_info(logKernel,"No se pudo conectar con el programa %i para obtener su codigo\n", program->pcb->pid);
		FD_CLR(program->socket, master);
		close(program->socket);
		free(program->pcb);
		free(program);
		return;
	}

	t_metadata_program* metadata = metadata_desde_literal(program->code);
	program->pcb->pc = metadata->instruccion_inicio;
	program->pcb->indiceDeCodigoCant = metadata->instrucciones_size;
	program->pcb->indiceDeCodigo = metadata->instrucciones_serializado;
	program->pcb->indiceDeEtiquetasCant = metadata->etiquetas_size;
	program->pcb->indiceDeEtiquetas = metadata->etiquetas;

	//CODIGO IMPRIMIR EL INDICE DE ETIQUETAS
	/*int i;
	for(i=0; i<metadata->etiquetas_size; i++){
		printf("Etiqueta: %s\n", metadata->etiquetas[i]);
	}*/

	//CODIGO IMPRIMIR EL INDICE DE CODIGO
	/*
	int i;
	for(i=0; i<metadata->instrucciones_size; i++){
		t_intructions instr = metadata->instrucciones_serializado[i];
		printf("Start: %i, Offset: %i\n", instr.start, instr.offset);
	}
	*/

	//CODIGO PARA TESTEAR EL ENVIO DEL PROGRAMA
	/*
	int i=0;
	for(i=0; i<program->codeSize; i++){
		printf("%c", ((char*)program->code)[i]);
	}
	printf("\n");
	*/

	printf("Se agrego a %i a la lista de programas nuevos\n", program->pcb->pid);
	log_info(logKernel,"Se agrego a %i a la lista de programas", program->pcb->pid);

	pthread_mutex_lock(&(queueNewPrograms->mutex));
	list_add(queueNewPrograms->list, program);
	pthread_mutex_unlock(&(queueNewPrograms->mutex));

	planificadorLargoPlazo();

	return;
}

void program_finish(t_program* program){
	pthread_mutex_lock(&(queueFinishedpPrograms->mutex));

	//TODO
	/*
	 * Aca deberiamos:
	 * 		Enviar info estadistico
	 * 		Informar del fin de ejecucion y su exitCode
	 * 		Cerrar el socket
	 * 		Dejar el programa finalizado.
	 */

	list_add(queueFinishedpPrograms->list, program);
	pthread_mutex_unlock(&(queueFinishedpPrograms->mutex));
}

void program_interrup(int socket, int interruptionCode, int overrideInterruption){
	bool _buscarProgramaSocket(t_program* programa){
		return programa->socket==socket;
	}

	bool _buscarProgramaSocketInCPUs(t_cpu* cpu){
		return cpu->program->socket==socket;
	}

	int programaEncontrado = 0;

	//Lockeo la lista de cpus para que no pueda salir de la ejecucion de un cpu y pasarme por alto.
	pthread_mutex_lock(&(queueCPUs->mutex));


	//Reviso los cpus para ver si  el programa  esta ejecutando
	t_cpu* cpu = list_find(queueCPUs->list, (void*)_buscarProgramaSocketInCPUs);
	if(cpu != NULL){
		log_info(logKernel, "El programa %i fue encontrado en la lista de cpus y se le puso el interruption code: %i.", cpu->program->pcb->pid, interruptionCode);
		printf("El programa %i fue encontrado en la lista de cpus y se le puso el interruption code: %i\n", cpu->program->pcb->pid, interruptionCode);
		programaEncontrado = 1;
		if(cpu->program->interruptionCode == 0 || overrideInterruption == 1){
			cpu->program->interruptionCode = interruptionCode;
		}
		//TODO enviar interrupcion al cpu para que este lo devuelva con el error y se procese la muerte con la funcion 'program_finish'
	}

	//Reviso la cola de nuevos
	if(programaEncontrado == 0){
		pthread_mutex_lock(&(queueNewPrograms->mutex));
		t_program* program = list_remove_by_condition(queueNewPrograms->list, (void*)_buscarProgramaSocket);
		if(program != NULL){
			log_info(logKernel, "El programa %i fue encontrado en la lista de nuevos y se le puso el interruption code: %i.", program->pcb->pid, interruptionCode);
			printf("El programa %i fue encontrado en la lista de nuevos y se le puso el interruption code: %i\n", program->pcb->pid, interruptionCode);
			programaEncontrado = 1;
			program->pcb->exitCode = interruptionCode;
			program_finish(program);
		}
		pthread_mutex_unlock(&(queueNewPrograms->mutex));
	}

	//Reviso la cola de listos
	if(programaEncontrado == 0){
		pthread_mutex_lock(&(queueReadyPrograms->mutex));
		t_program* program = list_remove_by_condition(queueReadyPrograms->list, (void*)_buscarProgramaSocket);
		if(program != NULL){
			log_info(logKernel, "El programa %i fue encontrado en la lista de listos y se le puso el interruption code: %i.", program->pcb->pid, interruptionCode);
			printf("El programa %i fue encontrado en la lista de listos y se le puso el interruption code: %i\n", program->pcb->pid, interruptionCode);
			programaEncontrado = 1;
			program->pcb->exitCode = interruptionCode;
			program_finish(program);
		}
		pthread_mutex_unlock(&(queueReadyPrograms->mutex));
	}

	//Deslockeo los cpus
	pthread_mutex_unlock(&(queueCPUs->mutex));
}
