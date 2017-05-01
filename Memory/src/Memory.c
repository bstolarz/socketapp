#include <stdio.h>
#include <stdlib.h>
#include "commons/structures.h"
#include "commons/declarations.h"
#include "functions/config.h"
#include "functions/log.h"
#include "functions/memory.h"
#include "threads/select.h"
#include <commons/log.h>
#include "other/debug_console.h"
#include "other/test.h"
#include "pthread.h"

int main(int argc, char* argv[]){

	if (argc != 2){
		printf("Missing config path\n");
		return -1;
	}

	configMemory = malloc(sizeof(t_memory));
	config_read(argv[1]);

	logMemory = log_create_file();
	log_config();

	ERROR_NO_RESOURCES_FOR_PROCCESS = -1;
	ERROR_MEMORY = -5;

	memory_init();
	// test_cpu_connection();

	// server and console loop
	pthread_t serverThread;
	pthread_create(&serverThread, NULL, &start_server, NULL);
	debug_console();

	pthread_join(serverThread, NULL);

	config_free();
	free(pageTable);
	log_destroy(logMemory);
	
	return EXIT_SUCCESS;
}
