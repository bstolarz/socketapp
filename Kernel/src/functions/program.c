#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <stdbool.h>

#include "../libSockets/server.h"
#include "../libSockets/recv.h"
#include "../libSockets/send.h"

#include "../commons/structures.h"
#include "../commons/declarations.h"
#include "../commons/error_codes.h"

#include "../interface/memory.h"
#include "../planner/ltp.h"
#include "../functions/cpu.h"

int program_generate_id(){
	programID++;
	return programID;
}

void program_process_new(fd_set* master, int socket){
	t_program * program = malloc(sizeof(t_program));
	program->socket = socket;
	program->interruptionCode = 0;
	program->waiting=0;
	program->waitingReason = string_new();
	program->fileDescriptors =  list_create();
	program->incrementalFD = 3;
	program->heapPages = list_create();
	program->quantum = 0;

	program->stats.syscallEjecutadas=0;
	program->stats.rafagas=0;
	program->stats.pagesAlloc=0;
	program->stats.pagesFree=0;
	program->stats.syscallPrivilegiadas=0;
	program->pcb = malloc(sizeof(t_pcb));

	program->pcb->pid=program_generate_id();
	program->pcb->pc=0;

	program->pcb->processFileTable=dictionary_create();
	program->pcb->stackPosition=0;
	program->pcb->cantPagsCodigo=0;
	program->pcb->exitCode = 1;

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

	// crear el stack
	t_list* stack = list_create();

	// crear el contexto del main del programa
	t_indiceDelStack* mainContext = malloc(sizeof(t_indiceDelStack));
	mainContext->argCount = 0;
	mainContext->vars = dictionary_create();
	mainContext->retPos = -1;
	mainContext->retVar = NULL;

	list_add(stack, mainContext);

	program->pcb->indiceDeStack = stack;

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

	pthread_mutex_lock(&(queueNewPrograms->mutex));
	list_add(queueNewPrograms->list, program);
	pthread_mutex_unlock(&(queueNewPrograms->mutex));

	log_info(logKernel,"Se agrego a %i a la lista de programas", program->pcb->pid);

	planificador_largo_plazo();
	cpu_inactive_planner(1);

	return;
}

int program_to_ready(t_program* program){

	//Obtengo el frame size
	int frameSize = memory_frame_size();
	if(frameSize<=0){
		log_error(logKernel, "[program_to_ready] no pude obtener el frame size");
		return -20;
	}


	//Calculo la cantidad de paginas del codigo
	program->pcb->cantPagsCodigo = program->codeSize / frameSize;
	if((program->pcb->cantPagsCodigo * frameSize) < program->codeSize){
		program->pcb->cantPagsCodigo++;
	}

	// ya que aca tengo a mano el frameSize, seteo el max offset del stack
	program->pcb->maxStackPosition = configKernel->stack_size * frameSize;

	//Calculo cantidad de paginas a solicitar
	int paginasTotales = program->pcb->cantPagsCodigo + configKernel->stack_size;

	//Solicito las paginas
	int respuestaInit;
	if((respuestaInit = memory_init(program, paginasTotales)) != 0){
		return respuestaInit;
	}

	//Escribo en memoria
	int respuestaWrite;
	if((respuestaWrite = memory_write(program, 0, 0, program->code, program->codeSize)) != program->codeSize){
		return respuestaWrite;
	}

	return 0;
}
/*int find_file_on_global_fd(t_fd* f){
	int size=list_size(globalFileDescriptors->list);
	int i;
	for (i=0;i<size;i++){
		t_fd* fileToCheck=(t_fd*)list_get(globalFileDescriptors->list,i);
		if (fileToCheck->value==f->value){
			return i;
		}
	}
	return -1;
}*/
bool fileIsNotOpenAnyMore(t_global_fd* f){
	return f->open==0;
}
void deleteFileFromProcessFileTable(t_fd* f){
	pthread_mutex_lock(&(globalFileDescriptors->mutex));
	f->global->open--;
	t_global_fd* fileToRemove=list_remove_by_condition(globalFileDescriptors->list,(void*)fileIsNotOpenAnyMore);
	free(fileToRemove);
	pthread_mutex_unlock(&(globalFileDescriptors->mutex));
	free(f->permissions);
}
void close_opened_files(t_program* p){
	int tam=list_size(p->fileDescriptors);
	int i;
	if (tam>0){
		log_info(logKernel,"Se detectaron %i archivos sin cerrar. Se procedera a cerrarlos\n", tam);
		printf("Se detectaron %i archivos sin cerrar. Se procedera a cerrarlos\n", tam);
		for(i=0;i!=tam;i++){
			list_remove_and_destroy_element(p->fileDescriptors,i,(void*)deleteFileFromProcessFileTable);
		}

	}
}

