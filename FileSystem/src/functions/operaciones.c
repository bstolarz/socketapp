#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <commons/config.h>
#include "../commons/structures.h"
#include "../commons/declarations.h"

//Implementacion de operaciones, por ahora son dummies
int validar(char* path){
	int resultado = access(path, F_OK);
	return resultado;
}

int crear(char* path){
	return 0;
}

int borrar(char* path){
	return 0;
}

int obtenerDatos(char* path, off_t offset, size_t size){
	return 0;
}

int guardarDatos(char* path, off_t offset, size_t size, void* buffer){
	return 0;
}
