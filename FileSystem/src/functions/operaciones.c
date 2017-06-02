#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include "bitmap.h"
#include "config.h"
#include "../commons/structures.h"
#include "../commons/declarations.h"
#include "auxiliares.h"

//Falta validar que sea solo archivo posta, NO directorio. Si es directorio entonces tiene que devolver que no existe
int validar(char* path) {
	log_info(logs, "Antes del access");
	int resultado = access(path, F_OK);
	log_info(logs, "Despues del access");
	if (resultado == 0 && is_regular_file(path)) {
		log_info(logs, "Se encontro el archivo para el path: %s", path);
		return 1;
	}else{
		log_info(logs, "No se encontro el archivo para el path: %s", path);
		return -ENOENT;
	}
}

int crear(char* path) {
	int posBloqueLibre = encontrarUnBloqueLibre();
	log_info(logs, "PosBloqueLibre: %d", posBloqueLibre);
	if (posBloqueLibre >= 0) {
		ocuparBloqueLibre(posBloqueLibre);
		crearArchivo(path, posBloqueLibre);
		log_info(logs, "Se creo el archivo, se le asigno el bloque: %d", posBloqueLibre);
		return 1;
	} else {
		log_info(logs, "No se encontro un bloque libre en el bitmap");
		return -ENOENT;
	}

}

//Falta vaciar los bloques fisicos
int borrar(char* path) {
	//Valido que exista el archivo
	if (validar(path) == 1) {
		t_metadata_archivo* archivo = malloc(sizeof(t_metadata_archivo));
		read_fileMetadata(path, archivo);

		list_iterate(archivo->bloques, (void*) liberarBloqueDelBitmap);
		//Falta vaciar los bloques fisicos
		eliminarMetadataArchivo(path);

		list_destroy(archivo->bloques);
		free(archivo);
		return 1;
	} else {
		log_info(logs, "No se encontro el archivo, por ende no se lo puede borrar");
		return -ENOENT;
	}

}

//Falta
int obtenerDatos(char* path, off_t offset, size_t size) {
	if (validar(path) == 1) {
		//hago las cosas
		return 1;
	} else {
		log_info(logs, "No se encontro el archivo, por ende no se le puede obtener datos");
		return -ENOENT;
	}

}

//Falta
int guardarDatos(char* path, off_t offset, size_t size, void* buffer) {
	if (validar(path) == 1) {
		t_metadata_archivo* archivo = malloc(sizeof(t_metadata_archivo));
		read_fileMetadata(path, archivo);

		int posBloqueArranque = ceil(offset / configMetadata->tamanioBloques);
		int bytesAEscribirEnElBloque = (posBloqueArranque * configMetadata->tamanioBloques) - offset;
		int byteComienzoEscritura = configMetadata->tamanioBloques - bytesAEscribirEnElBloque;
		int bytesEscritos = 0;
		int sizeAux = size;

		//Validar antes del while si tengo que reservar bloques y si el bitmap los tiene, si no los tiene entonces no escribo nada
		while (sizeAux > 0) {
			int numeroDeBloqueFisico = (int)list_get(archivo->bloques, posBloqueArranque - 1);

			char* pathBloqueFisico = armarPathBloqueDatos(numeroDeBloqueFisico);
			FILE* bloqueFisico = fopen(pathBloqueFisico, "w");

			//Si lo que me queda por escribir (sizeAux) es mayor a lo que tengo que escribir en el bloque
			if(sizeAux >= (configMetadata->tamanioBloques - byteComienzoEscritura)){
				memcpy(bloqueFisico+byteComienzoEscritura,buffer+((int)size-sizeAux),configMetadata->tamanioBloques - byteComienzoEscritura);
			}else{
				memcpy(bloqueFisico+byteComienzoEscritura,buffer+((int)size-sizeAux),sizeAux);
			}

			if(sizeAux >= (configMetadata->tamanioBloques - byteComienzoEscritura)){
				log_info(logs,"Size aux vale %d y byte comienzo vale %d",sizeAux,byteComienzoEscritura);
				actualizarBytesEscritos(&bytesEscritos,configMetadata->tamanioBloques-byteComienzoEscritura);
			}else{
				log_info(logs,"Size aux vale %d y byte comienzo vale %d",sizeAux,byteComienzoEscritura);
				actualizarBytesEscritos(&bytesEscritos,sizeAux);
			}
			sizeAux=sizeAux-(configMetadata->tamanioBloques-byteComienzoEscritura);

			log_info(logs, "Se han escrito %d bytes\n",bytesEscritos);
			byteComienzoEscritura=0;
			posBloqueArranque+=1;

			fclose(bloqueFisico);
		}

		list_destroy(archivo->bloques);
		free(archivo);
		return 1;
	} else {
		log_info(logs, "No se encontro el archivo, por ende no se le puede guardar datos");
		return -ENOENT;
	}


}
