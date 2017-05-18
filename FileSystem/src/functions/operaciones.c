#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <commons/config.h>
#include "../commons/structures.h"
#include "../commons/declarations.h"

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
	return 0;
}

int borrar(char* path){
	return -ENOENT;
}

int obtenerDatos(char* path, off_t offset, size_t size){
	return -ENOENT;
}

int guardarDatos(char* path, off_t offset, size_t size, void* buffer){
	return -ENOENT;
}
