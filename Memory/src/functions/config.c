#include <stdio.h>
#include <stdlib.h>
#include <commons/string.h>
#include <commons/config.h>
#include "../commons/declarations.h"

void config_init(char* path){
	configMemory = malloc(sizeof(t_memory));
	t_config* config = config_create(path);

	configMemory->portNumStr = string_duplicate(config_get_string_value(config, "PUERTO"));
	configMemory->frameCount = config_get_int_value(config, "MARCOS");
	configMemory->frameSize = config_get_int_value(config, "MARCO_SIZE");
	configMemory->cacheEntryCount = config_get_int_value(config, "ENTRADAS_CACHE");
	configMemory->cachePerProccess = config_get_int_value(config, "CACHE_X_PROC");
	configMemory->cacheReplaceStrategy = string_duplicate(config_get_string_value(config, "REEMPLAZO_CACHE"));
	configMemory->responseDelay = config_get_int_value(config, "RETARDO_MEMORIA");

	config_destroy(config);
}

void config_print(){
	printf("PUERTO: %s\n",configMemory->portNumStr);
	printf("MARCOS: %i\n",configMemory->frameCount);
	printf("MARCO_SIZE: %i\n",configMemory->frameSize);
	printf("ENTRADAS_CACHE: %i\n",configMemory->cacheEntryCount);
	printf("CACHE_X_PROC: %i\n",configMemory->cachePerProccess);
	printf("REEMPLAZO_CACHE: %s\n",configMemory->cacheReplaceStrategy);
	printf("RETARDO_MEMORIA: %i\n",configMemory->responseDelay);
}

void config_memory_destroy()
{
	free(configMemory->cacheReplaceStrategy);
	free(configMemory->portNumStr);
	free(configMemory);
	configMemory = NULL;
}
