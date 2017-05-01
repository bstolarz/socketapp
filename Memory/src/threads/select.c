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


request_handler command_to_handler(char* command)
{
	if (strcmp("read", command) == 0)
	{
		return &handle_read;
	}
	else if (strcmp("write", command) == 0)
	{
		return &handle_write;
	}
	else if (strcmp("init", command) == 0)
	{
		return &handle_init;
	}
	else if (strcmp("end", command) == 0)
	{
		return &handle_end;
	}
	else if (strcmp("frame_size", command) == 0)
	{
		return &handle_frame_size;
	}
	else
	{
		return NULL;
	}
}

struct socket_thread_arg
{
	int clientSocket;
	char* command;
};

void* socket_thread(void* socketAndCommandVoid)
{
	struct socket_thread_arg* socketAndCommand = (struct socket_thread_arg*) socketAndCommandVoid;
	int clientSocket = socketAndCommand->clientSocket;
	char* command = socketAndCommand->command;
	int recvCommandResult = string_length(command);
	request_handler handler = command_to_handler(command);

	while (handler != NULL)
	{
		handler(clientSocket);
		recvCommandResult = socket_recv_string(clientSocket, &command);

		if (recvCommandResult == -1) break;
		handler = command_to_handler(command);
	}

	log_info(logMemory, "socket (%d) close. thread will end", clientSocket);

	// liberar cosas
	close(clientSocket);

	free(socketAndCommand->command);
	free(socketAndCommand);

	// el thread termina solo porque es detachable

	return NULL;
}

void open_socket_thread(fd_set* selectSockets, int socket, int byteCount, char* data)
{
    log_debug(logMemory, "select -> open_socket_thread (%d)\n", socket);

    // saco al socket de los sockets del select
    FD_CLR(socket, selectSockets);

    pthread_t socketThread;
    pthread_attr_t attr;

    // hago detachable al thread
    // (porque este proceso vive hasta que le tires ctrl c)
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    struct socket_thread_arg* socketAndCommand = malloc(sizeof(struct socket_thread_arg));
    socketAndCommand->clientSocket = socket;
    socketAndCommand->command = string_duplicate(data); // por las dudas que el select lo destruya

	pthread_create(&socketThread, &attr, &socket_thread, socketAndCommand);
}

void handle_connection_lost(fd_set* master, int socket, int byteCount)
{
    printf("lost connection to %d socket con byte count = %d\n", socket, byteCount);
    close(socket);
	FD_CLR(socket, master); // eliminar del conjunto maestro
}

void* start_server(void* _)
{
    socket_server_select(configMemory->portNumStr, &handle_connection_lost, &open_socket_thread);

    return NULL;
}
