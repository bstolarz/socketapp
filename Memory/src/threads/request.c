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
		}else if (strcmp("free_page", command) == 0){
			response = handle_free_page(request->socket);
		}else if (strcmp("frame_size", command) == 0){
			response = handle_frame_size(request->socket);
		} else {
			log_error(logMemory, "No logre comprender el comando: %s del socket %d", command, request->socket);
		}

		if(response>=0){
			free(command);
			command = NULL;
			nBytes = socket_recv_string(request->socket, &command);
		}else{
			nBytes = -1;
			log_error(logMemory, "Ocurrio un error al procesar la solicitud %s de %d", command, request->socket);
		}
	}

	// saco de lista de threads
	pthread_mutex_lock(&threadsList->mutex);
	_Bool same_thread(void* elem)
	{
		// puse un puntero
		pthread_t threadId = *((pthread_t*) elem);
		return threadId == request->threadId;
	};

	list_remove_by_condition(threadsList->list, same_thread);
	pthread_mutex_unlock(&threadsList->mutex);

	// Free's
	if (command != NULL) free(command);
	// request->command ya fue liberado por algun free(command)
	close(request->socket);
	free(request);

	return NULL;
}
