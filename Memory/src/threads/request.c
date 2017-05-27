#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <commons/log.h>
#include <commons/string.h>

#include "../libSockets/server.h"
#include "../libSockets/recv.h"
#include "../commons/structures.h"
#include "../commons/declarations.h"
#include "handlers.h"

void* socket_thread_requests(void* args){
	t_socket_thread_arg* request = args;
	char* command = request->command;
	int nBytes = request->nbytes;
	int response;

	while (nBytes>0) {
		if (strcmp("read", command) == 0){
			response = handle_read(request->socket);
		}else if (strcmp("write", command) == 0){
			response = handle_write(request->socket);
		}else if (strcmp("init", command) == 0){
			response = handle_init(request->socket);
		}else if (strcmp("end", command) == 0){
			response = handle_end(request->socket);
		}else if (strcmp("get_pages", command) == 0){
			response = handle_get_pages(request->socket);
		}else if (strcmp("frame_size", command) == 0){
			response = handle_frame_size(request->socket);
		}else{
			log_error(logMemory, "No logre comprender el comando: %s del socket %d", command, request->socket);
		}

		if(response>=0){
			nBytes = socket_recv_string(request->socket, &command);
		}else{
			nBytes = -1;
			log_error(logMemory, "Ocurrio un error al procesar la solicitud %s de %d", command, request->socket);
		}
	}

	// Free's
	close(request->socket);
	free(request->command);
	free(request);

	//TODO falta borrar de la lista de threadsList

	return NULL;
}
