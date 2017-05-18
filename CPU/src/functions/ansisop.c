#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <parser/parser.h>
#include "primitivas.h"
#include "serialization.h"
#include "memory.h"
#include "../commons/structures.h"
#include "../commons/declarations.h"

void ansisop_init(){
	ansisop_funciones = malloc(sizeof(AnSISOP_funciones));
	ansisop_funciones->AnSISOP_asignar=AnSISOP_asignar;
	ansisop_funciones->AnSISOP_definirVariable=AnSISOP_definirVariable;
	ansisop_funciones->AnSISOP_dereferenciar=AnSISOP_dereferenciar;
	ansisop_funciones->AnSISOP_obtenerPosicionVariable=AnSISOP_obtenerPosicionVariable;
	ansisop_funciones->AnSISOP_finalizar=AnSISOP_finalizar;
	ansisop_funciones->AnSISOP_irAlLabel=AnSISOP_irAlLabel;
	ansisop_funciones->AnSISOP_llamarConRetorno=AnSISOP_llamarConRetorno;
	ansisop_funciones->AnSISOP_retornar=AnSISOP_retornar;
	ansisop_funciones->AnSISOP_llamarSinRetorno=AnSISOP_llamarSinRetorno;

	ansisop_funciones_kernel = (AnSISOP_kernel*) malloc(sizeof(AnSISOP_kernel));
	ansisop_funciones_kernel->AnSISOP_wait=AnSISOP_wait;
	ansisop_funciones_kernel->AnSISOP_signal=AnSISOP_signal;
}
