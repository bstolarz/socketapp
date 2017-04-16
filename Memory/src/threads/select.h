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

#ifndef THREADS_SELECT_H_
#define THREADS_SELECT_H_

void handle_connection_lost(fd_set* master, int socket, int byteCount);
void handle_command(fd_set* sockets, int socket, int byteCount, char* data);
void start_server();

#endif /* THREADS_SELECT_H_ */