_Bool program_did_finish(t_program* program, _Bool lockQueue)
{
	_Bool is_the_program(void* elem)
	{
		t_program* alreadyEndedProgram = (t_program*) elem;
		return alreadyEndedProgram->pcb->pid == program->pcb->pid;
	};

	if (lockQueue) pthread_mutex_lock(&(queueFinishedPrograms->mutex));
	t_program* findResult = list_find(queueFinishedPrograms->list, is_the_program);
	if (lockQueue) pthread_mutex_unlock(&(queueFinishedPrograms->mutex));

	return findResult == program;
}

void program_finish(t_program* program){
	// TODO: esto no se si hace falta
	if (program_did_finish(program, 1))
	{
		log_info(logKernel, "[program_finish] program %d ya habia finalizado", program->pcb->pid);
		return;
	}

	pthread_mutex_lock(&(queueFinishedPrograms->mutex));
	list_add(queueFinishedPrograms->list, program);
	pthread_mutex_unlock(&(queueFinishedPrograms->mutex));

	close_opened_files(program);
	memory_end_program(program);

	// avisar a consola y cerrar
	// con FD_ISSET(program->socket, programMasterRecord) funca pero
	// esto le da un poco de mas de sentido;
	int informConsole =	program->interruptionCode != ERROR_CONSOLE_DISCONNECTED &&
						program->interruptionCode != ERROR_CONSOLE_FINISH_COMMAND;
	FD_CLR(program->socket, programMasterRecord);

	log_debug(logKernel, "socket del programa (%d) estaba sacado? %d", program->socket, informConsole == 0);


	if (informConsole)
	{
		if(socket_send_string(program->socket, "FinEjecucion")<=0){
			log_info(logKernel,"No se pudo conectar con el programa %i para que finalizo\n", program->pcb->pid);
			close(program->socket);
			return;
		}


	int memory_leak = get_memory_leaks(program);

	if(memory_leak>0){
		log_info(logKernel,"Se detecta %i B de heap del proceso sin liberar. \n", memory_leak);
		printf("Se detecta %i B de heap del proceso sin liberar. \n",memory_leak);
	}

	//TODO cerrar los archivos abiertos
	memory_end_program(program);
		if(socket_send_int(program->socket, program->pcb->exitCode)<=0){
			log_info(logKernel,"No se pudo conectar con el programa %i para que finalizo\n", program->pcb->pid);
			close(program->socket);
			return;
		}
	}

	close(program->socket);
}

int get_memory_leaks(t_program* program){

	int leak = 0, i;


	if(list_size(program->heapPages) > 0){ // aca guarda las paginas del heap

		t_heap_page* page = list_get(program->heapPages, i);
		for(i=0;i<list_size(program->heapPages);i++){

			int freeSpace = ((t_heap_page*)list_get(program->heapPages, i))->freeSpace;
			if(freeSpace < pageSize - sizeof(t_heapmetadata)){
				leak = leak + (pageSize - 2*sizeof(t_heapmetadata) - freeSpace);
			}

			log_info(logKernel, "page size: %i \n", pageSize);
			log_info(logKernel, "freeSpace: %i \n", freeSpace);
		}
	}

	return leak;
}

