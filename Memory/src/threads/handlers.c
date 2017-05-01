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

void handle_init(int clientSocket)
{
	// recibir
	int PID, pageCount;

	if (socket_recv_int(clientSocket, &PID) == -1 ||
		socket_recv_int(clientSocket, &pageCount) == -1)
	{
		log_error(logMemory, "[init] request: problema recviendo datos (socket %d)", clientSocket);
		return;
	}


	// procesar
	int success = program_init(PID, pageCount);


	// enviar
	int sendResult = socket_send_int(clientSocket, success);

	if (sendResult == -1)
		log_error(logMemory, 	"[init] request: problema send result (PID: %d, pageCount: %d, success: %d, socket: %d)",
								PID, pageCount, success, clientSocket);
}

void handle_end(int clientSocket)
{
	// recibir
	int PID;

	if (socket_recv_int(clientSocket, &PID) == -1)
	{
		log_error(logMemory, "[end] request: problema recviendo datos (socket %d)", clientSocket);
		return;
	}


	// procesar
	program_end(PID);


	// enviar
	int sendResult = socket_send_int(clientSocket, 0);

	if (sendResult == -1)
		log_error(logMemory, "[end] request: problema send (PID: %d, socket %d)", PID, clientSocket);
}

void handle_read(int clientSocket)
{
	// recibir
	int PID, page, offset, size;

	if (socket_recv_int(clientSocket, &PID) == -1 ||
		socket_recv_int(clientSocket, &page) == -1 ||
		socket_recv_int(clientSocket, &offset) == -1 ||
		socket_recv_int(clientSocket, &size) == -1)
	{
		log_error(logMemory, "[read] request: problema recviendo datos (socket %d)", clientSocket);
		return;
	}


	// procesar
    void* data = memory_read(PID, page, offset, size);


    // responder
    int sendResult = socket_send(clientSocket, data, size); // TODO: si data es nula hay que mandar un void* de size 0s?

    if (sendResult == -1)
    {
    	log_error(	logMemory,
    				"[read] request: problema en send (PID %d, page %d, offset %d, size %d, socket %d)",
        			PID, page, offset, size, clientSocket);
    }

    if (data != NULL) free(data);
}

void handle_write(int clientSocket)
{
    // recibir
	int PID, page, offset;
	void* buffer;

	if (socket_recv_int(clientSocket, &PID) == -1 ||
		socket_recv_int(clientSocket, &page) == -1 ||
		socket_recv_int(clientSocket, &offset) == -1)
	{
		log_error(logMemory, "[write] request: problema recviendo posicion memoria (socket %d)", clientSocket);
		return;
	}

	int size = socket_recv(clientSocket, &buffer, 1);

	if (size == -1)
	{
		log_error(logMemory, "[write] request: problema recviendo datos (socket %d)", clientSocket);
		return;
	}

	// procesar
    int writeResult = memory_write(PID, page, offset, size, buffer);

    // responder
    int sendResult = socket_send_int(clientSocket, writeResult);

    if (sendResult == -1)
    {
    	log_debug(	logMemory,
    				"[write] problema send result (PID: %d, page: %d, offset: %d, size: %d, socket %d)",
    				PID, page, offset, size, clientSocket);
    }

    free(buffer);
}

void handle_frame_size(int clientSocket)
{
	int sendResult = socket_send_int(clientSocket, configMemory->frameSize);

	if (sendResult == -1)
		log_error(logMemory, "[frame_size] request: send malo a socket %s (socket %d)", clientSocket);
}


