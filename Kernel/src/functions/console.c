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

#include "../interface/memory.h"
#include "../planner/ltp.h"
#include "../functions/cpu.h"

void console_process_list(){
	void _printProccessList(t_program * program){
		printf("	%i\n", program->pcb->pid);
	}

	void _printProccessFinishList(t_program * program){
		printf("	%i - %i\n", program->pcb->pid, program->pcb->exitCode);
	}

	void _printProccessExecutingList(t_cpu * cpu){
		if(cpu->program != NULL){
			printf("	%i\n", cpu->program->pcb->pid);
		}
	}

	pthread_mutex_lock(&queueNewPrograms->mutex);
	printf("Lista de procesos nuevos\n");
	list_iterate(queueNewPrograms->list, (void*)_printProccessList);
	pthread_mutex_unlock(&queueNewPrograms->mutex);

	pthread_mutex_lock(&queueCPUs->mutex);
	printf("Lista de procesos ejecutando\n");
	list_iterate(queueCPUs->list, (void*)_printProccessExecutingList);
	pthread_mutex_unlock(&queueCPUs->mutex);

	pthread_mutex_lock(&queueReadyPrograms->mutex);
	printf("Lista de procesos listos\n");
	list_iterate(queueReadyPrograms->list, (void*)_printProccessList);
	pthread_mutex_unlock(&queueReadyPrograms->mutex);

	pthread_mutex_lock(&queueBlockedPrograms->mutex);
	printf("Lista de procesos bloqueados\n");
	list_iterate(queueBlockedPrograms->list, (void*)_printProccessList);
	pthread_mutex_unlock(&queueBlockedPrograms->mutex);

	pthread_mutex_lock(&queueFinishedPrograms->mutex);
	printf("Lista de procesos finalizados\n");
	list_iterate(queueFinishedPrograms->list, (void*)_printProccessFinishList);
	pthread_mutex_unlock(&queueFinishedPrograms->mutex);
	printf("\n-----------------FIN DE LISTAR PROCESOS------------\n\n");
}
t_program* seek_program(t_list* l,int pid){
	int i,t;
	t_program* program=NULL;
	t=list_size(l);
	printf("List size: %d\n", t);
	for(i=0;i<t;i++){
		t_program* p=(t_program*)list_get(l,i);
		if(p->pcb->pid==pid){
			program=p;
			printf("Encontre el programa\n");
		}
	}
	return program;
}

t_program* get_program(int pid){

	bool encontrarPrograma(t_program* program){
		return program->pcb->pid==pid;
	}

	t_program* prog = NULL;

	pthread_mutex_lock(&queueBlockedPrograms->mutex);
	prog=list_find(queueBlockedPrograms->list,(void*)encontrarPrograma);
	pthread_mutex_unlock(&queueBlockedPrograms->mutex);

	if(prog == NULL){
		pthread_mutex_lock(&queueFinishedPrograms->mutex);
		prog=list_find(queueFinishedPrograms->list,(void*)encontrarPrograma);
		pthread_mutex_unlock(&queueFinishedPrograms->mutex);
	}

	if(prog == NULL){
		printf("No estaba en finalizados\n");
		pthread_mutex_lock(&queueReadyPrograms->mutex);
		prog=list_find(queueReadyPrograms->list,(void*)encontrarPrograma);
		pthread_mutex_unlock(&queueReadyPrograms->mutex);
	}

	return prog;
	}
