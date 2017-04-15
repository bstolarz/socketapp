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

void handle_read(int socket)
{
    int PID, page, offset, size;

    socket_recv_int(socket, &PID);
    socket_recv_int(socket, &page);
    socket_recv_int(socket, &offset);
    socket_recv_int(socket, &size);

    void* data = memory_read(PID, page, offset, size);

    // log_debug(memoryLog, "read: %d, %d, %d = %d", PID, page, offset, *((int*)(data)));

    socket_send(socket, data, size);
    // socket_send_int(socket, *((int*)(data)));
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

void handle_command(fd_set* sockets, int socket, int byteCount, char* data)
{
    printf("recibi commando %s de socket (%d)\n", data, socket);
    
    if (strcmp("read", data) == 0)
        handle_read(socket);

    else if (strcmp("write", data) == 0)
        handle_write(socket);
    
    else if (strcmp("init", data) == 0)
        log_debug(memoryLog, "llego commando iniciar programa");
    
    else if (strcmp("end", data) == 0)
        log_debug(memoryLog, "llego commando finalizar programa");
    
    else
        log_debug(memoryLog, "no existe el commando %s", data);
}

void start_server() {
    socket_server_select(configMemory->portNumStr,
                         &handle_connection_lost,
                         &handle_command);
}
