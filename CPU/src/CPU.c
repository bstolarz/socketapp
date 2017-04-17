#include <netdb.h>
#include <unistd.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include "commons/structures.h"
#include "commons/declarations.h"
#include "functions/config.h"
#include "libSockets/client.h"
#include "libSockets/send.h"
#include "libSockets/recv.h"
#include "libSockets/server.h"

int serverKernel=0;
int serverMemory=0;


int recv_pcb(int socketServer,t_pcb* pcb){

	if (socket_recv_int(socketServer,&(pcb->pid))>0){
		log_info(logCPU,"Recibo el PID del programa: %d\n",pcb->pid);
	}else{
		log_info(logCPU,"Error recibiendo PID del programa\n");
		return -1;
	}
	if (socket_recv_int(socketServer,&(pcb->pc))>0){
		log_info(logCPU,"Recibo el Program Counter del programa con valor: %d\n",pcb->pc);
	}else{
		log_info(logCPU,"Error recibiendo el Program Counter\n");
		return -1;
	}
	if (socket_recv_int(socketServer,&(pcb->cantPagsCodigo))>0){
		log_info(logCPU,"Recibo la cantidad de paginas de codigo del programa: %d\n",pcb->cantPagsCodigo);
	}else{
		log_info(logCPU,"Error al recibir la cantidad de codigo de paginas\n");
		return -1;
	}
	if (socket_recv_int(socketServer,&(pcb->indiceDeCodigo->offset_inicio))>0){
		log_info(logCPU,"Se recibe el offset de inicio del indice de codigo con valor: %d\n",pcb->indiceDeCodigo->offset_inicio);
		return -1;
	}else{
		log_info(logCPU,"Error recibiendo el offset de inicio del indice de codigo\n");
		return -1;
	}
	if (socket_recv_int(socketServer,&(pcb->indiceDeCodigo->offset_fin))>0){
		log_info(logCPU,"Se recibe el offset de fin del indice de codigo con valor: %d\n",pcb->indiceDeCodigo->offset_fin);
	}else{
		log_info(logCPU,"Error recibiendo el offset de fin del indice de codigo\n");
		return -1;
	}
	if (socket_recv_int(socketServer,&(pcb->stackPosition))>0){
		log_info(logCPU,"Se recibe correctamente el stack position: %d\n",pcb->stackPosition);
	}else{
		log_info(logCPU,"Error recibiendo el stack position\n");
	}
	return 1;
}
t_log* logCreate(){
	char pid[10];
		snprintf(pid, 10,"%d",(int)getpid());

		char* path = string_new();
		string_append(&path, "logCPU");
		string_append(&path, pid);

		remove(path);

		t_log* logs = log_create(path, "CPU", false, LOG_LEVEL_DEBUG);

		if (logs == NULL) {
			printf("[SISTEMA] - No se pudo generar el archivo de logueo.\n");
			return NULL;
		};


		log_info(logs, "Archivo de logueo inicializado.");

		return logs;
}
void incrementarPC(t_pcb* pcb){
	pcb->pc++;
}
void kernel_lost_conection(fd_set* master, int socket, int nbytes){
	//program_interrup(socket, -6, 0);
	FD_CLR(socket, master);
}
int solicitarProximaSentenciaAEjecutarAMemoria(t_pcb* pcb){
	//El offset inicio y fin no es siempre el mismo si envío ese send?
	if (socket_send_int(serverMemory,pcb->indiceDeCodigo->offset_inicio)>0){
		log_info(logCPU,"Envio correctamente el offset inicio del indice de codigo\n",pcb->indiceDeCodigo->offset_inicio);
	}else{
		log_info(logCPU,"Error enviando offset inicio del indice de codigo\n");
		return -1;
	}
	if (socket_send_int(serverMemory,pcb->indiceDeCodigo->offset_fin)>0){
		log_info(logCPU,"Envio correctamente el offset fin del indice de codigo\n",pcb->indiceDeCodigo->offset_fin);
	}else{
		log_info(logCPU,"Error enviando el offset fin del indice de codigo\n",pcb->indiceDeCodigo->offset_fin);
		return -1;
	}
	return 1;
}
void kernel_recv_package(fd_set* master, int socket, int nbytes, char* package){
	if(strcmp(package, "PCB") == 0){
		t_pcb* pcb=(t_pcb*)malloc(sizeof(t_pcb));
		recv_pcb(socket,pcb);
		incrementarPC(pcb);
		if(solicitarProximaSentenciaAEjecutarAMemoria(pcb)){

		};
	}
}
void* recv_from_kernel(void* arg){
	socket_server_select(configCPU->puerto_memory,*kernel_lost_conection,*kernel_recv_package);
	return arg;
}
void* recv_from_memory(void* arg){
	return arg;
}
int main(int arg, char* argv[]) {
	if(arg!=2){
			printf("Path missing! %d\n", arg);
			return 1;
		}

		configCPU=malloc(sizeof(t_cpu));
		config_read(argv[1]);
		config_print();
		logCPU=logCreate();
		//Me conenecto al Kernel
		socket_client_create(&serverKernel, "127.0.0.1", "6668");

		//Me conecto a la Memoria
		socket_client_create(&serverMemory, "127.0.0.1", "6667");
		if(serverKernel){
			socket_send_string(serverKernel, "NewCPU");

			pthread_create(&pthreadKernel,NULL,recv_from_kernel,NULL);
			pthread_create(&pthreadMemory,NULL,recv_from_memory,NULL);

			pthread_join(pthreadKernel,NULL);
			pthread_join(pthreadMemory,NULL);
		}

		return EXIT_SUCCESS;
}

