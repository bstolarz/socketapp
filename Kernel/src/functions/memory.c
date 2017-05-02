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

