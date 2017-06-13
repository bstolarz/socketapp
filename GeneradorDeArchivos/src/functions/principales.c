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
#include "../commons/structures.h"
#include "../commons/declarations.h"
#include <commons/bitarray.h>

bool ends_with_char(char* unString, char unChar){
	return unString[string_length(unString)-1] == unChar;
}

char* concatenarSegundoParametroAlPrimero(char* puntoMontaje, char* otroString){
	char* pathMetadata = string_new();
	string_append(&pathMetadata, puntoMontaje);

	if(!ends_with_char(pathMetadata, '/')){
		string_append(&pathMetadata, "/");
	}

	string_append(&pathMetadata, otroString);
	return pathMetadata;
}

void truncarBloquesBitmap(char* pathCarpetaBloques){
	char* tamanioBloque = string_new();
	sprintf(tamanioBloque, "%d ", configMetadata->tamanioBloques);

	int i;
	for (i=0; i<configMetadata->cantidadBloques; i++){
		char* comando = string_new();
		char* numeroBloque = string_new();
		sprintf(numeroBloque, "%d.bin", i);

		string_append(&comando, "truncate -s ");
		string_append(&comando, tamanioBloque);
		string_append(&comando, pathCarpetaBloques);
		string_append(&comando, numeroBloque);

		if(access(comando, F_OK)){
			remove(comando);
		}

		system(comando);

		free(comando);
		free(numeroBloque);
	}
	free(tamanioBloque);
}


void inicializarBitmapEnCero(char* pathBitmap){
	int i;

	int bitmapFD = open(pathBitmap, O_RDWR, (mode_t)0600);

	log_info(logs, "Creo archivo bitarray %s", pathBitmap);
	char * bitmap = mmap(0, configMetadata->cantidadBloques-1, PROT_WRITE, MAP_SHARED, bitmapFD, 0);

	if (bitmap == MAP_FAILED) {
		close(bitmapFD);
		perror("MAP_FAILED");
		log_info(logs, "%i", errno);
		exit(EXIT_FAILURE);
	}

	for(i=0;i<configMetadata->cantidadBloques;i++)
		bitmap[i]=0;

	t_bitarray * bitarray = bitarray_create_with_mode(bitmap, configMetadata->cantidadBloques/8, MSB_FIRST);

	close(bitmapFD);

	munmap(&bitarray, configMetadata->cantidadBloques-1);
}
