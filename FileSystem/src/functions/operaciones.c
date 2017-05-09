#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <commons/config.h>
#include "../commons/structures.h"
#include "../commons/declarations.h"

//Implementacion de operaciones, por ahora son dummies
int validar(char* path){
	return NULL;
}

int crear(char* path){
	return NULL;
}

int borrar(char* path){
	return NULL;
}

size_t obtenerDatos(char* path, off_t offset, size_t size){
	return NULL;
}

int guardarDatos(char* path, off_t offset, size_t size, void* buffer){
	return NULL;
}
