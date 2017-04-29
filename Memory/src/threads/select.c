#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <commons/log.h>

#include "../libSockets/server.h"
#include "../libSockets/recv.h"
#include "../libSockets/send.h"

#include "../commons/structures.h"
#include "../commons/declarations.h"
#include "handlers.h"
#include "../functions/memory.h"

typedef void* (*request_handler)(void*);

void handle_connection_lost(fd_set* master, int socket, int byteCount){
    printf("lost connection to %d socket con byte count = %d\n", socket, byteCount);
    close(socket);
	FD_CLR(socket, master); // eliminar del conjunto maestro
}

void recv_request(int clientSocket, int byteCount, char* data, request_handler* requestHandlerPtr, void** requestPtr)
{
	if (strcmp("read", data) == 0)
	{
		t_read_request* readRequest = malloc(sizeof(t_read_request));
		readRequest->clientSocket = clientSocket;
		socket_recv_int(clientSocket, &(readRequest->PID));
	    socket_recv_int(clientSocket, &(readRequest->page));
		socket_recv_int(clientSocket, &(readRequest->offset));
		socket_recv_int(clientSocket, &(readRequest->size));

		*requestPtr = (void*) readRequest;
		*requestHandlerPtr = handle_read;

		log_debug(logMemory, "read request: PID %d page %d offset %d size %d (socket %d)",
				readRequest->PID, readRequest->page, readRequest->offset, readRequest->size, readRequest->clientSocket);
	}
	else if (strcmp("write", data) == 0)
	{
		t_write_request* writeRequest = malloc(sizeof(t_write_request));
		writeRequest->clientSocket = clientSocket;
		socket_recv_int(clientSocket, &(writeRequest->PID));
		socket_recv_int(clientSocket, &(writeRequest->page));
		socket_recv_int(clientSocket, &(writeRequest->offset));
		socket_recv_int(clientSocket, &(writeRequest->size));
	    socket_recv(clientSocket, &(writeRequest->buffer), writeRequest->size);

		*requestPtr = (void*) writeRequest;
		*requestHandlerPtr = handle_write;

		log_debug(logMemory, "write request: PID %d page %d offset %d size %d (socket %d)",
				writeRequest->PID, writeRequest->page, writeRequest->offset, writeRequest->size, writeRequest->clientSocket);
	}
	else if (strcmp("init", data) == 0)
	{
		t_init_program_request* initRequest = malloc(sizeof(t_init_program_request));
		initRequest->clientSocket = clientSocket;
		socket_recv_int(clientSocket, &(initRequest->PID));
		socket_recv_int(clientSocket, &(initRequest->pageCount));

		*requestPtr = (void*) initRequest;
		*requestHandlerPtr = handle_init;

		log_debug(logMemory, "init request: PID %d pageCount %d (socket %d)", initRequest->PID, initRequest->pageCount, initRequest->clientSocket);
	}
	else if (strcmp("end", data) == 0)
	{
		t_end_program_request* endRequest = malloc(sizeof(t_end_program_request));
		endRequest->clientSocket = clientSocket;
		socket_recv_int(clientSocket, &(endRequest->PID));

		*requestPtr = (void*) endRequest;
		*requestHandlerPtr = handle_end;

		log_debug(logMemory, "end request: PID %d (socket %d)", endRequest->PID, endRequest->clientSocket);
	}
	else if (strcmp("frame_size", data) == 0)
	{
		int* clientSocketPtr = malloc(sizeof(int));
		*clientSocketPtr = clientSocket;

		*requestPtr = (void*) clientSocketPtr;
		*requestHandlerPtr = handle_frame_size;
	}
}

void handle_command(fd_set* sockets, int socket, int byteCount, char* data){
    log_debug(logMemory, "recibi commando %s de socket (%d)\n", data, socket);

    request_handler requestHandler = NULL;
    void* request; // argumento de los threads. hago free de esto en la funcion thread

    recv_request(socket, byteCount, data, &requestHandler, &request);

    // empezar threads
    pthread_t requestThread;
    pthread_attr_t attr;

    // hago detachable al thread
    // (porque este proceso vive hasta que le tires ctrl c)
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    if (requestHandler != NULL)
		pthread_create(&requestThread, &attr, requestHandler, request);
    	//requestHandler(socketDyn); // sin thread por si algo anda mal
    else
    	log_warning(logMemory, "no existe el commando %s", data);
}

void* start_server(void* _) {
    socket_server_select(configMemory->portNumStr, &handle_connection_lost, &handle_command);

    return NULL;
}
