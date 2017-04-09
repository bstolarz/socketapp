/*
 * handShakeWithCPU.c
 *
 *  Created on: 9/4/2017
 *      Author: utnso
 */


#include <stdio.h>
#include <stdlib.h>
#include "../commons/declarations.h"
void agregarCPU(int socket){
	list_add(CPUs,(void*)socket);
};
