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
		return 1;
	}

	//No encontro el archivo
	return -ENOENT;
}

int crear(char* path){
	int posBloqueLibre = encontrarUnBloqueLibre();
	if (posBloqueLibre >= 0){
		ocuparBloqueLibre(posBloqueLibre);
		crearArchivo(path, posBloqueLibre);
		crearBloqueDatos(posBloqueLibre);
	}
	else{
		return -ENOENT;
	}
	return 0;
}

int borrar(char* path){
	t_metadata_archivo* archivo = malloc(sizeof(t_metadata_archivo));
	read_fileMetadata(path, archivo);

	int i;
	for(i=0; i<list_size(archivo->bloques); i++){
		int* bloque = list_get(archivo->bloques, i);
		liberarBloqueDelBitmap(*bloque);
		eliminarBloqueDatos(*bloque);
		eliminarMetadataArchivo(path);
	}

	list_destroy(archivo->bloques);
	free(archivo);
	return -ENOENT;
}

int obtenerDatos(char* path, off_t offset, size_t size){
	return -ENOENT;
}

int guardarDatos(char* path, off_t offset, size_t size, void* buffer){
	return -ENOENT;
}
