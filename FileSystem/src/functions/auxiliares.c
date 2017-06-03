#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <commons/config.h>
#include <commons/string.h>
#include <string.h>
#include "bitmap.h"
#include "../commons/structures.h"
#include "../commons/declarations.h"


char* armarPathArchivo(char* pathDelKernel){
	char* pathTotal = string_new();
	string_append(&pathTotal, configFileSystem->punto_montaje);
	string_append(&pathTotal, "Archivos/");
	string_append(&pathTotal, pathDelKernel);

	return pathTotal;
}

char* armarPathBloqueDatos(int numeroBloque) {
	char* pathTotal = string_new();
	string_append(&pathTotal, configFileSystem->punto_montaje);
	string_append(&pathTotal, "Bloques/");

	char* bloqueDato = string_new();
	sprintf(bloqueDato, "%d.bin", numeroBloque);
	string_append(&pathTotal, bloqueDato);

	free(bloqueDato);
	return pathTotal;
}

void crearArchivo(char* path, int posBloqueLibre){
	FILE* archivo = fopen(path, "w+");
	log_info(logs, "Hice fopen del path: %s", path);
	fprintf(archivo, "TAMANIO=0\n");
	log_info(logs, "Hice fprintf de tamanio");

	char* bloque = string_new();
	sprintf(bloque, "%d", posBloqueLibre);

	char* lineaBloques = string_new();
	string_append(&lineaBloques, "BLOQUES=[");
	string_append(&lineaBloques, bloque);
	string_append(&lineaBloques, "]");

	fprintf(archivo, lineaBloques);
	log_info(logs, "Hice fprintf de la linea de bloques");

	free(bloque);
	free(lineaBloques);
	fclose(archivo);
}

void vaciarBloqueFisico(int bloque){
	char* pathBloqueFisico = armarPathBloqueDatos(bloque);
	//Como el archivo ya existe -> al abrirlo con "w" se sobreescribe
	FILE* bloqueFisico = fopen(pathBloqueFisico, "w");
	fclose(bloqueFisico);

	free(pathBloqueFisico);
}

void eliminarMetadataArchivo(char* path){
	remove(path);
}

void actualizarBytesEscritos (int* acum, int bytes){
	*acum += bytes;
}

int is_regular_file(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}
