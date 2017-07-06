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
int crear(char* path, char* pathConPuntoMontaje) {
	if(validar(pathConPuntoMontaje) == 1){
		log_info(logs, "Intentaste crear un archivo y este ya existe, devuelvo error");
		return -ENOENT;
	}
	else{
		int posBloqueLibre = encontrarUnBloqueLibre();
		log_info(logs, "PosBloqueLibre: %d", posBloqueLibre);
		if (posBloqueLibre >= 0) {
			ocuparBloqueLibre(posBloqueLibre);
			if(crearArchivo(pathConPuntoMontaje, posBloqueLibre) == 1){
				log_info(logs, "Se creo el archivo, se le asigno el bloque: %d", posBloqueLibre);
				return 1;
			}
			else{
				//No se pudo crear el archivo porque hay directorios entremedio del path que no existen => primero los creo
				if(seVanAPoderCrearLosDirectoriosNecesarios(path)){
					crearDirectoriosNecesarios(path);
					crearArchivo(pathConPuntoMontaje, posBloqueLibre);
					return 1;
				}
				crearArchivo(pathConPuntoMontaje, posBloqueLibre);
				return -ENOENT;
			}
		} else {
			log_info(logs, "No se encontro un bloque libre en el bitmap");
			return -ENOENT;
		}
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

		borrarDirectorioSiEstaVacio(path);
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
			close(fileDesBF);
			free(pathBloqueFisico);
		}

		list_destroy(archivo->bloques);
		free(archivo);
		return iSize;
	} else {
		log_info(logs, "No se encontro el archivo, por ende no se le puede obtener datos");
		return -ENOENT;
	}

}


