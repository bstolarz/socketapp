#include <stdio.h>
#include <stdlib.h>
#include <commons/string.h>
#include <commons/config.h>
#include "../commons/declarations.h"

void config_read(char* path){
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

void config_free()
{
	free(configMemory->cacheReplaceStrategy);
	free(configMemory->portNumStr);
	free(configMemory);
	configMemory = NULL;
}
