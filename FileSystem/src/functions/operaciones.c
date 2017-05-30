#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <commons/config.h>
#include "bitmap.h"
#include "config.h"
#include "../commons/structures.h"
#include "../commons/declarations.h"
#include "auxiliares.h"

//Implementacion de operaciones, por ahora son dummies
int validar(char* path){
	int resultado = access(path, F_OK);
	if(resultado == 0){
		log_info(logs, "Se encontro el archivo para el path: %s", path);
		return 1;
	}

	log_info(logs, "No se encontro el archivo para el path: %s", path);
	//No encontro el archivo
	return -ENOENT;
}

int crear(char* path){
	int posBloqueLibre = encontrarUnBloqueLibre();
	if (posBloqueLibre >= 0){
		ocuparBloqueLibre(posBloqueLibre);
		crearArchivo(path, posBloqueLibre);
		log_info(logs, "Se creo el archivo");
	}
	else{
		log_info(logs, "No se encontro un bloque libre en el bitmap");
		return -ENOENT;
	}
	return 0;
}

int borrar(char* path){
	//Valido que exista el archivo
	if(validar(path) == 1){
		t_metadata_archivo* archivo = malloc(sizeof(t_metadata_archivo));
			read_fileMetadata(path, archivo);

			list_iterate(archivo->bloques, (void*)liberarBloqueDelBitmap);
			eliminarMetadataArchivo(path);

			list_destroy(archivo->bloques);
			free(archivo);
	}
	else{
		log_info(logs, "No se encontro el archivo, por ende no se lo puede borrar");
	}

	return -ENOENT;
}

int obtenerDatos(char* path, off_t offset, size_t size){
	if(validar(path) == 1){
		//hago las cosas
	}
	else{
		log_info(logs, "No se encontro el archivo, por ende no se le puede obtener datos");
	}
	return -ENOENT;
}

int guardarDatos(char* path, off_t offset, size_t size, void* buffer){
	if(validar(path) == 1){
		//hago las cosas
	}
	else{
		log_info(logs, "No se encontro el archivo, por ende no se le puede guardar datos");
	}
	return -ENOENT;
}
