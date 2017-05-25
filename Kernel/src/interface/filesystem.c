#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <stdbool.h>

#include "../libSockets/server.h"
#include "../libSockets/client.h"
#include "../libSockets/recv.h"
#include "../libSockets/send.h"

#include "../commons/structures.h"
#include "../commons/declarations.h"

void filesystem_connect(){
	pthread_mutex_init(&(fileSystemServer.mutex),NULL);
	socket_client_create(&fileSystemServer.socket, configKernel->ip_fs, configKernel->puerto_fs);

	socket_send_string(fileSystemServer.socket, "Kernel");
}
