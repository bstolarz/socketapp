/*
 * Structures.h
 *
 *  Created on: 1/4/2017
 *      Author: utnso
 */
#include <commons/collections/list.h>

#ifndef COMMONS_STRUCTURES_H_
#define COMMONS_STRUCTURES_H_

typedef struct{
	char* puerto;
	char* punto_montaje;
}t_fileSystem;

typedef struct{
	int tamanioBloques;
	int cantidadBloques;
} t_metadata;

typedef struct{
	int tamanio;
	t_list* bloques;
} t_metadata_archivo;

#endif /* COMMONS_STRUCTURES_H_ */
