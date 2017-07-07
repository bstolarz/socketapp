/*
 * config.h
 *
 *  Created on: 3/4/2017
 *      Author: utnso
 */

#ifndef FUNCTIONS_CONFIG_H_
#define FUNCTIONS_CONFIG_H_
#include "../commons/structures.h"
#include <commons/config.h>

void config_read_semaforos(t_config* config);
void config_read_shared_vars(t_config* config);
void config_read(char* path);
void* config_observe_changes(void* configPath);


#endif /* FUNCTIONS_CONFIG_H_ */
