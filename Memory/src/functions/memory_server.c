#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <commons/log.h>

#include "../libSockets/server.h"
#include "../libSockets/recv.h"
#include "../libSockets/send.h"

#include "../commons/structures.h"
#include "../commons/declarations.h"
#include "memory.h"

void handle_connection_lost(fd_set* master, int socket, int byteCount)
{
    printf("lost connection to %d socket con byte count = %d\n", socket, byteCount);
    close(socket);
	FD_CLR(socket, master); // eliminar del conjunto maestro
}

void handle_init(int clientSocket)
{
	int PID, pageCount, success;

	socket_recv_int(clientSocket, &PID);
	socket_recv_int(clientSocket, &pageCount);

	success = program_init(PID, pageCount);

	socket_send_int(clientSocket, success);
}

void handle_end(int clientSocket)
{
	int PID;

	socket_recv_int(clientSocket, &PID);

	program_end(PID);

	socket_send_int(clientSocket, 1); // solo para coordinar que llego, no se si es necesario
}

void handle_read(int socket)
{
    int PID, page, offset, size;

    socket_recv_int(socket, &PID);
    socket_recv_int(socket, &page);
    socket_recv_int(socket, &offset);
    socket_recv_int(socket, &size);

    void* data = memory_read(PID, page, offset, size);

    socket_send(socket, data, size);
}

void handle_write(int socket)
{
    int PID, page, offset, size;
    
    socket_recv_int(socket, &PID);
    socket_recv_int(socket, &page);
    socket_recv_int(socket, &offset);
    socket_recv_int(socket, &size);

    void* data;

    socket_recv(socket, &data, size);
    memory_write(PID, page, offset, size, data);

    // socket_send_int(socket, 0); // si no mandar error
}

void handle_frame_size(int clientSocket)
{
	socket_send_int(clientSocket, configMemory->frameSize);
}

void handle_command(fd_set* sockets, int socket, int byteCount, char* data)
{
    log_debug(memoryLog, "recibi commando %s de socket (%d)\n", data, socket);
    
    if (strcmp("read", data) == 0)
        handle_read(socket);

    else if (strcmp("write", data) == 0)
        handle_write(socket);
    
    else if (strcmp("init", data) == 0)
        handle_init(socket);
    
    else if (strcmp("end", data) == 0)
        handle_end(socket);
    
    else if (strcmp("frame_size?", data) == 0)
    	handle_frame_size(socket);

    else
        log_warning(memoryLog, "no existe el commando %s", data);
}

void start_server() {
    socket_server_select(configMemory->portNumStr,
                         &handle_connection_lost,
                         &handle_command);
}
