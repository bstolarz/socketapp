#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>
#include <commons/config.h>
#include <commons/string.h>
#include "commons/structures.h"
#include "commons/declarations.h"
#include <commons/bitarray.h>
#include "functions/operaciones.h"
#include "libSockets/client.h"
#include "libSockets/server.h"
#include "libSockets/send.h"
#include "libSockets/recv.h"

void config_print1(){
	printf("El puerto del FS: %s\n", configFileSystem->puerto);
	printf("Punto de montaje: %s\n", configFileSystem->punto_montaje);
}

void config_read1(char* path){
	t_config* config = config_create(path);

	configMetadata->tamanioBloques=atoi(string_duplicate(config_get_string_value(config,"TAMANIO_BLOQUES")));
	configMetadata->cantidadBloques=atoi(string_duplicate(config_get_string_value(config,"CANTIDAD_BLOQUES")));

	config_destroy(config);
}

void config_free1(){
	free(configFileSystem->puerto);
	free(configFileSystem->punto_montaje);
	free(configFileSystem);
}

void initSadica(){

	//1. File System (Tiene tamanio bloques, cant bloques, magic number)
	//2. bitmap (array con 0 y 1, inicializado con los bloques 40, 21, 82, 3)
	//3. Metadata (tamanio real del archivo en bytes, array con nro bloques en orden
		// TAMANIO=250  BLOQUES=[40,21,82,3]


	/***** Abro archivos ******/
	config_read1(strcat(configFileSystem->punto_montaje, "/Metadata/Metadata.bin"));

	int bitmapArchive = open(strcat(configFileSystem->punto_montaje, "/Metadata/Bitmap.bin"), O_RDWR);

	//el file con metadata del archivo, se lee y se escribe con bloques si se agrega info al archivo
	//FILE *md = open(strcat(configFileSystem->punto_montaje, "/Archivos/passwords/alumnosSIGA.bin"), O_RDWR);

	//Mapeo bitmap.bin
	char* bitmapMapped = mmap(0, configMetadata->cantidadBloques-1, PROT_WRITE, MAP_SHARED, bitmapArchive, 0);

	//Crear bit array
	bitarray = bitarray_create_with_mode(bitmapMapped, configMetadata->cantidadBloques-1 /8, MSB_FIRST);

}



