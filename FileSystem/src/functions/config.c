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
#include <commons/collections/list.h>
#include "../commons/declarations.h"


void config_print(){
	printf("El puerto del FileSystem: %s\n", configFileSystem->puerto);
	printf("Punto de montaje: %s\n", configFileSystem->punto_montaje);

}

void config_read(char* path){
	t_config* config = config_create(path);
	if(config==NULL)
		log_info(logs, "El config_create no encontro el path");

	configFileSystem->puerto=string_duplicate(config_get_string_value(config,"PUERTO"));
	configFileSystem->punto_montaje=string_duplicate(config_get_string_value(config,"PUNTO_MONTAJE"));

	config_destroy(config);
}

void config_free(){
	free(configFileSystem->puerto);
	free(configFileSystem->punto_montaje);
	free(configFileSystem);
}

void read_fileMetadata(char* path, t_metadata_archivo* archivo) {
	t_config* config = config_create(path);

	archivo->tamanio = config_get_int_value(config, "TAMANIO");

	char** s = config_get_array_value(config, "BLOQUES");

	archivo->bloques = list_create();
	while (*s != NULL) {
		int bloque = atoi(*s);

		list_add(archivo->bloques, bloque);

		s++;
	}

	config_destroy(config);
}


void metadataFS_print(){
	printf("Cantidad de bloques del FS: %i\n", configMetadata->cantidadBloques);
	printf("Tamanio de cada bloque: %i\n", configMetadata->tamanioBloques);
}

void metadataFS_read(char* path){
	t_config* config = config_create(path);

	configMetadata->tamanioBloques=config_get_int_value(config,"TAMANIO_BLOQUES");
	configMetadata->cantidadBloques=config_get_int_value(config,"CANTIDAD_BLOQUES");

	config_destroy(config);
}

void metadataFS_free(){
	free(configMetadata);
}

