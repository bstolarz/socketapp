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

void truncarBloquesBitmap(){

	int i;
	for (i=0; i<configMetadata->cantidadBloques; i++){
		char* comando = string_new();
		char* numero = string_new();
		sprintf(numero, "%d.bin", i);
		string_append(&comando, "truncate -s 64 ../../FileSystem/Debug/mnt/SADICA_FS/Bloques/");
		string_append(&comando, numero);

		system(comando);

		free(comando);
		free(numero);
	}
}


void inicializarBitmapEnCero(){
	int i;
	char * strPath = "../../FileSystem/Debug/mnt/SADICA_FS/Metadata/Bitmap.bin";

	int bitmapFD = open(strPath, O_RDWR, (mode_t)0600);

	log_info(logs, "Creo archivo bitarray %s", strPath);
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
