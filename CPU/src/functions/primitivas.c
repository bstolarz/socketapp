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


//----------------------------------------------------------------------------------------------
//Dummies, algunas voy a ver de llenarlas estos dias
t_valor_variable AnSISOP_obtenerValorCompartida(t_nombre_compartida variable){
	log_info(logCPU, "Voy a obtener el valor de variable Compartida: %s.", variable);
	printf("Le pido al kernel el valor (copia) de la variable compartida.");

	int valorVariable;
	socket_recv_int(serverKernel, &valorVariable);
	log_info(logCPU, "El valor de la variable compartida %s es %d.", variable, valorVariable);

	return valorVariable;
}

t_valor_variable AnSISOP_asignarValorCompartida(t_nombre_compartida variable, t_valor_variable valor){
	log_info(logCPU, "Voy a asignar el valor %d a la variable compartida %s.", valor, variable);

	//hacer malloc de la variable compartida (es una estructura?)
	//asignarle a la variable compartida el nombre y el valor

	//Mandarsela al kernel para que la asigne
	return valor;
}


void AnSISOP_retornar(t_valor_variable retorno){
	log_info(logCPU, "Se invoco a la funcion AnSISOP_retornar.");

	t_indiceDelStack* currentContext = stack_context_current();

	pcb->pc = currentContext->retPos;
	stack_context_pop();

	//Calculo la direccion de retorno en base al retVar del contexto
	t_puntero direccion = (currentContext->retVar.page * VAR_SIZE) + currentContext->retVar.off;
	AnSISOP_asignar(direccion, retorno);
}

void AnSISOP_imprimirValor(t_valor_variable valor_mostrar){
	log_info(logCPU, "Se solicito imprimir el valor: %d", valor_mostrar);

	if(socket_send_string(serverKernel, "IMPVAR")<=0){
		if(socket_send_int(serverKernel, valor_mostrar)<=0){

		}
		else{
			log_info(logCPU, "No se pudo enviar el valor para que el kernel lo imprima.");
		}
	}
	else{
		log_info(logCPU, "No se pudo enviar la directiva de imprimir valor al kernel.");
	}


}

void AnSISOP_imprimirLiteral(char* texto){
	log_info(logCPU, "Se solicito imprimir la cadena literal: %s", texto);

	if(socket_send_string(serverKernel, "IMPLIT")<=0){
		if(socket_send_string(serverKernel, texto)<=0){

		}
		else{
			log_info(logCPU, "No se pudo enviar la cadena literal para que el kernel la imprima.");
		}
	}
	else{
		log_info(logCPU, "No se pudo enviar la directiva de imprimir literal al kernel.");
	}

}

void AnSISOP_wait(t_nombre_semaforo identificador_semaforo){
	printf("Hola, soy AnSISOP_wait\n");
}

void AnSISOP_signal(t_nombre_semaforo identificador_semaforo){
	log_info(logCPU, "Signal del semaforo: %s", identificador_semaforo);

	if(socket_send_string(serverKernel, "SIGNAL")<=0){
		if(socket_send_string(serverKernel, identificador_semaforo)<=0){

		}
		else{
			log_info(logCPU, "No se pudo enviar el identificador del semaforo para que el kernel le haga signal.");
		}
	}
	else{
		log_info(logCPU, "No se pudo enviar la directiva de SIGNAL al kernel.");
	}

}

t_puntero AnSISOP_alocar(t_valor_variable espacio){
	log_info(logCPU, "Se llamo a la funcion alocar, para un espacio de: %d", espacio);

	//Si este puntero es local entonces despues como le hago free en AnSISOP_liberar?
	t_puntero puntero = malloc(sizeof(t_valor_variable));

	if(socket_send_string(serverKernel, "ALOCAR")<=0){
		if(socket_send_int(serverKernel, espacio)<=0){

		}
		else{
			log_info(logCPU, "No se pudo enviar el espacio para que el kernel lo aloque.");
		}
	}
	else{
		log_info(logCPU, "No se pudo enviar la directiva de alocar al kernel.");
	}


	return puntero;
}

void AnSISOP_liberar(t_puntero puntero){
	log_info(logCPU, "Se va a liberar el puntero: %d", puntero);

	//aca haria el free del puntero al que le hice malloc en AnSISOP_alocar?

	if(socket_send_string(serverKernel, "LIBERAR")<=0){
		if(socket_send_int(serverKernel, puntero)<=0){

		}
		else{
			log_info(logCPU, "No se pudo enviar el puntero para que el kernel lo libere.");
		}
	}
	else{
		log_info(logCPU, "No se pudo enviar la directiva de liberar al kernel.");
	}

}










