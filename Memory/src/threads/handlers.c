#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <commons/log.h>

#include "../libSockets/server.h"
#include "../libSockets/recv.h"
#include "../libSockets/send.h"

#include "../commons/structures.h"
#include "../commons/declarations.h"
#include "../functions/memory.h"
#include "handlers.h"

void* handle_init(void* request)
{
	t_init_program_request* initRequest = (t_init_program_request*) request;
	int success = program_init(initRequest->PID, initRequest->pageCount);

	socket_send_int(initRequest->clientSocket, success);

	free(request);

	return NULL;
}

void* handle_end(void* request)
{
	t_end_program_request* endRequest = (t_end_program_request*) request;

	program_end(endRequest->PID);

	socket_send_int(endRequest->clientSocket, 0);

	free(request);

	return NULL;
}

void* handle_read(void* request)
{
	t_read_request* readRequest = (t_read_request*) request;

    void* data = memory_read(readRequest->PID, readRequest->page, readRequest->offset, readRequest->size);

    socket_send(readRequest->clientSocket, data, readRequest->size);

    free(request);

    return NULL;
}

void* handle_write(void* request)
{
	t_write_request* writeRequest = (t_write_request*) request;

    memory_write(writeRequest->PID, writeRequest->page, writeRequest->offset, writeRequest->size, writeRequest->buffer);
    // socket_send_int(socket, 0); // si no mandar error

    free(request);

    return NULL;
}

void* handle_frame_size(void* request)
{
	int clientSocket = *((int*) request);
	socket_send_int(clientSocket, configMemory->frameSize);

	free(request);

	return NULL;
}


