/*
 * config.h
 *
 *  Created on: 4/4/2017
 *      Author: utnso
 */
#include "../commons/structures.h"

#ifndef FUNCTIONS_CONFIG_H_
#define FUNCTIONS_CONFIG_H_

void config_print();
void config_read(char* path);
void config_free();
void read_fileMetadata(char* path, t_metadata_archivo* archivo);
void metadataFS_print();
void metadataFS_read(char* path);
void metadataFS_free();
void metadataFS_write();

#endif /* FUNCTIONS_CONFIG_H_ */
