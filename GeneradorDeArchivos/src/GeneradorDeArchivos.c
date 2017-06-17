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
#include <commons/log.h>
#include "functions/principales.h"
#include "functions/config.h"

int main(void) {

	remove("logGeneradorArchivos");
	logs = log_create("logGeneradorArchivos", "GeneradorDeArchivos", 0, LOG_LEVEL_TRACE);

	configMetadata = malloc(sizeof(t_metadata));

	printf("Ingrese el path donde se encuentra el punto de montaje 'mnt/SADICA_FS/'\n");
	printf("Acordate que ahora estas parado en GeneradorArchivos/Debug:\n");
	size_t cantidad = 50;
	char* puntoMontaje = malloc(sizeof(char)*cantidad);
	size_t cantLeida = getline(&puntoMontaje, &cantidad, stdin);
	puntoMontaje[cantLeida-1]='\0';


	char* pathMetadata = concatenarSegundoParametroAlPrimero(puntoMontaje, "Metadata/Metadata.bin");
	char* pathBitmap = concatenarSegundoParametroAlPrimero(puntoMontaje, "Metadata/Bitmap.bin");
	char* pathCarpetaBloques = concatenarSegundoParametroAlPrimero(puntoMontaje, "Bloques/");

	metadataFS_read(pathMetadata);

	while(1){
		printf("[GENERADOR] - 	bitmap			Vacia el bitmap y lo inicializa todo en 0.\n");
		printf("[GENERADOR] - 	bloques			Borra los bloques (si hubiera) y los trunca en tamanioBloques del Metadata.bin\n");
		printf("[GENERADOR] - 	exit			Salir del programa.\n");
		printf("[GENERADOR] -	Ingrese un comando:\n");

		size_t cantidad = 50;
		char* comando = malloc(sizeof(char)*cantidad);
		size_t cantLeida = getline(&comando, &cantidad, stdin);
		comando[cantLeida-1]='\0';

		if(strcmp(comando, "bitmap") == 0){
			printf("Inicializando bitmap en 0...\n");
			inicializarBitmapEnCero(pathBitmap);
		}
		else if(strcmp(comando, "bloques") == 0){
			printf("Truncando bloques del bitmap...\n");
			truncarBloquesBitmap(pathCarpetaBloques);
		}
		else if(strcmp(comando, "exit") == 0){
			free(configMetadata);
			return EXIT_SUCCESS;
		}

		printf("Listo!\n");
	}

	free(configMetadata);
	return EXIT_SUCCESS;
}
