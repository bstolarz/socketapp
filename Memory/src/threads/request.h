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

void* socket_thread_requests(void* args);

