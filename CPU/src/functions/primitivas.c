/*
 * primitivas.c
 *
 *  Created on: 20/4/2017
 *      Author: utnso
 */
//	#include "sintax.h"
#include "primitivas.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <parser/metadata_program.h>
#include <parser/parser.h>
#include <parser/metadata_program.h>
#include "../commons/structures.h"
#include "../commons/declarations.h"
#include <commons/collections/dictionary.h>
#include <commons/string.h>
#include <commons/string.h>
#include <commons/error.h>
#include "memory_requests.h"
#include "stack.h"

int VAR_SIZE = sizeof(t_valor_variable);

bool is_argument(t_nombre_variable identificador_variable)
{
	return isdigit(identificador_variable);
}

t_puntero AnSISOP_definirVariable (t_nombre_variable identificador_variable) {

	// Guardo el offset dond está esta variable/arg
	int varStackPosition = pcb->stackPosition;

	if (is_argument(identificador_variable))
	{
		stack_add_arg(identificador_variable);
	}
	else
	{
		stack_add_var(identificador_variable);
	}

	// Incremento la pila
	// TODO: check stack overflow?
	pcb->stackPosition += VAR_SIZE;

 	return varStackPosition; // retorno donde empezaba la var que puse en stack
}

t_puntero AnSISOP_obtenerPosicionVariable(t_nombre_variable identificador_variable)
{
	log_debug(logCPU, "[obtenerPosicionVariable] identificador = %c\n", identificador_variable);

	t_position* varPos;

	if (is_argument(identificador_variable))
	{
		varPos = stack_get_arg(identificador_variable);
	}
	else
	{
		varPos = stack_get_var(identificador_variable);
	}

	if (varPos == NULL)
		log_error(logCPU, "[obtenerPosicionVariable] no encontre %c\n", identificador_variable);

	return position_to_puntero(varPos);
}

t_valor_variable AnSISOP_dereferenciar(t_puntero direccion_variable){
	t_position pos = puntero_to_position(direccion_variable);
	log_debug(logCPU, "[dereferenciar] t_puntero = %d ---> page = %d, offset = %d\n", direccion_variable, pos.page, pos.off);

	void* readResult = memory_request_read(serverMemory, pcb->pid, pcb->cantPagsCodigo + pos.page, pos.off, VAR_SIZE);

	if (readResult == NULL)	log_error(logCPU, "[dereferenciar] no leyo bien de memoria");

	return *((int*)readResult);
}

void AnSISOP_asignar (t_puntero direccion_variable, t_valor_variable valor){
	t_position pos = puntero_to_position(direccion_variable);
	log_info(logCPU, "[asignar] page = %d, offset = %d, valor = %d\n", pos.page, pos.off, valor);

	int writeResult = memory_request_write(serverMemory, pcb->pid, pcb->cantPagsCodigo + pos.page, pos.off, VAR_SIZE, &valor);

	if (writeResult == -1)	log_error(logCPU, "[asignar] error al escribir en memoria");
}

// alcanza con devolver lo que te manda el kernel
/*t_valor_variable AnSISOP_obtenerValorCompartida(t_nombre_compartida variable){
	int* value=(int*)malloc(sizeof(int));
	if (socket_send_string(serverKernel, "ValueOfSharedVariable")>0){
		if (socket_send_string(serverKernel,variable)>0){

			socket_recv_int(serverKernel,value);
		}
	}
	return value;
}*/

// Cambia la linea de ejecucion a la correspondiente de la etiqueta buscada
void AnSISOP_irAlLabel (t_nombre_etiqueta etiqueta){
	pcb->pc = metadata_buscar_etiqueta(etiqueta, pcb->indiceDeEtiquetas, pcb->indiceDeEtiquetasCant);
}

