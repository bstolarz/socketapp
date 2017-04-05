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
	printf("IP KERNEL: %s\n",configConsole->ip_kernel);
	printf("PUERTO KERNEL: %s\n",configConsole->puerto_kernel);
}
