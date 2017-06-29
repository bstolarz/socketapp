#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
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

int isDirectoryEmpty(char *dirname) {
  int n = 0;
  struct dirent *d;
  DIR *dir = opendir(dirname);
  if (dir == NULL) //Not a directory or doesn't exist
    return 1;
  while ((d = readdir(dir)) != NULL) {
    if(++n > 2)
      break;
  }
  closedir(dir);
  if (n <= 2) //Directory Empty
    return 1;
  else
    return 0;
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

void borrarDirectorioSiEstaVacio(char* pathConArchivo){
	log_info(logs, "Entre en la funcion borrarDirectorioSiEstaVacio");
	char** arrayPath = string_split(pathConArchivo, "/");
	char* directorioNoBorrable = string_from_format("%sArchivos/", configFileSystem->punto_montaje);
	int cantDirectoriosEnArrayPath;

	size_t i=0;

	while(arrayPath[i + 1] != NULL){
		cantDirectoriosEnArrayPath++;
		i++;
	}

	log_info(logs, "Cantidad de directorios en el array de %s es %d", pathConArchivo, cantDirectoriosEnArrayPath);
	int n, m;
	for(n=cantDirectoriosEnArrayPath;n>=0;n--){
		log_info(logs, "N vale: %d", n);
		char* pathDirectorioABorrar = string_new();
		for(m=0; m<n; m++){
			string_append(&pathDirectorioABorrar, arrayPath[m]);
			string_append(&pathDirectorioABorrar, "/");
			//log_info(logs, "Path armado: %s", pathDirectorioABorrar);
		}
		if(strcmp(directorioNoBorrable, pathDirectorioABorrar) != 0){
			//Estoy en directorios que puedo borrar
			if(isDirectoryEmpty(pathDirectorioABorrar) == 1){
				char* comando = string_from_format("rmdir %s", pathDirectorioABorrar);
				system(comando);
				free(comando);
				log_info(logs, "Borre el directorio %s", pathDirectorioABorrar);
			}
			else{
				//Tengo un directorio con contenido -> no puedo seguir borrando mis directorios padre
				log_info(logs, "Tengo un directorio con contenido, no sigo subiendo.");
				free(pathDirectorioABorrar);
				break;
			}
		}
		else{
			//Estoy en puntoMontaje/Archivos -> no puedo borrarlo
			log_info(logs, "Directorio a borrar es igual a %s, termino aca.", directorioNoBorrable);
			free(pathDirectorioABorrar);
			break;
		}


		free(pathDirectorioABorrar);
	}


	// liberar strings
	free(directorioNoBorrable);

	i = 0;
	while (arrayPath[i] != NULL)
	{
		free(arrayPath[i]); // el nombre del archivo
		++i;
	}

	free(arrayPath);
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


