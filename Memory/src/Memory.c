#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commons/structures.h"
#include "commons/declarations.h"
#include "functions/config.h"
#include "functions/memory.h"
#include "functions/test.h"
#include "functions/memory_server.h"
#include <commons/log.h>
#include "libSockets/client.h"
#include "libSockets/send.h"
#include "libSockets/recv.h"


void init(char* configPath)
{
	memoryLog = log_create("memory_log.txt", "Memory", false, LOG_LEVEL_DEBUG);

	// config init
	config_init(configPath);
	config_print();

	memory_init();
}

void cleanup()
{
	config_memory_destroy();
	free(pageTable);
	log_destroy(memoryLog);
}




int main(int argc, char* argv[]){

	if (argc != 2){
		printf("Missing config path\n");
		return -1;
	}

	init(argv[1]);
	program_init(0, 3);
	start_server();
	program_end(0);
	cleanup();
	
	return EXIT_SUCCESS;
}
