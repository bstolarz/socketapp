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
	for (posLibre=0;posLibre<bitarray_get_max_bit(bitarray);posLibre++){
		if (!bitarray_test_bit(bitarray,posLibre)){
			return posLibre;
		}
	}

	//No hay bloques libres en el bitmap
	return -1;
}

bool hayNBloquesLibres(int cantBloquesNecesitados){

	//Devuelve la cantidad de Bloques Libres

	int cantBloquesLibres = 0, i=0;

	for(i=0;i<bitarray_get_max_bit(bitarray);i++)
	{
		if (!bitarray_test_bit(bitarray,i)){
			cantBloquesLibres++;
			if(cantBloquesLibres == cantBloquesNecesitados){
				return true;
			}
		}
	}

	log_info(logs, "hay %i bloques libres", cantBloquesLibres);

	return false;

}
void ocuparBloqueLibre(int posBloqueLibre){
	bitarray_set_bit(bitarray, posBloqueLibre);
	log_info(logs, "Se ocupo el bloque %d del bitmap", posBloqueLibre);
}

void liberarBloqueDelBitmap(int bloque){
	bitarray_clean_bit(bitarray, bloque);
	log_info(logs, "Se libera el bloque %d del bitmap", bloque);
}

int cantBloquesLibresBitmap(){
	int bloquesLibres = 0;
	//Recorro todo el bitmap para ir contando los bloques libres
	int i;
	for(i=0; i<configMetadata->cantidadBloques; i++){
		if(!bitarray_test_bit(bitarray,i)){
			bloquesLibres++;
		}
	}
	return bloquesLibres;
}
