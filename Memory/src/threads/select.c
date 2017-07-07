#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <commons/log.h>
#include <commons/string.h>

#include "../libSockets/server.h"
#include "../libSockets/recv.h"
#include "../commons/structures.h"
#include "../commons/declarations.h"
#include "request.h"

void socket_select_recv_package(fd_set* selectSockets, int socket, int byteCount, char* data)
{
    log_debug(logMemory, "select -> open_socket_thread (%d)\n", socket);

    // saco al socket de los sockets del select
    FD_CLR(socket, selectSockets);

    t_socket_thread_arg* args = malloc(sizeof(t_socket_thread_arg));
    args->socket = socket;
    args->command = data;
    args->nbytes = byteCount;

	pthread_create(&(args->threadId), NULL, &socket_thread_requests, args);

	pthread_mutex_lock(&threadsList->mutex);
	list_add(threadsList->list, &args->threadId);
	pthread_mutex_unlock(&threadsList->mutex);
}

void socket_select_connection_lost(fd_set* master, int socket, int byteCount)
{
    printf("lost connection to %d socket con byte count = %d\n", socket, byteCount);
    close(socket);
	FD_CLR(socket, master);
}

void* start_server(void* _)
{
    socket_server_select(configMemory->portNumStr, &socket_select_connection_lost, &socket_select_recv_package);

    return NULL;
}