/*
t_program* get_program(int pid){
	t_program* prog;
	pthread_mutex_lock(&queueBlockedPrograms->mutex);
	prog=seek_program(queueBlockedPrograms->list,pid);
	pthread_mutex_unlock(&queueBlockedPrograms->mutex);
	if (prog==NULL){
		pthread_mutex_lock(&queueFinishedPrograms->mutex);
		prog=seek_program(queueFinishedPrograms->list,pid);
		pthread_mutex_lock(&queueFinishedPrograms->mutex);
		if(prog==NULL){
			pthread_mutex_lock(&queueReadyPrograms->mutex);
			prog= seek_program(queueReadyPrograms->list,pid);
			pthread_mutex_unlock(&queueBlockedPrograms->mutex);
			if(prog==NULL){
				pthread_mutex_lock(&queueCPUs->mutex);
				//prog= seek_program(queueCPUs->list,pid);
				pthread_mutex_unlock(&queueCPUs->mutex);
			}
		//	return prog;

		}else {
			printf("Estaba en finish\n");
			return prog;
		}
	}else{
		return prog;
	}
}
*/int check_pid_is_running(int pid){
	pthread_mutex_lock(&queueBlockedPrograms->mutex);
	int tam=list_size(queueBlockedPrograms->list);
	int i;
	for (i=0;i!=tam;i++){
		t_program* p=(t_program*)list_get(queueBlockedPrograms->list,i);
		if (p->pcb->pid==pid){
			pthread_mutex_unlock(&queueBlockedPrograms->mutex);
			return 1;
		}
	}
	pthread_mutex_unlock(&queueBlockedPrograms->mutex);

	pthread_mutex_lock(&queueNewPrograms->mutex);
	tam=list_size(queueNewPrograms->list);
	for (i=0;i!=tam;i++){
		t_program* p=(t_program*)list_get(queueNewPrograms->list,i);
		if (p->pcb->pid==pid){
			pthread_mutex_unlock(&queueNewPrograms->mutex);
			return 1;
		}
	}
	pthread_mutex_unlock(&queueNewPrograms->mutex);

	pthread_mutex_lock(&queueReadyPrograms->mutex);
	tam=list_size(queueReadyPrograms->list);
	for (i=0;i!=tam;i++){
		t_program* p=(t_program*)list_get(queueReadyPrograms->list,i);
		if (p->pcb->pid==pid){
			pthread_mutex_unlock(&queueReadyPrograms->mutex);
			return 1;
		}
	}
	pthread_mutex_unlock(&queueReadyPrograms->mutex);

	pthread_mutex_lock(&queueCPUs->mutex);
	tam=list_size(queueCPUs->list);
	for (i=0;i!=tam;i++){
		t_program* p=(t_program*)list_get(queueCPUs->list,i);
		if (p->pcb->pid==pid){
			pthread_mutex_unlock(&queueCPUs->mutex);
			return 1;
		}
	}
	pthread_mutex_unlock(&queueCPUs->mutex);
	return 0;
}
int check_pid_is_incorrect(int pid){
	int tam=list_size(queueBlockedPrograms->list);
	int i;
	for (i=0;i<tam;i++){
		t_program* p=(t_program*)list_get(queueBlockedPrograms->list,i);
		if (p->pcb->pid==pid){
			return 1;
		}
	}
	tam=list_size(queueFinishedPrograms->list);
	for (i=0;i<tam;i++){
		t_program* p=(t_program*)list_get(queueFinishedPrograms->list,i);
		if (p->pcb->pid==pid){
			return 1;
		}
	}
	tam=list_size(queueNewPrograms->list);
	for (i=0;i<tam;i++){
		t_program* p=(t_program*)list_get(queueNewPrograms->list,i);
		if (p->pcb->pid==pid){
			return 1;
		}
	}
	tam=list_size(queueReadyPrograms->list);
	for (i=0;i<tam;i++){
		t_program* p=(t_program*)list_get(queueReadyPrograms->list,i);
		if (p->pcb->pid==pid){
			return 1;
		}
	}
	return 0;
}

void print_rafagas_del_proceso(int p){
	t_program* program=get_program(p);
	printf("El programa con PID [%d] ha ejecutado [%d rafagas]\n",program->pcb->pid,program->stats.rafagas);
}
void print_syscalls(int p){
	t_program* program=get_program(p);
	printf("El programa con PID [%d] ha ejecutado [%d syscalls]\n",program->pcb->pid,program->stats.syscallEjecutadas);
}
void print_list(t_fd* fd){
	printf("............................................\n");
	printf("FD -> %d\n",fd->value);
	printf("Flags -> %s\n",fd->permissions);
	printf("Path -> %s\n",fd->global->path);
}

