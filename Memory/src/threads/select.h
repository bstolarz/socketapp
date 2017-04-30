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

void* start_server(void*);

#endif /* THREADS_SELECT_H_ */
