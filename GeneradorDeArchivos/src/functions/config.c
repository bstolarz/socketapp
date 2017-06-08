#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include "../commons/declarations.h"



void metadataFS_read(char* path){
	t_config* config = config_create(path);

	configMetadata->tamanioBloques=config_get_int_value(config,"TAMANIO_BLOQUES");
	configMetadata->cantidadBloques=config_get_int_value(config,"CANTIDAD_BLOQUES");

	config_destroy(config);
}