void print_file_process_table(int p){

	t_program* program=get_program(p);
	if(program!=NULL){
		if(list_size(program->fileDescriptors)==0){
			printf("El programa con PID [%d] no ha abierto ningun archivo\n",program->pcb->pid);
		}else{
			if(list_size(program->fileDescriptors)==1){
				printf("\n\n\n------Tabla de archivos del proceso %d----------\n",p);
				t_list* l=program->fileDescriptors;
				t_fd* fd=(t_fd*)list_get(l,0);
				printf("FD -> %d\n",fd->value);
				printf("Flags -> %s\n",fd->permissions);
				printf("Path -> %s\n",fd->global->path);
				printf("------------------------------------------------\n\n");
			}else{
				printf("\n\n------Tabla de archivos del proceso %d----------\n",p);
				list_iterate(program->fileDescriptors,(void*)print_list);
				printf("------------------------------------------------\n\n");
			}
		}
	}else{
		printf("Esto se va al carajo!\n");
	}
}
void print_head_pages_used(int p){
	t_program* program=get_program(p);
	printf("Proceso con PID [%d] aloco %d paginas del heap\n",program->pcb->pid,program->stats.pagesAlloc);
	printf("Proceso con PID [%d] libero %d paginas del heap\n",program->pcb->pid,program->stats.pagesFree);
}
void console_get_process_stats(){
	int pidProceso;
	printf("[SISTEMA] - Ingrese el PID del proceso: ");
	scanf("%d",&pidProceso);
	while(check_pid_is_incorrect(pidProceso)==0){
		printf("El PID ingresado no existe. Vuelva a intentarlo. Si desea volver al menu de comandos ingrese 0\n");
		printf("[SISTEMA] - Ingrese el PID del proceso: ");
		scanf("%d",&pidProceso);
		if (pidProceso==0){
			return;
		}
	}
	printf("[SISTEMA] - Ingrese el numero de comando:\n");
	printf("[SISTEMA] - 1: Cantidad de rafagas.\n");
	printf("[SISTEMA] - 2: Operaciones privilegiadas ejecutadas.\n");
	printf("[SISTEMA] - 3: Tabla de archivos abiertos por el proceso.\n");
	printf("[SISTEMA] - 4: Cantidad de paginas de heap utilizadas.\n");

	int opcion;
	scanf("%d",&opcion);
	switch(opcion){
	case 1:
		print_rafagas_del_proceso(pidProceso);
		break;
	case 2:
		print_syscalls(pidProceso);
		break;
	case 3:
		print_file_process_table(pidProceso);
		break;
	case 4:
		print_head_pages_used(pidProceso);
		break;
	default:
		printf("La operacion elegida no existe.\n");
		break;
	}
}

void console_get_global_file_table(){
	t_list* l=globalFileDescriptors->list;
	int size=list_size(l);
	if (size==0){
		printf("No hay archivos abiertos\n");
	}else{
		printf("\n\n\n--------TABLA GLOBAL DE ARCHIVOS---------\n");


		int i;
		for (i=0;i!=size;i++){
			t_global_fd* globalFD=(t_global_fd*)list_get(l,i);
			printf("|Path -> ");
			printf("'%s'\n",globalFD->path);
			printf("|Open -> ");
			printf("%d\n",globalFD->open);
			printf("...........................................\n");
		}
		printf("\n\n");
	}
}

void console_multiprogram_degree(){
	printf("\n\nEl grado de multiprogramacion es: %d\n\n", configKernel->grado_multiprog);
	printf("Ingrese el nuevo valor\n");
	int value;
	scanf("%d",&value);
	configKernel->grado_multiprog=value;
	printf("\n\nEl nuevo valor del grado de multiprogramacion es: %d\n\n",value);
}

void console_finish_process(){
	printf("Ingrese el PID del proceso que desea finalizar\n");
	int p;
	scanf("%d",&p);
	if(check_pid_is_running(p)==1){
		t_program* pr=get_program(p);
		program_interrup(pr->socket,-7,1);
	}
	if(check_pid_is_running(p)==0){
		printf("El programa ha sido finalizado con exito\n");
	}else{
		printf("El programa no pudo ser finalizado\n");
	}
}

void console_stop_planning(){
	pthread_mutex_lock(&queueBlockedPrograms->mutex);
	pthread_mutex_lock(&queueCPUs->mutex);
	pthread_mutex_lock(&queueNewPrograms->mutex);
	pthread_mutex_lock(&queueReadyPrograms->mutex);
}

void console_start_planning(){
	pthread_mutex_unlock(&queueBlockedPrograms->mutex);
		pthread_mutex_unlock(&queueCPUs->mutex);
		pthread_mutex_unlock(&queueNewPrograms->mutex);
		pthread_mutex_unlock(&queueReadyPrograms->mutex);
}
