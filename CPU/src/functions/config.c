#include <stdio.h>
#include <stdlib.h>
#include <commons/string.h>
#include <commons/config.h>
#include "../commons/declarations.h"

void config_read(char* path){

	t_config* config = config_create(path);

	configCPU->ip_kernel = string_duplicate(config_get_string_value(config,"IP_KERNEL"));
	configCPU->puerto_kernel = string_duplicate(config_get_string_value(config,"PUERTO_KERNEL"));

	configCPU->ip_memory = string_duplicate(config_get_string_value(config,"IP_MEMORY"));
	configCPU->puerto_memory = string_duplicate(config_get_string_value(config,"PUERTO_MEMORY"));

	config_destroy(config);
}

void config_free()
{
	free(configCPU->ip_kernel);
	free(configCPU->puerto_kernel);
	free(configCPU->ip_memory);
	free(configCPU->puerto_memory);

	configCPU = NULL;
}
