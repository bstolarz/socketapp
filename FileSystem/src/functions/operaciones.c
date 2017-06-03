#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include "bitmap.h"
#include "config.h"
#include "../commons/structures.h"
#include "../commons/declarations.h"
#include "auxiliares.h"

//Lista
int validar(char* path) {
	int resultado = access(path, F_OK);
	if (resultado == 0 && is_regular_file(path)) {
		log_info(logs, "Se encontro el archivo para el path: %s", path);
		return 1;
	}else{
		log_info(logs, "No se encontro el archivo para el path: %s", path);
		return -ENOENT;
	}
}

//Lista
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

//Lista
int borrar(char* path) {
	if (validar(path) == 1) {
		t_metadata_archivo* archivo = malloc(sizeof(t_metadata_archivo));
		read_fileMetadata(path, archivo);

		list_iterate(archivo->bloques, (void*) liberarBloqueDelBitmap);
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
int obtenerDatos(char* path, off_t offset, size_t size, char** buf) {
	if (validar(path) == 1) {
		//Leo el archivo y me lo cargo en la estructura
		t_metadata_archivo* archivo = malloc(sizeof(t_metadata_archivo));
		read_fileMetadata(path, archivo);

		int desplazamientoHastaElBloque=floor(offset/configMetadata->tamanioBloques);
		log_info(logs, "DesplazamientoHastaElBloque: %d", desplazamientoHastaElBloque);
		int bloqueArranque=avanzarBloque(archivo, desplazamientoHastaElBloque);
		log_info(logs, "Bloque de arranque: %d", bloqueArranque);
		int byteComienzoLectura = offset-(desplazamientoHastaElBloque*configMetadata->tamanioBloques);
		log_info(logs, "ByteComienzoLectura: %d", byteComienzoLectura);
		int desplazamiento = 0;
		int iSize = size;
		int fileSize = archivo->tamanio;

		while (bloqueArranque != -1 && (iSize-desplazamiento)>0 && (fileSize-desplazamiento-offset)>0){
			char* pathBloqueFisico = armarPathBloqueDatos(bloqueArranque);
			int fileDesBF = open(pathBloqueFisico, O_RDWR);
			void* bloqueArranqueFisico = mmap(0, configMetadata->tamanioBloques, PROT_READ, MAP_SHARED, fileDesBF, 0);

			if((fileSize-desplazamiento-offset)>=(configMetadata->tamanioBloques-byteComienzoLectura)){
				if((iSize-desplazamiento)>=(configMetadata->tamanioBloques-byteComienzoLectura)){
					*buf = realloc(*buf, desplazamiento+configMetadata->tamanioBloques-byteComienzoLectura);
					memcpy(*buf+desplazamiento,bloqueArranqueFisico+byteComienzoLectura,configMetadata->tamanioBloques-byteComienzoLectura);
					desplazamiento += configMetadata->tamanioBloques-byteComienzoLectura;
				}else{
					*buf = realloc(*buf, iSize);
					memcpy(*buf+desplazamiento, bloqueArranqueFisico+byteComienzoLectura,iSize-desplazamiento);
					desplazamiento += iSize-desplazamiento;
				}
			}else{
				*buf = realloc(*buf, iSize-offset);
				memcpy(*buf+desplazamiento,bloqueArranqueFisico+byteComienzoLectura,iSize-desplazamiento-offset);
				desplazamiento += iSize-desplazamiento-offset;
			}

			desplazamientoHastaElBloque++;
			bloqueArranque = avanzarBloque(archivo, desplazamientoHastaElBloque);
			log_info(logs, "Proximo bloque arranque: %d", bloqueArranque);

			byteComienzoLectura=0;
			log_info(logs, "Proximo byteComienzoLectura: %d", byteComienzoLectura);

			munmap(bloqueArranqueFisico, configMetadata->tamanioBloques);
		}


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
