#include <sys/statvfs.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <commons/collections/list.h>
#include <commons/bitarray.h>
#include "../commons/declarations.h"
#include <commons/string.h>
#include <sys/stat.h>
#include <math.h>
#include <commons/log.h>

int encontrarUnBloqueLibre(){
	int posLibre;
	for (posLibre=0;posLibre<=bitarray_get_max_bit(bitarray);posLibre++){
		if (!bitarray_test_bit(bitarray,posLibre)){
			return posLibre;
		}
	}

	//No hay bloques libres en el bitmap
	return -1;
}

void ocuparBloqueLibre(int posBloqueLibre){
	bitarray_set_bit(bitarray, posBloqueLibre);
}
