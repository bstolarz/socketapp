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


void crearArchivos()
{
	// Funcion que crea Metada y bitmap. Creemos que esto despues sera proveido por la catedra

	int cantBloques = 64;

	FILE *fp = fopen((char*) strcat(configFileSystem->punto_montaje, "/Metadata/Metadata.bin"), "ab+");

	fprintf(fp, "TAMANIO_BLOQUES=%i\n", cantBloques);
	fprintf(fp, "CANTIDAD_BLOQUES=5192\n");
	fprintf(fp, "MAGIC_NUMBER=SADICA\n");
	fclose(fp);

	/*** b i t  a r r a y ***/

	char * bitArray = (char *) malloc(cantBloques);
	int i;

	FILE * fp2 = fopen((char*) strcat(configFileSystem->punto_montaje, "/Metadata/Metadata.bin"), "ab+");
	for(i=0;i<cantBloques;i++)
		if(i==40 || i==21 || i==82 || i==3)
			bitArray[i] = 1;
		else
			bitArray[i] = 1;
	size_t result;
	fwrite(bitArray, cantBloques * sizeof(char *), result, fp2);

	fclose(fp2);

	/*** m e t a  d a t a  p o r  a r c h i v o ***/

	FILE *fp3 = fopen((char*) strcat(configFileSystem->punto_montaje, "/Archivos/passwords/alumnosSIGA.bin"), "ab+");
	fprintf(fp3, "TAMANIO=250\n");
	fprintf(fp3, "BLOQUES=[40,21,82,3]");

	fclose(fp3);

	/*** b l o q u e s  d e  d a t o s ***/
	FILE * fpBloque = fopen((char*) strcat(configFileSystem->punto_montaje, "/Archivos/passwords/40.bin"), "ab+");
	fprintf(fpBloque, "Este es el bloque 40");
	fclose(fpBloque);

	fpBloque = fopen((char*) strcat(configFileSystem->punto_montaje, "/Archivos/passwords/21.bin"), "ab+");
	fprintf(fpBloque, "Este es el bloque 21");
	fclose(fpBloque);

	fpBloque = fopen((char*) strcat(configFileSystem->punto_montaje, "/Archivos/passwords/82.bin"), "ab+");
	fprintf(fpBloque, "Este es el bloque 82");
	fclose(fpBloque);

	fpBloque = fopen((char*) strcat(configFileSystem->punto_montaje, "/Archivos/passwords/3.bin"), "ab+");
	fprintf(fpBloque, "Este es el bloque 3");
	fclose(fpBloque);

}

void initSadica(){

	//1. File System (Tiene tamanio bloques, cant bloques, magic number)
	//2. bitmap (array con 0 y 1, inicializado con los bloques 40, 21, 82, 3)
	//3. Metadata (tamanio real del archivo en bytes, array con nro bloques en orden
		// TAMANIO=250  BLOQUES=[40,21,82,3]

	crearArchivos();
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



