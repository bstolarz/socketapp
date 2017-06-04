/*
 * config.c
 *
 *  Created on: 4/4/2017
 *      Author: utnso
 */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
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

	log_info("Se leyo el metadata del archivo: %s", path);
}

void write_metadataFS(char* path, t_metadata_archivo* archivo, int nroBloque){
	//Agrego bloque
	list_add(archivo->bloques, nroBloque);


	if (!remove(path)){
		/*
		int fd = open(path, O_CREAT);
		char * strMetadata = string_new();

		string_append(&strMetadata, "TAMANIO=250\n");
		string_append(&strMetadata, "Metadata/Bitmap.bin");
		*/
		FILE *fp = fopen(path, "w+");

		char * strToWrite = string_new();

		string_append(&strToWrite, "TAMANIO=");
		string_append(&strToWrite,(char*)archivo->tamanio);
		string_append(&strToWrite, "\n");
		string_append(&strToWrite, "BLOQUES=[");

		int i;
		for(i=0;i<archivo->bloques->elements_count;i++)
		{
			string_append_with_format(&strToWrite, "%i", list_get(archivo->bloques, i));

		}

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

