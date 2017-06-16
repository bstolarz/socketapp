#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include "../libSockets/send.h"
#include "../libSockets/recv.h"
#include "../libSockets/client.h"
#include "../commons/structures.h"
#include "../commons/declarations.h"

int ERROR_MEMORY = -5;

void* memory_read(int PID, int page, int offset, int size)
{
	/*
	 * retorna una lo que leyo. si hubo error, retorna NULL
	 */

    if ((socket_send_string(serverMemory, "read") > 0) &&
        (socket_send_int(serverMemory, PID) > 0) &&
        (socket_send_int(serverMemory, page) > 0) &&
        (socket_send_int(serverMemory, offset) > 0) &&
        (socket_send_int(serverMemory, size) > 0))
    {
    	int readResult;
    	if (socket_recv_int(serverMemory, &readResult) == sizeof(int))
    	{
    		if (readResult == size)
    		{
    			void* data;

				if (socket_recv(serverMemory, &data, 1) == size)
					return data;
    		}
    	}
    }

    return NULL;
}

int memory_write(int PID, int page, int offset, int size, void* buffer)
{
	/*
	 * retorn size si pudo;
	 * si no -1 si hubo problema mandando o recviendo; ERROR_MEMORY si memoria no pudo escribir
	 */
    if ((socket_send_string(serverMemory, "write") > 0) &&
        (socket_send_int(serverMemory, PID) > 0) &&
        (socket_send_int(serverMemory, page) > 0) &&
        (socket_send_int(serverMemory, offset) > 0) &&
        (socket_send(serverMemory, buffer, size) == size))
    {
    	int writeResult;
    	int recvResult = socket_recv_int(serverMemory, &writeResult);

    	if (recvResult == -1) return -1;
    	return writeResult;
    }
    
    return -1;
}

int memory_frame_size()
{
	/*
	 * retorna tamano del frame. si hubo error retorna -1
	 */

	int connectionResult = socket_send_string(serverMemory, "frame_size");
    if (connectionResult == -1) return -1;

    int frameSize = -1;
    connectionResult = socket_recv_int(serverMemory, &frameSize); // -1 si no pudo conectarse; si no
    if (connectionResult == -1) return -1;

    return frameSize;
}

void memory_connect(){


	socket_client_create(&serverMemory, configCPU->ip_memory, configCPU->puerto_memory);

	// obtener tamanio de pagina
	pageSize = memory_frame_size(serverMemory);

	if (pageSize > 0) {
		log_info(logCPU,"[page_size] se obtuvo pageSize = %d\n", pageSize);
	} else {
		log_error(logCPU, "[page_size] no se mando bien. exit program\n");
		exit(EXIT_FAILURE);
	}
}
