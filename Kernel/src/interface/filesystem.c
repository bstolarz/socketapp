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

void filesystem_connect(){
	pthread_mutex_init(&(fileSystemServer.mutex),NULL);
	fileSystemServer.socket=0;
	socket_client_create(&fileSystemServer.socket, configKernel->ip_fs, configKernel->puerto_fs);
	if(socket_send_string(fileSystemServer.socket,"KERNEL")>0){
		log_info(logKernel, "Conexion a FS exitosa\n");
	}else{
		log_info(logKernel, "No se pudo conectar a FS\n");
	}
}

int filesystem_read(char* path, size_t offset, int size, void** buffer){

	if (socket_send_string(fileSystemServer.socket, "OBTENERDATOS") == -1)
	{
		log_error(logKernel, "[OBTENERDATOS] Error mandando orden leer archivo '%s'",path);
		return -20;
	}

	if(socket_send_string(fileSystemServer.socket, path) == -1)
	{
		log_error(logKernel, "[OBTENERDATOS] Error mandando nombre del archivo '%s'",path);
		return -20;
	}

	if(socket_send_int(fileSystemServer.socket, offset) == -1)
	{
		log_error(logKernel, "[OBTENERDATOS] Error mandando offset archivo '%s'",path);
		return -20;
	}

	if(socket_send_int(fileSystemServer.socket, size) == -1)
	{
		log_error(logKernel, "[OBTENERDATOS] Error mandando tamanio a leer archivo '%s'",path);
		return -20;
	}

	int readCount;

	if (socket_recv_int(fileSystemServer.socket, &readCount) == -1)
	{
		log_error(logKernel, "[OBTENERDATOS] error obteniendo respuesta del fs archivo '%s'", path);
		return -20;
	}

	if (readCount >= 0)
	{
		if (socket_recv(fileSystemServer.socket, buffer, 1) == -1)
		{
			log_error(logKernel, "[OBTENERDATOS] error obteniendo data del fs archivo '%s'", path);
			return -20;
		}
	}

	return readCount;
}

int filesystem_write(char* path, int offset, void* buffer, int size){
	if(socket_send_string(fileSystemServer.socket,"GUARDARDATOS")>0){
		log_info(logKernel, "Envio correctamente a FS que quiero escribir el archivo '%s'",path);
	}else{
		log_info(logKernel, "Error al enviar a FS que quiero escribir el archivo '%s'",path);
	}
	if(socket_send_string(fileSystemServer.socket,path)>0){
		log_info(logKernel, "Envio correctamente a FS la path '%s' que quiero escribir",path);
	}else{
		log_info(logKernel, "Error al enviar a FS la path '%s' que quiero escribir",path);
	}
	if(socket_send_int(fileSystemServer.socket,offset)>0){
		log_info(logKernel, "Envio correctamente a FS el offset: %d",offset);
	}else{
		log_info(logKernel, "Error al enviar a FS el offset: %d", offset);
	}

	if(socket_send(fileSystemServer.socket, buffer, size)>0){
		log_info(logKernel, "Envio correctamente a FS el buffer a escribir: %s, %d", buffer, size);
	}else{
		log_info(logKernel, "Error al enviar a FS el buffer a escribir: %s, %d", buffer, size);
	}

	int respuestaFromFS;

	if (socket_recv_int(fileSystemServer.socket,&respuestaFromFS)>0) {
		return respuestaFromFS;
	}
	else {
		log_error(logKernel, "Error recibiendo respuesta del FS al escribir archivo");
		return -1;
	}
}

// TODO: no se esta usando
int filesystem_close(){
	if(socket_send_string(fileSystemServer.socket,"CERRAR")>0){
		log_info(logKernel, "Envio correctamente a FS que quiero borrar");
	}else{
		log_info(logKernel, "Error al enviar a FS que quiero borrar");
	}

	return 0;
}

int filesystem_delete(char* path){
	if(socket_send_string(fileSystemServer.socket,"BORRAR")<=0){
		log_info(logKernel, "Error al enviar a FS que quiero borrar");
	}

	if(socket_send_string(fileSystemServer.socket,path)<=0){
		log_info(logKernel, "Error al enviar a FS el path que quiero borrar");
	}

	int resp;
	if(socket_recv_int(fileSystemServer.socket,&resp)>0){
		log_info(logKernel, "Error al obtener la respuesta del borrado");
	}

	return resp;
}

int filesystem_create(char* path){
	//Le envio al FS la orden de crear
	if(socket_send_string(fileSystemServer.socket,"CREAR")>0){
		log_info(logKernel, "Le informo al FS que cree un archivo");
	}else{
		log_info(logKernel, "Error al informar as FS que cree un archivo");
	}
	//Le envio al FS el path
	if(socket_send_string(fileSystemServer.socket,path)>0){
		log_info(logKernel, "Le envio al FS la path: %s",path);
	}else{
		log_info(logKernel, "Error al enviar al FS el archivo: %s", path);
	}

	//Recibo exito o fail de FS
	int resp;
	if(socket_recv_int(fileSystemServer.socket,&resp)>0){
		log_info(logKernel, "Recibo con exito la respuesta de creacion de un archivo de FS");
	}else{
		log_info(logKernel, "Error al informar al FS que cree un archivo");
	}
	return resp;
}

int filesystem_validate(char* path){
	//Le pido el file descriptor a FS
	if (socket_send_string(fileSystemServer.socket,"VALIDAR")<=0){
		log_info(logKernel, "Error al indicarle a FS que quiero validar una path");
		return 0;
	}

	//Envio el path a FS
	if (socket_send_string(fileSystemServer.socket,path)<=0){
		log_info(logKernel, "Error al enviarle a FS la path: %s", path);
		return 0;
	}

	int respuesta;
	if (socket_recv_int(fileSystemServer.socket,&respuesta)<=0){
		log_info(logKernel, "Error al obtener respuesta del FS");
		return 0;
	}
	return respuesta;
}