// Preserva el contexto de ejecución actual para poder retornar luego al mismo.
// Modifica las estructuras correspondientes para mostrar un nuevo contexto vacío.
// Los parámetros serán definidos luego de esta instrucción de la misma manera que una variable local,
// con identificadores numéricos empezando por el 0.
void AnSISOP_llamarSinRetorno(t_nombre_etiqueta etiqueta){
	log_info(logCPU, "[llamarSinRetorno] Agrego nuevo contexto al indice de stack.\n");

	t_indiceDelStack* ind = stack_context_create();
	ind->retPos=pcb->pc;
	list_add(pcb->indiceDeStack, ind);

	// irallabel?
	// mandarlo a ejecutar la funcion
	pcb->pc = metadata_buscar_etiqueta(etiqueta, pcb->indiceDeEtiquetas, pcb->indiceDeEtiquetasCant);
}

//falta completar, pregunta en el foro hecha
//Preserva el contexto de ejecución actual para poder retornar luego al mismo, junto con la posicion de la variable entregada por donde_retornar.
//	 * Modifica las estructuras correspondientes para mostrar un nuevo contexto vacío.
void AnSISOP_llamarConRetorno(t_nombre_etiqueta etiqueta, t_puntero donde_retornar){

	// llamar sin retorno?
	log_info(logCPU, "[llamarConRetorno] Agrego nuevo contexto al indice de stack.\n");

	t_indiceDelStack* ind = stack_context_create();

	ind->retPos=pcb->pc; // guardar instruccion de retorno
	ind->retVar = puntero_to_position(donde_retornar); // guardar variable donde poner retorno

	list_add(pcb->indiceDeStack, ind);

	// irallabel?
	// mandarlo a ejecutar la funcion
	pcb->pc = metadata_buscar_etiqueta(etiqueta, pcb->indiceDeEtiquetas, pcb->indiceDeEtiquetasCant);
}

//Cambia el Contexto de Ejecución Actual para volver al Contexto anterior al que se está ejecutando,
// recuperando el Cursor de Contexto Actual y el Program Counter previamente apilados en el Stack.
// En caso de estar finalizando el Contexto principal (el ubicado al inicio del Stack),
// deberá finalizar la ejecución del programa.
void AnSISOP_finalizar (void)
{
	//vuelvo el PC a la posicion de retorno de la primitiva
	t_indiceDelStack* currentStackContext = stack_context_current();

	pcb->pc = currentStackContext->retPos;

	// saco del stack el contexto actual
	stack_pop();

	bool programCompleted = list_is_empty(pcb->indiceDeStack);

	if (programCompleted) // termino el main
	{
		printf("avisar a kernel el fin o marcar un bool, etc");
		pcb->exitCode = 0;
		// on_program_completed();
	}
}



//Dummies, algunas voy a ver de llenarlas estos dias
t_valor_variable AnSISOP_obtenerValorCompartida(t_nombre_compartida variable){
	printf("Hola, soy AnSISOP_obtenerValorCompartida\n");
	return NULL;
}

t_valor_variable AnSISOP_asignarValorCompartida(t_nombre_compartida variable, t_valor_variable valor){
	printf("Hola, soy AnSISOP_asignarValorCompartida\n");
		return NULL;
}


void AnSISOP_retornar(t_valor_variable retorno){
	printf("Hola, soy AnSISOP_retornar\n");
}

void AnSISOP_imprimirValor(t_valor_variable valor_mostrar){
	printf("Hola, soy AnSISOP_imprimirValor\n");
}

void AnSISOP_imprimirLiteral(char* texto){
	printf("Hola, soy AnSISOP_imprimirLiteral\n");
}

void AnSISOP_wait(t_nombre_semaforo identificador_semaforo){
	printf("Hola, soy AnSISOP_wait\n");
}

void AnSISOP_signal(t_nombre_semaforo identificador_semaforo){
	printf("Hola, soy AnSISOP_signal\n");
}

t_puntero AnSISOP_alocar(t_valor_variable espacio){
	printf("Hola, soy AnSISOP_alocar y estoy re loco\n");
	return NULL;
}

void AnSISOP_liberar(t_puntero puntero){
	printf("Hola, soy AnSISOP_liberar, vengo a liberarte de la esclavitud\n");
}










