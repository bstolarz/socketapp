/*
 * config.c
 *
 *  Created on: 3/4/2017
 *      Author: utnso
 */

#include <stdio.h>
#include <stdlib.h>
#include <commons/string.h>
#include <commons/config.h>
#include "../commons/declarations.h"

void config_read(char* path){
	t_config* config = config_create(path);

	configMemory->portNum = config_get_int_value(config, "PUERTO");
	configMemory->frameCount = config_get_int_value(config, "MARCOS");
	configMemory->frameSize = config_get_int_value(config, "MARCO_SIZE");
	configMemory->cacheEntryCount = config_get_int_value(config, "ENTRADAS_CACHE");
	configMemory->cachePerProccess = config_get_int_value(config, "CACHE_X_PROC");
	configMemory->cacheReplaceStrategy = string_duplicate(config_get_string_value(config, "REEMPLAZO_CACHE"));
	configMemory->responseDelay = config_get_int_value(config, "RETARDO_MEMORIA");

	config_destroy(config);
}

void config_print(){
	printf("PUERTO: %i\n",configMemory->portNum);
	printf("MARCOS: %i\n",configMemory->frameCount);
	printf("MARCO_SIZE: %i\n",configMemory->frameSize);
	printf("ENTRADAS_CACHE: %i\n",configMemory->cacheEntryCount);
	printf("CACHE_X_PROC: %i\n",configMemory->cachePerProccess);
	printf("REEMPLAZO_CACHE: %s\n",configMemory->cacheReplaceStrategy);
	printf("RETARDO_MEMORIA: %i\n",configMemory->responseDelay);
}
