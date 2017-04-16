#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commons/structures.h"
#include "commons/declarations.h"
#include "functions/config.h"
#include "functions/log.h"
#include "functions/memory.h"
#include "functions/test.h"
#include "functions/memory_server.h"
#include <commons/log.h>
#include "libSockets/client.h"
#include "libSockets/send.h"
#include "libSockets/recv.h"

int main(int argc, char* argv[]){

	if (argc != 2){
		printf("Missing config path\n");
		return -1;
	}

	configMemory = malloc(sizeof(t_memory));
	config_read(argv[1]);
	//config_read("/home/utnso/git/tp-2017-1c-SocketApp/memory");

	logMemory = log_create_file();
	log_config();

	memory_init();
	start_server();

	config_free();
	free(pageTable);
	log_destroy(logMemory);
	
	return EXIT_SUCCESS;
}
