/*
 * config.c
 *
 *  Created on: 4/4/2017
 *      Author: utnso
 */
#include <stdio.h>
#include <stdlib.h>
#include <commons/string.h>
#include <commons/config.h>
#include "../commons/declarations.h"


void config_print(){
	printf("El puerto del FS: %d\n", configFileSystem->puerto);
	printf("Punto de montaje: %s\n", configFileSystem->punto_montaje);
}
void config_read(char* path){
	t_config* config = config_create(path);
	configFileSystem->puerto=config_get_int_value(config,"PUERTO");
	configFileSystem->punto_montaje=string_duplicate(config_get_string_value(config,"PUNTO_MONTAJE"));

	config_destroy(config);
}
