#include <commons/config.h>
#include <commons/log.h>
#include <commons/string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "memory_config.h"

memory_config* createConfig(char* path)
{
	t_config* config = config_create(path);
	memory_config* memoryConfig = malloc(sizeof(memory_config));

	memoryConfig->portNum = config_get_int_value(config, "PUERTO");
	
	memoryConfig->frameCount = config_get_int_value(config, "MARCOS");
	memoryConfig->frameSize = config_get_int_value(config, "MARCO_SIZE");
	memoryConfig->responseDelay = config_get_int_value(config, "RETARDO_MEMORIA"); 

	memoryConfig->cacheEntryCount = config_get_int_value(config, "ENTRADAS_CACHE"); 
	memoryConfig->cachePerProccess = config_get_int_value(config, "CACHE_X_PROC");
	memoryConfig->cacheReplaceStrategy = string_duplicate(config_get_string_value(config, "REEMPLAZO_CACHE"));
		
	config_destroy(config);

	return memoryConfig;
}

int main(int argc, char* argv[])
{
	char* configPath;

	if (argc != 2)
	{
		printf("necesito un path del config de argument\nusando un default\n");
		configPath = "default_config.txt";
	}
	else
	{
		configPath = argv[1];
	}

	t_log* logger;
	memory_config* config;
	
	logger = log_create("memory_log.txt", "Memory", false, LOG_LEVEL_DEBUG);
	config = createConfig(configPath);

	log_debug(	logger,
				"memory config:\npuerto = %d\ncant frames = %d\ntamano frame = %d\ndelay = %d",
				config->portNum, config->frameCount, config->frameSize, config->responseDelay);
	log_debug(	logger,
				"cache config:\nentradas cache = %d\ncache x proc = %d\nalgo reemplazo = %s\n",
				config->cacheEntryCount, config->cachePerProccess, config->cacheReplaceStrategy);


	log_destroy(logger);
	free(config->cacheReplaceStrategy);
	free(config);

	return EXIT_SUCCESS;
}
