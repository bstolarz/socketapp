#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>
#include<sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <commons/bitarray.h>
#include "../commons/sadica.h"
#include "../commons/declarations.h"
//#include "functions/dump.h"

void initSadica (char* pathSadicaDrive){
	//Abro el archivo
		int arch;
		arch=open(pathSadicaDrive, O_RDWR, (mode_t)0600);
		if(arch==-1){
			perror("No se pudo abrir el drive Osada");
			exit(EXIT_FAILURE);
		}

	//Obtengo el  tamanio  del archivo
		struct stat sbuf;
		if (stat(pathSadicaDrive, &sbuf) == -1) {
			close(arch);
			perror("No se pudo obtener los atributos del drive Osada");
			exit(EXIT_FAILURE);
		}

	//Mappeo el archivo
		char* fileMapped = mmap(0, sbuf.st_size-1, PROT_WRITE, MAP_SHARED, arch, 0);
		if (fileMapped == MAP_FAILED) {
			close(arch);
			perror("No se pudo mappear el drive Osada");
			exit(EXIT_FAILURE);
		}
}
