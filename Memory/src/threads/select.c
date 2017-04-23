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

void handle_connection_lost(fd_set* master, int socket, int byteCount){
    printf("lost connection to %d socket con byte count = %d\n", socket, byteCount);
    close(socket);
	FD_CLR(socket, master); // eliminar del conjunto maestro
}

void handle_command(fd_set* sockets, int socket, int byteCount, char* data){
    log_debug(logMemory, "recibi commando %s de socket (%d)\n", data, socket);

    if (strcmp("read", data) == 0){
    	handle_read(socket);
    }else if (strcmp("write", data) == 0){
    	handle_write(socket);
    }else if (strcmp("init", data) == 0){
        handle_init(socket);
    }else if (strcmp("end", data) == 0){
        handle_end(socket);
    }else if (strcmp("frame_size?", data) == 0){
    	handle_frame_size(socket);
    }else{
        log_warning(logMemory, "no existe el commando %s", data);
    }
}

void* start_server(void* _) {
    socket_server_select(configMemory->portNumStr, &handle_connection_lost, &handle_command);

    return NULL;
}
