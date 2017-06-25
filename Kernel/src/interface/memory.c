#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <stdbool.h>

#include "../libSockets/server.h"
#include "../libSockets/client.h"
#include "../libSockets/recv.h"
#include "../libSockets/send.h"

#include "../commons/structures.h"
#include "../commons/declarations.h"

#include "../functions/heap.h"
#include "memory.h"

void memory_connect(){
	pthread_mutex_init(&(memoryServer.mutex),NULL);
	socket_client_create(&memoryServer.socket, configKernel->ip_memoria, configKernel->puerto_memoria);
	pageSize = memory_frame_size();
}

int memory_frame_size(){
	int frameSize = 0;
	pthread_mutex_lock(&memoryServer.mutex);

	//Consulto por el frame size
	if(socket_send_string(memoryServer.socket, "frame_size")<=0){
		printf("Se perdio la conexion con la memoria\n");
		log_warning(logKernel, "Se perdio la conexion con la memoria");
		pthread_mutex_unlock(&memoryServer.mutex);
		return -20;
	}

	//Recibo el frame size
	if(socket_recv_int(memoryServer.socket, &frameSize)<=0){
		printf("Se perdio la conexion con la memoria\n");
		log_warning(logKernel, "Se perdio la conexion con la memoria");
		pthread_mutex_unlock(&memoryServer.mutex);
		return -20;
	}

	pthread_mutex_unlock(&memoryServer.mutex);

	return frameSize;
}

int memory_init(t_program* program, int cantPaginas){

	pthread_mutex_lock(&memoryServer.mutex);

	//Pido el inicio de un nuevo programa
	if(socket_send_string(memoryServer.socket, "init")<=0){
		printf("Se perdio la conexion con la memoria\n");
		log_warning(logKernel, "Se perdio la conexion con la memoria");
		pthread_mutex_unlock(&memoryServer.mutex);
		return -20;
	}

	//Envio el PID
	if(socket_send_int(memoryServer.socket, program->pcb->pid)<=0){
		printf("Se perdio la conexion con la memoria\n");
		log_warning(logKernel, "Se perdio la conexion con la memoria");
		pthread_mutex_unlock(&memoryServer.mutex);
		return -20;
	}

	//Envio la cantidad de paginas a pedir
	if(socket_send_int(memoryServer.socket, cantPaginas)<=0){
		printf("Se perdio la conexion con la memoria\n");
		log_warning(logKernel, "Se perdio la conexion con la memoria");
		pthread_mutex_unlock(&memoryServer.mutex);
		return -20;
	}

	//Obtengo respuesta del init
	int respuesta =0;
	if(socket_recv_int(memoryServer.socket, &respuesta)<=0){
		printf("Se perdio la conexion con la memoria\n");
		log_warning(logKernel, "Se perdio la conexion con la memoria");
		pthread_mutex_unlock(&memoryServer.mutex);
		return -20;
	}

	pthread_mutex_unlock(&memoryServer.mutex);
	return respuesta;
}

int memory_get_pages(t_program* program, int cantPaginas){

	pthread_mutex_lock(&memoryServer.mutex);

	//Pido el inicio de un nuevo programa
	if(socket_send_string(memoryServer.socket, "get_pages")<=0){
		printf("Se perdio la conexion con la memoria\n");
		log_warning(logKernel, "Se perdio la conexion con la memoria");
		pthread_mutex_unlock(&memoryServer.mutex);
		return -20;
	}

	//Envio el PID
	if(socket_send_int(memoryServer.socket, program->pcb->pid)<=0){
		printf("Se perdio la conexion con la memoria\n");
		log_warning(logKernel, "Se perdio la conexion con la memoria");
		pthread_mutex_unlock(&memoryServer.mutex);
		return -20;
	}

	//Envio la cantidad de paginas a pedir
	if(socket_send_int(memoryServer.socket, cantPaginas)<=0){
		printf("Se perdio la conexion con la memoria\n");
		log_warning(logKernel, "Se perdio la conexion con la memoria");
		pthread_mutex_unlock(&memoryServer.mutex);
		return -20;
	}

	//Obtengo respuesta del init
	int respuesta =0;
	if(socket_recv_int(memoryServer.socket, &respuesta)<=0){
		printf("Se perdio la conexion con la memoria\n");
		log_warning(logKernel, "Se perdio la conexion con la memoria");
		pthread_mutex_unlock(&memoryServer.mutex);
		return -20;
	}

	pthread_mutex_unlock(&memoryServer.mutex);
	return respuesta;
}

