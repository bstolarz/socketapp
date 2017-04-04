/*
 * config.c
 *
 *  Created on: 3/4/2017
 *      Author: utnso
 */

#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include "../commons/declarations.h"

void config_read(char* path){
	t_config* config=config_create(path);
	configConsole->ip_kernel = config_get_string_value(config,"IP_KERNEL");
	configConsole->puerto_kernel = config_get_string_value(config,"PUERTO_KERNEL");
}

void config_print(){
	printf("La ip del kernel es: %s\n",configConsole->ip_kernel);
	printf("El puerto del kernel es: %s\n",configConsole->puerto_kernel);
}
