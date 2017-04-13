/*
 * config.c
 *
 *  Created on: 3/4/2017
 *      Author: utnso
 */

#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/string.h>
#include "../commons/declarations.h"

void config_read(char* path){
	t_config* config=config_create(path);

	configConsole->ip_kernel = string_duplicate(config_get_string_value(config,"IP_KERNEL"));
	configConsole->puerto_kernel = string_duplicate(config_get_string_value(config,"PUERTO_KERNEL"));

	config_destroy(config);
}

void config_print(){
	log_info(logConsole,"IP KERNEL: %s",configConsole->ip_kernel);
	log_info(logConsole,"PUERTO KERNEL: %s",configConsole->puerto_kernel);
}

void config_free(){
	free(configConsole->ip_kernel);
	free(configConsole->puerto_kernel);
	free(configConsole);
}