int guardarDatos(char* path, off_t offset, size_t size, void* buffer) {

	if(offset+size >= configMetadata->cantidadBloques*configMetadata->tamanioBloques){
		printf("Lo que desea escribir excede el tamanio del disco. No se pueden guardar los datos. \n");
		return -ENOENT;
	}

	if(size == 0){
		printf("El tamanio a escribir no puede ser cero. \n");
		return -ENOENT;
	}


	if (validar(path) == 1) {

		t_metadata_archivo* archivo = malloc(sizeof(t_metadata_archivo));
		read_fileMetadata(path, archivo);

		//Validar antes del while si tengo que reservar bloques y si el bitmap los tiene, si no los tiene entonces no escribo nada

		log_info(logs, "elements count: %i \n", archivo->bloques->elements_count);
		log_info(logs, "size: %i \n", size);
		log_info(logs, "offset: %i \n", offset);

		double cantBloquesLibresNuevos;

		log_info(logs, "cantBloquesArchivo: %d", archivo->bloques->elements_count);

		//TODO: si es una cantidad que no entra en un bloque, me esta devolviendo un bloque menos.
		if(offset+size > archivo->bloques->elements_count*configMetadata->tamanioBloques){
			double cantBloquesLibresAux = (float)(offset+size-(archivo->bloques->elements_count*configMetadata->tamanioBloques))
																			/(float)configMetadata->tamanioBloques;
			cantBloquesLibresNuevos = ceil(cantBloquesLibresAux);


		}else{
			cantBloquesLibresNuevos = 0;
		}

		log_info(logs, "Necesito %f bloques libres", cantBloquesLibresNuevos);
		if(cantBloquesLibresNuevos > 0){
			if(!hayNBloquesLibres(cantBloquesLibresNuevos)){
				log_info(logs, "No hay espacio para guardar los cambios");
				return -ENOENT;
			}
		}
		else{
			//escribe sobre el espacio ya previamente reservado

		}
		log_info(logs, "SI hay espacio para guardar los cambios");


		int posBloqueArranque = (int) floor(offset / configMetadata->tamanioBloques);

		int byteComienzoEscritura = offset - posBloqueArranque * configMetadata->tamanioBloques;


		int bytesEscritos = 0;
		int sizeAux = size;
		int posPrimerBloqueLibre;
		int cantAsignaciones = 0;
		int i;
		int bytesAEscribirEnBloque = 0;


		log_info(logs, "Este archivo tiene asignados %i bloques", archivo->bloques->elements_count);


		for(i=0;i<(int)cantBloquesLibresNuevos;i++){
			posPrimerBloqueLibre = encontrarUnBloqueLibre();
			list_add(archivo->bloques, (int)posPrimerBloqueLibre);
			ocuparBloqueLibre(posPrimerBloqueLibre);
		}


		while (sizeAux > 0) {

			log_info(logs, "posbloqueArranque=%i", posBloqueArranque);
			int numeroDeBloqueFisico = (int)list_get(archivo->bloques, posBloqueArranque);

			char* pathBloqueFisico = armarPathBloqueDatos(numeroDeBloqueFisico);
			int bloqueFisico = open(pathBloqueFisico, O_RDWR, (mode_t)0600);

			if(bloqueFisico == -1){
				log_info(logs, "No se pudo abrir el archivo %s del bloque", pathBloqueFisico);
				return -1;
			}

			char* bloqueFisicoMapped = mmap(NULL, configMetadata->tamanioBloques, PROT_WRITE, MAP_SHARED, bloqueFisico, 0);

			if(bloqueFisicoMapped == MAP_FAILED){
				log_info(logs, "El mapeo con el archivo %s del bloque ha fallado porque %s", pathBloqueFisico, strerror(errno));
				return -ENOENT;
			}



			int espacioLibreEnBloqueActual = configMetadata->tamanioBloques - (offset - posBloqueArranque * configMetadata->tamanioBloques);
			if(sizeAux <= configMetadata->tamanioBloques){
				bytesAEscribirEnBloque = sizeAux;
				log_info(logs, "lo que resta por escribir entra en el bloque actual");
			}
			else{
				log_info(logs, "lo que resta por escribir excede al bloque actual");
				bytesAEscribirEnBloque = configMetadata->tamanioBloques;
			}

			memcpy(bloqueFisicoMapped+byteComienzoEscritura, buffer+((int)size-sizeAux), bytesAEscribirEnBloque);
			log_info(logs, "Size aux vale %d y byte comienzo vale %d", sizeAux, byteComienzoEscritura);
			actualizarBytesEscritos(&bytesEscritos, sizeAux);

			/*
			//Si lo que me queda por escribir (sizeAux) es mayor al espacio libre en el bloque
			if(sizeAux >= espacioLibreEnBloqueActual){
				log_info(logs, "lo que resta por escribir excede al bloque actual");
				memcpy(bloqueFisicoMapped+byteComienzoEscritura, buffer+((int)size-sizeAux), configMetadata->tamanioBloques - byteComienzoEscritura);
				log_info(logs, "Size aux vale %d y byte comienzo vale %d", sizeAux, byteComienzoEscritura);
				actualizarBytesEscritos(&bytesEscritos, configMetadata->tamanioBloques-byteComienzoEscritura);
			}else{
				log_info(logs, "lo que resta por escribir entra en el bloque actual");
				memcpy(bloqueFisicoMapped+byteComienzoEscritura, buffer+((int)size-sizeAux), sizeAux);
				log_info(logs, "Size aux vale %d y byte comienzo vale %d", sizeAux, byteComienzoEscritura);
				actualizarBytesEscritos(&bytesEscritos, sizeAux);

			}
			*/

			/*
			//Ocupo bloque si se pudo mapear archivo correctamente
			if(cantAsignaciones >0 || (offset>0 && (offset % configMetadata->tamanioBloques) == 0)){
				ocuparBloqueLibre(posPrimerBloqueLibre);
				list_add(archivo->bloques, posPrimerBloqueLibre);
			}
			*/
			sizeAux=sizeAux-(configMetadata->tamanioBloques-byteComienzoEscritura);

			log_info(logs, "Se han escrito %d bytes en el bloque %d",bytesEscritos, posBloqueArranque);
			byteComienzoEscritura=0;
			posBloqueArranque = posBloqueArranque + 1;
			cantAsignaciones++;


			munmap(bloqueFisicoMapped, configMetadata->tamanioBloques);
			log_info(logs, "hizo el munmap");
			close(bloqueFisico);
			free(pathBloqueFisico);
			log_info(logs, "cerro archivo fisico \n");
		}

		if((offset+size)>archivo->tamanio)
			archivo->tamanio=offset+size;

		metadataFS_write(path, archivo);

		list_destroy(archivo->bloques);
		free(archivo);
		return 1;
	}
	else{
		//No existe el archivo
		printf("No se encontro el archivo, por ende no se le puede guardar datos. \n");
		return -ENOENT;
	}


}
