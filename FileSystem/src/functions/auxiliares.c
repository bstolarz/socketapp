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


void crearArchivo(char* path, int posBloqueLibre){

	FILE* archivo = fopen(path, "w");
	//Cuando creo el bloque de datos va a tener 0 bytes o 1 byte?
	fprintf(archivo, "TAMANIO=0\n");

	char* lineaBloques = "BLOQUES=[";
	char* bloque = "";
	sprintf(bloque, "%d", posBloqueLibre);

	string_append(&lineaBloques, bloque);
	//Me queda BLOQUES=[80
	string_append(&lineaBloques, "]");
	//Me queda BLOQUES=[80]

	fprintf(archivo, "%s", lineaBloques);
	fclose(archivo);
}

void eliminarMetadataArchivo(char* path){
	remove(path);
}
void crearBloqueDatos(int posBloqueLibre){
	//Abstraer desde aca asi lo reuso en eliminarBloqueDatos
	char* path = configFileSystem->punto_montaje;
	string_append(&path, "Bloques/");

	char* bloque = "";
	sprintf(bloque, "%d.bin", posBloqueLibre);
	string_append(&path, bloque);
	//Hasta aca

	FILE* archivoBloqueDatos = fopen(path, "w");
	fclose(archivoBloqueDatos);
}

void eliminarBloqueDatos(int bloque){
	//Abstraer desde aca
	char* path = configFileSystem->punto_montaje;
	string_append(&path, "Bloques/");

	char* bloqueD = "";
	sprintf(bloqueD, "%d.bin", bloque);
	string_append(&path, bloqueD);
	//Hasta aca

	remove(path);
}
