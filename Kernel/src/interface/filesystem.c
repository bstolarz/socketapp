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
	socket_client_create(&fileSystemServer.socket, configKernel->ip_fs, configKernel->puerto_fs);

	socket_send_string(fileSystemServer.socket, "Kernel");
}

void filesystem_leer(char* path, size_t offset, int size){
	if (socket_send_string(fileSystemServer.socket,"OBTENERDATOS")>0){
		log_info(logKernel, "Le pido a FS obtener datos");
		if (socket_send_string(fileSystemServer.socket,path)>0){
			log_info(logKernel, "Le paso a FS el path '%s'",path);

		}else{

		}
	}
}

void filesystem_escribir(char* path, int offset, int size){
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
	if(socket_send_int(fileSystemServer.socket,size)>0){
		log_info(logKernel, "Envio correctamente a FS el tamanio a escribir: %d",size);
	}else{
		log_info(logKernel, "Error al enviar a FS el tamanio a escribir: %d", size);
	}


}

void filesystem_close(){
	if(socket_send_string(fileSystemServer.socket,"CERRAR")>0){
		log_info(logKernel, "Envio correctamente a FS que quiero borrar");
	}else{
		log_info(logKernel, "Error al enviar a FS que quiero borrar");
	}
}

void filesystem_delete(){
	if(socket_send_string(fileSystemServer.socket,"BORRAR")>0){
		log_info(logKernel, "Envio correctamente a FS que quiero borrar");
	}else{
		log_info(logKernel, "Error al enviar a FS que quiero borrar");
	}
}

int filesystem_create(char* flags,char* path){
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

int filesystem_validar(char* path){
	//Le pido el file descriptor a FS
	if (socket_send_string(fileSystemServer.socket,"VALIDAR")>0){
		log_info(logKernel, "Le indico a FS que quiero validar una path");
	}else{
		log_info(logKernel, "Error al indicarle a FS que quiero validar una path");
	}
	//Envio el path a FS
	if (socket_send_string(fileSystemServer.socket,path)>0){
		log_info(logKernel, "Le envio a FS la path: %s",path);
	}else{
		log_info(logKernel, "Error al enviarle a FS la path: %s", path);
	}
	int respuesta;
	if (socket_recv_int(fileSystemServer.socket,&respuesta)>0){
		log_info(logKernel, "Recibo la validacion del FS");
		if (respuesta>0){
			log_info(logKernel, "Existe el archivo en FS");
		}else{
			log_info(logKernel, "No existe el archivo en FS");
			EXIT_FAILURE;
		}
	}
	return respuesta;
}
