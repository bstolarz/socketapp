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
