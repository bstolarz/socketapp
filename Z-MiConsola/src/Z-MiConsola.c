#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>
#include <commons/collections/list.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/log.h>
#include "commons/declarations.h"
#include "libSockets/send.h"
#include "libSockets/recv.h"
#include "libSockets/client.h"
#include "commons/structures.h"

int main(int argc, char* argv[]) {

	int serverSocket = 1;
	socket_client_create(&serverSocket, "127.0.0.1", "6670");
	socket_send_string(serverSocket, "CONSOLA");
	printf("Le mande mi identificador\n");

	return EXIT_SUCCESS;
}