int memory_free_page(t_program* program, int page){
	pthread_mutex_lock(&memoryServer.mutex);

	//Pido el inicio de un nuevo programa
	if(socket_send_string(memoryServer.socket, "free_page")<=0){
		printf("Se perdio la conexion con la memoria\n");
		log_warning(logKernel, "Se perdio la conexion con la memoria");
		pthread_mutex_unlock(&memoryServer.mutex);
		return -20;
	}

	//Envio el PID
	if(socket_send_int(memoryServer.socket, program->pcb->pid)<=0){
		printf("Se perdio la conexion con la memoria\n");
		log_warning(logKernel, "Se perdio la conexion con la memoria");
		pthread_mutex_unlock(&memoryServer.mutex);
		return -20;
	}

	//Envio la cantidad de paginas a pedir
	if(socket_send_int(memoryServer.socket, page)<=0){
		printf("Se perdio la conexion con la memoria\n");
		log_warning(logKernel, "Se perdio la conexion con la memoria");
		pthread_mutex_unlock(&memoryServer.mutex);
		return -20;
	}

	//Obtengo respuesta del init
	int respuesta =0;
	if(socket_recv_int(memoryServer.socket, &respuesta)<=0){
		printf("Se perdio la conexion con la memoria\n");
		log_warning(logKernel, "Se perdio la conexion con la memoria");
		pthread_mutex_unlock(&memoryServer.mutex);
		return -20;
	}

	pthread_mutex_unlock(&memoryServer.mutex);
	return respuesta;
}

int memory_read(t_program* program, int page, int offset, int size, void** buffer){
	pthread_mutex_lock(&memoryServer.mutex);

	//Envio la operacion
	if(socket_send_string(memoryServer.socket, "read")<=0){
		printf("Se perdio la conexion con la memoria\n");
		log_warning(logKernel, "Se perdio la conexion con la memoria");
		pthread_mutex_unlock(&memoryServer.mutex);
		return -20;
	}

	//Envio el PID
	if(socket_send_int(memoryServer.socket, program->pcb->pid)<=0){
		printf("Se perdio la conexion con la memoria\n");
		log_warning(logKernel, "Se perdio la conexion con la memoria");
		pthread_mutex_unlock(&memoryServer.mutex);
		return -20;
	}

	//Envio la pagina
	if(socket_send_int(memoryServer.socket, page)<=0){
		printf("Se perdio la conexion con la memoria\n");
		log_warning(logKernel, "Se perdio la conexion con la memoria");
		pthread_mutex_unlock(&memoryServer.mutex);
		return -20;
	}

	//Envio el desplazamiento
	if(socket_send_int(memoryServer.socket, offset)<=0){
		printf("Se perdio la conexion con la memoria\n");
		log_warning(logKernel, "Se perdio la conexion con la memoria");
		pthread_mutex_unlock(&memoryServer.mutex);
		return -20;
	}

	//Envio el tamaño
	if(socket_send_int(memoryServer.socket, size)<=0){
		printf("Se perdio la conexion con la memoria\n");
		log_warning(logKernel, "Se perdio la conexion con la memoria");
		pthread_mutex_unlock(&memoryServer.mutex);
		return -20;
	}

	// primero veo si no lei fuera de las pags del proceso
	int readResult;
	if(socket_recv_int(memoryServer.socket, &readResult)<=0){
		printf("Se perdio la conexion con la memoria\n");
		log_warning(logKernel, "Se perdio la conexion con la memoria");
		pthread_mutex_unlock(&memoryServer.mutex);
		return -20;
	}

	if (readResult != size)
		return readResult;

	//Obtengo respuesta
	int nbytes =0;
	if((nbytes = socket_recv(memoryServer.socket, buffer, 1))<=0){
		printf("Se perdio la conexion con la memoria\n");
		log_warning(logKernel, "Se perdio la conexion con la memoria");
		pthread_mutex_unlock(&memoryServer.mutex);
		return -20;
	}

	pthread_mutex_unlock(&memoryServer.mutex);
	return nbytes;
}

