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
#include "functions/memory_requests.h"
#include <parser/metadata_program.h>

int serverKernel=0;
int serverMemory=0;

void test_memory_connection()
{
	char* code = 0;
	FILE *f = fopen("../programas-ejemplo/facil.ansisop", "rb");
	int fileSize;

	if (f)
	{
	  fseek (f, 0, SEEK_END);
	  fileSize = ftell (f);

	  fseek (f, 0, SEEK_SET);
	  code = malloc(fileSize);

	  if (code){
		fread (code, 1, fileSize, f);
	  }
	  fclose (f);

	  printf("codigo leido: %s\n", code);
	}else{
		printf("lei mal el archivo!");
		exit(EXIT_FAILURE);
	}

	t_metadata_program* programMetadata = metadata_desde_literal(code);

	int i;
	t_intructions* instructionsBeginSize = programMetadata->instrucciones_serializado;

	for (i = 0; i != programMetadata->instrucciones_size; ++i)
	{
		//printf("instruccion %d: empieza en %d y termina en %d\n", i, instructionsBeginSize[i].start, instructionsBeginSize[i].offset);
		int codePage = instructionsBeginSize[i].start / pageSize;
		int codeOffset = instructionsBeginSize[i].start % pageSize;
		int size = instructionsBeginSize[i].offset;
		void* data = memory_request_read(serverMemory, 0, codePage, codeOffset, size);

		if (data == NULL) printf("no pude obtener instruccion\n");
		else printf("instruccion %d desde memoria: %s\n", i, (char*)data);
	}

	metadata_destruir(programMetadata);
	free(code);
	exit(EXIT_SUCCESS);
}

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
	pageSize = memory_request_frame_size(serverMemory);

	if (pageSize > 0)
	{
		log_info(logCPU,"[page_size] se obtuvo pageSize = %d\n", pageSize);
	}
	else
	{
		log_error(logCPU, "[page_size] no se mando bien. exit program\n");
		exit(EXIT_FAILURE);
	}
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
	socket_client_create(&serverKernel, configCPU->ip_kernel, configCPU->puerto_kernel);
	socket_send_string(serverKernel, "NewCPU");

	connect_to_memory();
	//test_memory_connection();

	if(serverKernel){
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

