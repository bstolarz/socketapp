#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>
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
#include "functions/config.h"


void crearArchivos()
{
	// Funcion que crea Metada y bitmap. Creemos que esto despues sera proveido por la catedra
	/*
	int tamanioBloques = 64;
	int cantBloques=5192;

	FILE *fp = fopen((char*) strcat(configFileSystem->punto_montaje, "/Metadata/Metadata.bin"), "ab+");

	fprintf(fp, "TAMANIO_BLOQUES=%i\n", tamanioBloques);
	fprintf(fp, "CANTIDAD_BLOQUES=%i\n", cantBloques);
	fprintf(fp, "MAGIC_NUMBER=SADICA\n");
	fclose(fp);
	*/

	/*** c r e o  b i t  a r r a y ***/

	int i;
	char * strPath = string_new();

	string_append(&strPath, configFileSystem->punto_montaje);
	string_append(&strPath, "Metadata/Bitmap.bin");


	int bitmapFD = open(strPath, O_RDWR, (mode_t)0600);

	log_info(logs, "Creo archivo bitarray %s", strPath);
	char * bitmap = mmap(0, configMetadata->cantidadBloques-1, PROT_WRITE, MAP_SHARED, bitmapFD, 0);

	if (bitmap == MAP_FAILED) {
		close(bitmapFD);
		perror("No se pudo mappear Sadica");
		log_info(logs, "%i", errno);
		exit(EXIT_FAILURE);
	}

	for(i=0;i<configMetadata->cantidadBloques;i++)
		bitmap[i]=0;

	log_info(logs, "Se mapeo bitarray a memoria");

	t_bitarray * bitarray = bitarray_create_with_mode(bitmap, configMetadata->cantidadBloques/8, MSB_FIRST);

	log_info(logs, "hizo el create with mode, tamanio: %i", bitarray_get_max_bit(bitarray));

	for(i=0;i<configMetadata->cantidadBloques;i++)
		if(i==40 || i==21 || i==82 || i==3)
			bitarray_set_bit(bitarray, i);
		else
			bitarray_clean_bit(bitarray, i);


	log_info(logs, "cant bloques recorridos: %i", i);
	close(bitmapFD);

	munmap(&bitarray, configMetadata->cantidadBloques-1);
	free(strPath);

	/*** c r e o  m e t a  d a t a  p o r  a r c h i v o ***/
	/*
	FILE *fp3 = fopen((char*) strcat(configFileSystem->punto_montaje, "/Archivos/passwords/alumnosSIGA.bin"), "ab+");
	fprintf(fp3, "TAMANIO=250\n");
	fprintf(fp3, "BLOQUES=[40,21,82,3]");

	fclose(fp3);
	*/

	/*** c r e o  b l o q u e s  d e  d a t o s ***/
	/*
	FILE * fpBloque;
	for(i=0;i<cantBloques;i++){
		fpBloque = fopen((char*) strcat(configFileSystem->punto_montaje, "/Bloques/%i.bin"), "ab+");
		fclose(fpBloque);
	}
	*/
}

void initSadica(){

	//1. File System (Tiene tamanio bloques, cant bloques, magic number)
	//2. bitmap (array con 0 y 1, inicializado con los bloques 40, 21, 82, 3)
	//3. Metadata (tamanio real del archivo en bytes, array con nro bloques en orden
		// TAMANIO=250  BLOQUES=[40,21,82,3]

	log_info(logs, "entro a initSadica");

	//crearArchivos(); /*Por ahora solo creo el Bitmap*/

	char* pathBitmap = string_new();
	string_append(&pathBitmap, configFileSystem->punto_montaje);
	string_append(&pathBitmap, "Metadata/Bitmap.bin");

	int bitmapArchive = open(pathBitmap, O_RDWR);

	//el file con metadata del archivo, se lee y se escribe con bloques si se agrega info al archivo
	//FILE *md = open(strcat(configFileSystem->punto_montaje, "/Archivos/passwords/alumnosSIGA.bin"), O_RDWR);

	//Mapeo bitmap.bin
	char* bitmapMapped = mmap(0, configMetadata->cantidadBloques-1, PROT_WRITE, MAP_SHARED, bitmapArchive, 0);

	//Crear bit array
	bitarray = bitarray_create_with_mode(bitmapMapped, configMetadata->cantidadBloques-1 /8, MSB_FIRST);

	//Liberar strings
}

void unmountSadica(){

	int i;
	char * path;
	remove((char*) strcat(configFileSystem->punto_montaje, "/Metadata/Metadata.bin"));
	remove((char*) strcat(configFileSystem->punto_montaje, "/Metadata/Bitmap.bin"));
	remove((char*) strcat(configFileSystem->punto_montaje, "/Archivos/passwords/alumnosSIGA.bin"));
	for(i=0;i<configMetadata->cantidadBloques;i++){
		path=strcat(configFileSystem->punto_montaje, "/Bloques/");
		path=strcat(path, (char*) i);
		path=strcat(path, ".bin");
		remove(path);
	}


}



