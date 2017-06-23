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

int is_regular_file(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

char* armarPathMetadataFS(){
	char* pathTotal = string_new();
	string_append(&pathTotal, configFileSystem->punto_montaje);
	string_append(&pathTotal, "Metadata/Metadata.bin");

	return pathTotal;
}

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

int crearArchivo(char* path, int posBloqueLibre){
	FILE* archivo = fopen(path, "w+");
	if(archivo != NULL){
		log_info(logs, "Hice fopen del path: %s", path);
		fprintf(archivo, "TAMANIO=0\n");
		log_info(logs, "Hice fprintf de tamanio");

		fprintf(archivo, "BLOQUES=[%d]", posBloqueLibre);
		log_info(logs, "Hice fprintf de la linea de bloques");

		fclose(archivo);
		return 1;
	}
	else{
		//El path tiene directorios entremedio que no existen
		return -ENOENT;
	}

}

bool seVanAPoderCrearLosDirectoriosNecesarios(char* pathDelKernel){
	char* directorio = string_duplicate(configFileSystem->punto_montaje);
	string_append(&directorio, "Archivos/");
	char** arrayDeDirectoriosYArchivoFinal = string_split(pathDelKernel, "/");
	size_t i = 0;

	while(arrayDeDirectoriosYArchivoFinal[i + 1] != NULL){
		string_append(&directorio, arrayDeDirectoriosYArchivoFinal[i]);

		if(access(directorio, F_OK) == 0 && is_regular_file(directorio)){
			return false;
		}

		string_append(&directorio, "/");
		i++;
	}

	// liberar strings
	free(directorio);

	i = 0;
	while (arrayDeDirectoriosYArchivoFinal[i] != NULL)
	{
		free(arrayDeDirectoriosYArchivoFinal[i]); // el nombre del archivo
		++i;
	}

	free(arrayDeDirectoriosYArchivoFinal);

	return true;
}

int crearDirectoriosNecesarios(char* pathDelKernel){
	char* directorio = string_duplicate(configFileSystem->punto_montaje);
	string_append(&directorio, "Archivos/");
	char** arrayDeDirectoriosYArchivoFinal = string_split(pathDelKernel, "/");
	size_t i = 0;

	while(arrayDeDirectoriosYArchivoFinal[i + 1] != NULL){
		string_append(&directorio, arrayDeDirectoriosYArchivoFinal[i]);

		char* comando = string_from_format("mkdir %s", directorio);
		system(comando);
		free(comando);

		string_append(&directorio, "/");
		i++;
	}

	// liberar strings
	free(directorio);

	i = 0;
	while (arrayDeDirectoriosYArchivoFinal[i] != NULL)
	{
		free(arrayDeDirectoriosYArchivoFinal[i]); // el nombre del archivo
		++i;
	}
	free(arrayDeDirectoriosYArchivoFinal);

	return 1;
}

int avanzarBloque(t_metadata_archivo* archivo, int desplazamientoHastaElBloque){
	if(list_size(archivo->bloques) > desplazamientoHastaElBloque){
		return list_get(archivo->bloques, desplazamientoHastaElBloque);
	}
	else{
		return -1;
	}
}

void eliminarMetadataArchivo(char* path){
	remove(path);
}

void actualizarBytesEscritos (int* acum, int bytes){
	*acum += bytes;
}