void program_interrup(int socket, int interruptionCode, int overrideInterruption){
	/*
	 * Interruption codes:
	 *	-1		No se pudieron reservar recursos para ejecutar el programa
	 * 	-2		El programa intento leer un archivo que no existe
	 * 	-3		El programa intento leer un archivo sin permisos
	 * 	-4		El programa intento escribir un archivo sin permisos
	 * 	-5		Excepcion de memoria
	 * 	-6		Finalizado a travez de desconexion de consola
	 * 	-7		Finalizado a travez del comando finalizar programa de la consola
	 * 	-8		Se intento reservar mas memoria que el tamaño de una pagina
	 * 	-9		No se pueden asignar mas paginas al proceso
	 * 	-10		Se intento borrar un archivo abierto por varios procesos
	 * 	-11		File descriptor inexistente
	 * 	-12		Se intento abrir un archivo inexistente
	 * 	-13		No se pudo borrar un archivo en FS
	 * 	-14		Semaforo inexistente
	 * 	-15		Shared variable inexistente
	 * 	-16		El cpu se desconecto y dejo el programa en un estado inconsistente
	 * 	-17		Finalizado a travez del comando finalizar programa de la consola del kernel
	 * 	-20		Error sin definicion
	 * */


	bool _buscarProgramaSocket(t_program* programa){
		return programa->socket==socket;
	}

	bool _buscarProgramaSocketInCPUs(t_cpu* cpu){
		return cpu->program != NULL && cpu->program->socket==socket;
	}

	int programaEncontrado = 0;

	//Lockeo la lista de cpus para que no pueda salir de la ejecucion de un cpu y pasarme por alto.
	pthread_mutex_lock(&(queueCPUs->mutex));


	//Reviso los cpus para ver si  el programa  esta ejecutando
	t_cpu* cpu = list_find(queueCPUs->list, (void*)_buscarProgramaSocketInCPUs);
	if(cpu != NULL){
		programaEncontrado = 1;
		if(cpu->program->interruptionCode == 0 || overrideInterruption == 1){
			log_info(logKernel, "El programa %i fue encontrado en la lista de cpus y se le puso el interruption code: %i.", cpu->program->pcb->pid, interruptionCode);
			printf("El programa %i fue encontrado en la lista de cpus y se le puso el interruption code: %i\n", cpu->program->pcb->pid, interruptionCode);
			cpu->program->interruptionCode = interruptionCode;
		}
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
		pthread_mutex_lock(&(queueBlockedPrograms->mutex));
		t_program* program = list_remove_by_condition(queueBlockedPrograms->list, (void*)_buscarProgramaSocket);
		if(program != NULL){
			log_info(logKernel, "El programa %i fue encontrado en la lista de bloqueados y se le puso el interruption code: %i.", program->pcb->pid, interruptionCode);
			printf("El programa %i fue encontrado en la lista de bloqueados y se le puso el interruption code: %i\n", program->pcb->pid, interruptionCode);
			programaEncontrado = 1;
			program->pcb->exitCode = interruptionCode;
			program_finish(program);
		}
		pthread_mutex_unlock(&(queueBlockedPrograms->mutex));
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

void program_unblock(t_semaforo* sem){
	pthread_mutex_lock(&sem->mutex);

	bool is_blocked_by_sem(t_program* program){
		return 	program->waiting==1 && (strcmp(program->waitingReason, sem->nombre) == 0);
	}

	pthread_mutex_lock(&queueBlockedPrograms->mutex);
	t_program* blockedProgram = list_remove_by_condition(queueBlockedPrograms->list, (void*)is_blocked_by_sem);
	pthread_mutex_unlock(&queueBlockedPrograms->mutex);

	if (blockedProgram != NULL){

		blockedProgram->waiting = 0;
		free(blockedProgram->waitingReason);
		blockedProgram->waitingReason = NULL;

		pthread_mutex_lock(&queueReadyPrograms->mutex);
		list_add(queueReadyPrograms->list, blockedProgram);
		pthread_mutex_unlock(&queueReadyPrograms->mutex);

		printf("[prog_unblovk] se desbloqueo %d\n", blockedProgram->pcb->pid);

		cpu_inactive_planner(0 /*no lockear cpus, ya vienen loqueadas desde el handler*/);
	}
	else
	{
		printf("[prog_unblovk] no se desbloquea ningun programa\n");
	}

	pthread_mutex_unlock(&sem->mutex);
}
