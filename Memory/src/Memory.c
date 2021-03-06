#include <stdio.h>
#include <stdlib.h>
#include "commons/structures.h"
#include "commons/declarations.h"
#include "functions/config.h"
#include "functions/log.h"
#include "threads/select.h"
#include <commons/log.h>
#include "other/debug_console.h"
#include "pthread.h"
#include "functions/memory.h"

int main(int argc, char* argv[]){

	if (argc != 2){
		printf("Missing config path\n");
		return -1;
	}


	configMemory = malloc(sizeof(t_memory));
	config_read(argv[1]);

	logMemory = log_create_file();
	log_config();

	threadsList = malloc(sizeof(t_queue));
	threadsList->list = list_create();
	pthread_mutex_init(&(threadsList->mutex),NULL);

	memory_init();

	pthread_t serverThread;
	pthread_create(&serverThread, NULL, &start_server, NULL);

	debug_console();

	pthread_join(serverThread, NULL);

	log_destroy(logMemory);
	memory_destroy();
	config_free();
	

	return EXIT_SUCCESS;
}
