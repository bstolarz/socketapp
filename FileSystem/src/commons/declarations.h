/*
 * declarations.h
 *
 *  Created on: 4/4/2017
 *      Author: utnso
 */
#include "structures.h"
#include <commons/bitarray.h>
#include <commons/log.h>

#ifndef COMMONS_DECLARATIONS_H_
#define COMMONS_DECLARATIONS_H_

t_fileSystem* configFileSystem;
t_metadata* configMetadata;
int serverSocket;
t_bitarray* bitarray;
t_log* logs;
int socketKernel;

#endif /* COMMONS_DECLARATIONS_H_ */