int memory_write(t_program* program, int page, int offset, void* buffer, int size){
	pthread_mutex_lock(&memoryServer.mutex);

	//Envio la operacion
	if(socket_send_string(memoryServer.socket, "write")<=0){
		printf("Se perdio la conexion con la memoria\n");
		log_warning(logKernel, "Se perdio la conexion con la memoria");
		pthread_mutex_unlock(&memoryServer.mutex);
		return -20;
	}

	//Envio el PID
	if(socket_send_int(memoryServer.socket, program->pcb->pid)<=0){
		printf("Se perdio la conexion con la memoria\n");
		log_warning(logKernel, "Se perdio la conexion con la memoria");
		pthread_mutex_unlock(&memoryServer.mutex);
		return -20;
	}

	//Envio la pagina
	if(socket_send_int(memoryServer.socket, page)<=0){
		printf("Se perdio la conexion con la memoria\n");
		log_warning(logKernel, "Se perdio la conexion con la memoria");
		pthread_mutex_unlock(&memoryServer.mutex);
		return -20;
	}

	//Envio el desplazamiento
	if(socket_send_int(memoryServer.socket, offset)<=0){
		printf("Se perdio la conexion con la memoria\n");
		log_warning(logKernel, "Se perdio la conexion con la memoria");
		pthread_mutex_unlock(&memoryServer.mutex);
		return -20;
	}

	//Envio el buffer
	if(socket_send(memoryServer.socket, buffer, size)<=0){
		printf("Se perdio la conexion con la memoria\n");
		log_warning(logKernel, "Se perdio la conexion con la memoria");
		pthread_mutex_unlock(&memoryServer.mutex);
		return -20;
	}

	//Obtengo respuesta
	int respuesta =0;
	if(socket_recv_int(memoryServer.socket, &respuesta)<=0){
		printf("Se perdio la conexion con la memoria\n");
		log_warning(logKernel, "Se perdio la conexion con la memoria");
		pthread_mutex_unlock(&memoryServer.mutex);
		return -20;
	}

	pthread_mutex_unlock(&memoryServer.mutex);
	return respuesta;
}

t_puntero memory_heap_alloc(t_program* program, int size){

	//Verifico que se pueda reservar el tamaño solicitado
	if(size > (pageSize - 2*(sizeof(t_heapmetadata)))){
		program->interruptionCode = -8;
		return -8;
	}

	int page = 0;
	int offset = 0;
	if(heap_find_space_available(program, size, &page, &offset) != 1){
		page = heap_new_page(program);
	}

	heap_alloc(program, size, page, offset);

	return (page * pageSize) + offset + sizeof(t_heapmetadata);
}

void memory_heap_free(t_program* program, int page, int offset){
	bool _findPage(t_heap_page* hp){
		return hp->page==page;
	}

	t_heap_page* heapPage = list_find(program->heapPages, (void*)_findPage);

	if (heapPage == NULL){
		program->interruptionCode = -5;
		return;
	}

	if(offset< 0 || offset>(pageSize-1)){
		program->interruptionCode = -5;
		return;
	}

	heap_free(program, page, offset);
}






