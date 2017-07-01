/*
 * config.c
 *
 *  Created on: 4/4/2017
 *      Author: utnso
 */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
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

	string_append(&(configFileSystem->punto_montaje), "/");

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

	log_info(logs, "Se leyo el metadata del archivo: %s", path);
}

void metadataFS_write(char* path, t_metadata_archivo* archivo, int nroBloque){

	log_info(logs, "Entra a metadataFS_write");
	if (!remove(path)){

		log_info(logs, "Entra a Borrar archivo");
		FILE *fp = fopen(path, "w+");
		log_info(logs, "creo archivo!!x");
		char * strToWrite = string_new();

		string_append(&strToWrite, "TAMANIO=");
		string_append_with_format(&strToWrite, "%i", archivo->tamanio);
		string_append(&strToWrite, "\n");
		string_append(&strToWrite, "BLOQUES=[");
		log_info(logs, "%s", strToWrite);
		int i;
		for(i=0;i<archivo->bloques->elements_count;i++)
		{
			string_append_with_format(&strToWrite, "%i,", list_get(archivo->bloques, i));
		}

		log_info(logs, "%s", strToWrite);
		strToWrite[(int)strlen(strToWrite)-1] = ']';
		fprintf(fp, strToWrite);

		fclose(fp);
		free(strToWrite);
	}

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

