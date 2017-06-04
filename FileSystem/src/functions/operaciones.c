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

//Lista
int obtenerDatos(char* path, off_t offset, size_t size, char** buf) {
	if (validar(path) == 1) {
		//Leo el archivo y me lo cargo en la estructura
		t_metadata_archivo* archivo = malloc(sizeof(t_metadata_archivo));
		read_fileMetadata(path, archivo);

		int desplazamientoHastaElBloque=floor(offset/configMetadata->tamanioBloques);
		int bloqueArranque=avanzarBloque(archivo, desplazamientoHastaElBloque);
		int byteComienzoLectura = offset-(desplazamientoHastaElBloque*configMetadata->tamanioBloques);

		int desplazamiento = 0;
		int iSize = size;
		int fileSize = archivo->tamanio;

		//Reallocs
		if(offset == 0 && iSize<=fileSize){
			*buf=realloc(*buf, iSize);
			log_info(logs, "HAGO REALLOC de %d bytes", iSize);
		}
		else if(offset == 0 && iSize>fileSize){
			*buf=realloc(*buf, fileSize);
			log_info(logs, "HAGO REALLOC de %d bytes", fileSize);
			iSize=fileSize;
		}
		else if(offset>0 && offset<=fileSize && iSize+offset == fileSize){
			*buf=realloc(*buf, iSize);
			log_info(logs, "HAGO REALLOC de %d bytes", iSize);
		}
		else if(offset>0 && offset<=fileSize && iSize+offset > fileSize){
			*buf=realloc(*buf, fileSize-offset);
			log_info(logs, "HAGO REALLOC de %d bytes", fileSize-offset);
			iSize = fileSize-offset;
		}
		else if(offset>0 && offset<=fileSize && iSize+offset<fileSize){
			*buf=realloc(*buf, iSize);
			log_info(logs, "HAGO REALLOC de %d bytes", iSize);
		}
		else{
			log_info(logs, "Me diste un offset que se pasa del tamanioArchivo, aborto realloc y lectura");
			return -1;
		}


		while (bloqueArranque != -1 && (iSize-desplazamiento)>0 && (fileSize-desplazamiento-offset)>0){
			log_info(logs, "DesplazamientoHastaElBloque: %d", desplazamientoHastaElBloque);
			log_info(logs, "Bloque de arranque: %d", bloqueArranque);
			log_info(logs, "ByteComienzoLectura: %d", byteComienzoLectura);
			char* pathBloqueFisico = armarPathBloqueDatos(bloqueArranque);
			int fileDesBF = open(pathBloqueFisico, O_RDWR);
			void* bloqueArranqueFisico = mmap(0, configMetadata->tamanioBloques, PROT_READ, MAP_SHARED, fileDesBF, 0);


			if((fileSize-desplazamiento-offset)>=(configMetadata->tamanioBloques-byteComienzoLectura)){
				if((iSize-desplazamiento)>=(configMetadata->tamanioBloques-byteComienzoLectura)){
					//*buf = realloc(*buf, desplazamiento+configMetadata->tamanioBloques-byteComienzoLectura);
					memcpy(*buf+desplazamiento,bloqueArranqueFisico+byteComienzoLectura,configMetadata->tamanioBloques-byteComienzoLectura);
					log_info(logs, "Hice memcpy con size: %d", configMetadata->tamanioBloques-byteComienzoLectura);
					desplazamiento += configMetadata->tamanioBloques-byteComienzoLectura;
				}else{
					//*buf = realloc(*buf, iSize);
					memcpy(*buf+desplazamiento, bloqueArranqueFisico+byteComienzoLectura,iSize-desplazamiento);
					log_info(logs, "Hice memcpy con size: %d", iSize-desplazamiento);
					desplazamiento += iSize-desplazamiento;
				}
			}else{
				//*buf = realloc(*buf, archivo->tamanio);
				//log_info(logs, "Hago realloc de %d bytes", archivo->tamanio);
				memcpy(*buf+desplazamiento,bloqueArranqueFisico+byteComienzoLectura,iSize-desplazamiento);
				log_info(logs, "Hice memcpy con size: %d", iSize-desplazamiento);
				desplazamiento += iSize-desplazamiento-offset;
			}

			desplazamientoHastaElBloque++;
			bloqueArranque = avanzarBloque(archivo, desplazamientoHastaElBloque);

			byteComienzoLectura=0;
			munmap(bloqueArranqueFisico, configMetadata->tamanioBloques);
		}

		return iSize;
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
