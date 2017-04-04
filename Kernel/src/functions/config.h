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
void generarListaDeSemaforos(t_config* config, t_console* console);
void generarVectorDeVariablesCompartidas(t_config* config,t_console* console);
void leerConfiguracionDeKernel(char* path);



#endif /* FUNCTIONS_CONFIG_H_ */
