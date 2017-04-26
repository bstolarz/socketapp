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
	//Envio el comando read a Memoria
	if (socket_send_string(serverMemory,"read")>0){
		log_info(logCPU,"Se envia correctamente el comando read a Memoria\n");
	}else{
		log_info(logCPU,"Error enviando el comando read a Memoria\n");
	}
	//Envio el PID
	if (socket_send_int(serverMemory,pcb->pid)>0){
		log_info(logCPU,"Envio del PID del programa a Memoria\n");
	}else{
		log_info(logCPU,"Error enviando el PID del programa a Memoria\n");
		return -1;
	}

	div_t values;
	int offsetToMemory;
	values=div(pcb->indiceDeCodigo->offset_inicio,pageSize);
	int page=values.quot;

	//Envio el numero de pagina
	if (socket_send_int(serverMemory,page)>0){
		log_info(logCPU,"Se envia el numero de pagina: %d\n",page);
	}else{
		log_info(logCPU,"Error enviando el numero de pagina %d al Memoria\n",page);
		return -1;
	}
	offsetToMemory=(pcb->indiceDeCodigo->offset_inicio)-(page*pageSize);

	//Envio el offset
	if (socket_send_int(serverMemory,offsetToMemory)>0){
		log_info(logCPU,"Envio el offset a Memoria: %d\n",offsetToMemory);
	}else{
		log_info(logCPU,"Error enviando el offset a Memoria\n");
		return -1;
	}
	return 1;
}

void connect_to_memory()
{
	socket_client_create(&serverMemory, configCPU->ip_memory, configCPU->puerto_memory);

	// obtener tamanio de pagina
	int askPageSizeResult = socket_send_string(serverMemory, "frame_size");

	if (askPageSizeResult > 0)
	{
		log_info(logCPU, "[page_size] se mando bien\n");
	}
	else
	{
		log_error(logCPU, "[page_size] no se mando bien exit program\n");
		exit(EXIT_FAILURE);
	}

	//Recibo el tamanio de pagina desde Memoria
	if (socket_recv_int(serverMemory, &pageSize) > 0)
	{
		log_info(logCPU,"[page_size] pageSize = %d\n", pageSize);
	}
	else
	{
		log_info(logCPU, "[page_size] error en recv(). exit program\n");
		exit(EXIT_FAILURE);
	}
}

int main(int arg, char* argv[]) {
	if(arg!=2){
		printf("Path missing! %d\n", arg);
		return 1;
	}

	// inicializacion - init()?
	configCPU = malloc(sizeof(t_cpu));
	config_read(argv[1]);
	config_print();

	logCPU = logCreate();

	// Me conenecto al Kernel
	socket_client_create(&serverKernel, configCPU->ip_kernel, configCPU->puerto_kernel);
	socket_send_string(serverKernel, "NewCPU");

	connect_to_memory();

	if(serverKernel)
	{
		pcb=(t_pcb*)malloc(sizeof(t_pcb));
		recv_pcb(serverKernel,pcb);
		incrementarPC(pcb);
		if(solicitarProximaSentenciaAEjecutarAMemoria(pcb)){
			void* buffer;
			socket_recv(serverKernel,&buffer,pcb->indiceDeCodigo->offset_fin);
			analizadorLinea((char*)buffer,funciones,kernel);
			//actualiza los valores del programa en la memoria
			incrementarPC(pcb);
			//notifico al Kernel que terminé de ejecutar
			socket_send_string(serverKernel,"FinishedQuantum");
		}
	}

	return EXIT_SUCCESS;
}

