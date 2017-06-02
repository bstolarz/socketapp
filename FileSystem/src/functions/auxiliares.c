#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <commons/config.h>
#include <commons/string.h>
#include <string.h>
#include "bitmap.h"
#include "../commons/structures.h"
#include "../commons/declarations.h"

char* armarPathArchivo(char* pathDelKernel){
	char* pathTotal = "";
	string_append(&pathTotal, configFileSystem->punto_montaje);
	string_append(&pathTotal, "Archivos/");
	string_append(&pathTotal, pathDelKernel);

	return pathTotal;
}

char* armarPathBloqueDatos(int numeroBloque) {
	char* pathTotal = "";
	string_append(&pathTotal, configFileSystem->punto_montaje);
	string_append(&pathTotal, "Bloques/");

	char* bloqueDato = "";
	sprintf(bloqueDato, "%d.bin", numeroBloque);
	string_append(&pathTotal, bloqueDato);

	return pathTotal;
}

void crearArchivo(char* path, int posBloqueLibre){
	FILE* archivo = fopen(path, "w");
	fprintf(archivo, "TAMANIO=0\n");

	char* lineaBloques = "BLOQUES=[";
	char* bloque = "";
	sprintf(bloque, "%d", posBloqueLibre);

	string_append(&lineaBloques, bloque);
	string_append(&lineaBloques, "]");

	fprintf(archivo, "%s", lineaBloques);
	fclose(archivo);
}

void eliminarMetadataArchivo(char* path){
	remove(path);
}

void crearBloqueDatos(int posBloqueLibre){
	char* pathBloqueDato = armarPathBloqueDatos(posBloqueLibre);

	FILE* archivoBloqueDatos = fopen(pathBloqueDato, "w");
	fclose(archivoBloqueDatos);
}

int avanzarBloquesParaEscribir (int bloqueInicial,int desplazamientoLimite){
	//Implementarla
	return 0;
}

void actualizarBytesEscritos (int* acum, int bytes){
	*acum += bytes;
}
