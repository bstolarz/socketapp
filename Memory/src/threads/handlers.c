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
#include "../functions/ram.h"

int handle_init(int clientSocket)
{
	//Recivo PID
	int PID;
	if(socket_recv_int(clientSocket, &PID) <= 0){
		log_error(logMemory, "[init] request: problema recviendo PID (socket %d)", clientSocket);
		return -1;
	}

	//Recivo page count
	int pageCount;
	if(socket_recv_int(clientSocket, &pageCount) <= 0){
		log_error(logMemory, "[init] request: problema recviendo pageCount (socket %d)", clientSocket);
		return -1;
	}

	//Proceso la peticion
	int success = ram_program_init(PID, pageCount);

	//Envio el resultado
	int nBytes = socket_send_int(clientSocket, success);
	if (nBytes <= 0){
		log_error(logMemory, "[init] request: problema send result (PID: %d, pageCount: %d, success: %d, socket: %d)", PID, pageCount, success, clientSocket);
	}

	return 0;
}

int handle_end(int clientSocket){
	//Recivo PID
	int PID;
	if(socket_recv_int(clientSocket, &PID) <= 0){
		log_error(logMemory, "[end] request: problema reciviendo PID (socket %d)", clientSocket);
		return -1;
	}

	//Proceso la peticion
	program_end(PID);

	//Envio el resultado
	int nBytes = socket_send_int(clientSocket, 0);
	if (nBytes<=0){
		log_error(logMemory, "[end] request: problema send (PID: %d, socket %d)", PID, clientSocket);
		return -1;
	}

	return 0;
}

int handle_read(int clientSocket)
{
	//Recivo PID
	int PID;
	if(socket_recv_int(clientSocket, &PID) <= 0){
		log_error(logMemory, "[end] request: problema reciviendo PID (socket %d)", clientSocket);
		return -1;
	}

	//Recivo Page
	int page;
	if(socket_recv_int(clientSocket, &page) <= 0){
		log_error(logMemory, "[read] request: problema reciviendo page (socket %d)", clientSocket);
		return -1;
	}

	//Recivo offset
	int offset;
	if(socket_recv_int(clientSocket, &offset) <= 0){
		log_error(logMemory, "[read] request: problema reciviendo offset (socket %d)", clientSocket);
		return -1;
	}

	//Recivo size
	int size;
	if(socket_recv_int(clientSocket, &size) <= 0){
		log_error(logMemory, "[read] request: problema reciviendo size (socket %d)", clientSocket);
		return -1;
	}

	//Proceso la peticion
	/*
	 * TODO
	 * aca es necesario que memory_read devulva un int con el estado de la lectura
	 * los bytes que se leyeron, o un numero negativo si no pudo leerse
	 *
	 * Primero se enviara este estado y despues se enviara la data
	 * Luego si se envio la data, se hara el free
	 */
    void* data = memory_read(PID, page, offset, size);

    //Envio la respuesta
    int nBytes = socket_send(clientSocket, data, size);
    if (nBytes == -1){
    	log_error(logMemory, "[read] request: problema en send (PID %d, page %d, offset %d, size %d, socket %d)", PID, page, offset, size, clientSocket);
    	return -1;
    }

    if (data != NULL) free(data);

    return 0;
}

int handle_write(int clientSocket)
{
	//Recivo PID
	int PID;
	if(socket_recv_int(clientSocket, &PID) <= 0){
		log_error(logMemory, "[write] request: problema reciviendo PID (socket %d)", clientSocket);
		return -1;
	}

	//Recivo page
	int page;
	if(socket_recv_int(clientSocket, &page) <= 0){
		log_error(logMemory, "[write] request: problema reciviendo page (socket %d)", clientSocket);
		return -1;
	}

	//Recivo page
	int offset;
	if(socket_recv_int(clientSocket, &offset) <= 0){
		log_error(logMemory, "[write] request: problema reciviendo offset (socket %d)", clientSocket);
		return -1;
	}

	//Recivo buffer
	void* buffer;
	int size;
	if((size = socket_recv(clientSocket, &buffer, 1)) <= 0){
		log_error(logMemory, "[write] request: problema reciviendo buffer (socket %d)", clientSocket);
		return -1;
	}

	//Proceso la solicitud
    int writeResult = memory_write(PID, page, offset, size, buffer);

    //Envio la respuesta
    int nBytes = socket_send_int(clientSocket, writeResult);
    if (nBytes <= 0){
    	log_debug(	logMemory,"[write] problema send result (PID: %d, page: %d, offset: %d, size: %d, socket %d)", PID, page, offset, size, clientSocket);
    	return -1;
    }

    free(buffer);

    return 0;
}

int handle_frame_size(int clientSocket){
	int nBytes = socket_send_int(clientSocket, configMemory->frameSize);
	if (nBytes <= 0){
		log_error(logMemory, "[frame_size] request: send malo a socket %s (socket %d)", clientSocket);
		return -1;
	}

	return 0;
}


