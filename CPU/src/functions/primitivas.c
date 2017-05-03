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

int off=0;
int currentPage=0;
int size=4;
int pos=0;

bool is_argument(t_nombre_variable identificador_variable)
{
	return isdigit(identificador_variable);
}

t_puntero AnSISOP_definirVariable (t_nombre_variable identificador_variable){
	t_indiceDelStack* currentContext = stack_context_current();

	t_position* memoryPos = (t_position*)malloc(sizeof(t_position));
	if(off+size>pageSize){
		currentPage++;
		off=0;
	}
	memoryPos->page=currentPage;
	memoryPos->off=off;
	memoryPos->size = size;

	if (is_argument(identificador_variable))
	{
		// argumento => va en args
		int index = identificador_variable - '0';
		currentContext->args[index] = memoryPos;

		log_info(logCPU,"[definirVar] arg {%c} en (pag: %d, offset: %d y size: %d)\n",
				identificador_variable, memoryPos->page, memoryPos->off, memoryPos->size);
	}
	else
	{
		// variable local => va en vars
		dictionary_put(currentContext->vars, string_from_format("%c", identificador_variable), memoryPos);

		log_info(logCPU,"[definirVar] var local {%c} en (pag: %d, offset: %d y size: %d)\n",
				identificador_variable, memoryPos->page, memoryPos->off, memoryPos->size);
	}

	//Guardo el valor a retornar por la funcion AnSISOP
	int off_defined=off;

	//Actualizo el offset
	off+=size;

 	return currentPage*pageSize+off_defined;

}

t_puntero AnSISOP_obtenerPosicionVariable(t_nombre_variable identificador_variable){

	t_indiceDelStack* currentContext = stack_context_current();
	t_position* varPos;

	if (is_argument(identificador_variable))
	{
		int index = identificador_variable - '0';
		varPos = currentContext->args[index];
	}
	else
	{
		char keyStr[2] = { identificador_variable, '\0' };
		varPos = dictionary_get(currentContext->vars, keyStr);
	}

	return varPos->page * pageSize + varPos->off;
}

t_valor_variable AnSISOP_dereferenciar(t_puntero direccion_variable){
	div_t values = div(direccion_variable, pageSize);
	int page = values.quot;
	int offsetToMemory = values.rem;

	log_info(logCPU, "[dereferenciar] page = %d, offset = %d\n", page, offsetToMemory);
	void* readResult = memory_request_read(serverMemory, pcb->pid, page, offsetToMemory, size);

	if (readResult == NULL)	log_error(logCPU, "[dereferenciar] no leyo bien de memoria");
	else					log_info(logCPU, "[dereferenciar] leyo bien");

	return *((int*)readResult);
}

void AnSISOP_asignar (t_puntero direccion_variable, t_valor_variable valor){
	div_t values = div(direccion_variable, pageSize);
	int page = values.quot;
	int offsetToMemory = values.rem;

	log_info(logCPU, "[asignar] page = %d, offset = %d, valor = %d\n", page, offsetToMemory, valor);
	int writeResult = memory_request_write(serverMemory, pcb->pid, page, offsetToMemory, size, &valor);

	if (writeResult == -1)	log_error(logCPU, "[asignar] error al escribir en memoria");
	else					log_info(logCPU, "[asignar] se escribio bien en memoria");
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

void AnSISOP_irAlLabel (t_nombre_etiqueta t_nombre_etiqueta){
	pcb->pc = metadata_buscar_etiqueta(t_nombre_etiqueta, pcb->indiceDeEtiquetas, pcb->indiceDeEtiquetasCant);
}
// Preserva el contexto de ejecución actual para poder retornar luego al mismo.
// Modifica las estructuras correspondientes para mostrar un nuevo contexto vacío.
 //Los parámetros serán definidos luego de esta instrucción de la misma manera que una variable local, con identificadores numéricos empezando por el 0.

//Preserva el contexto de ejecución actual para poder retornar luego al mismo.
//Modifica las estructuras correspondientes para mostrar un nuevo contexto vacío.
void AnSISOP_llamarSinRetorno(t_nombre_etiqueta etiqueta){
	pos++;

	//verifico que pueda guardar el contexto en la misma pagina
	if(off+size>pageSize){
			currentPage++;
			off=0;
	}else{
		off+=size;
	}

	t_position* arg=(t_position*)malloc(sizeof(t_position));
	arg->page=currentPage;
	arg->off=off;
	arg->size=size;

	t_indiceDelStack* ind=(t_indiceDelStack*)malloc(sizeof(t_indiceDelStack));
	//ind->args=arg;
	ind->retPos=pcb->pc;
	list_add(pcb->indiceDeStack, ind);
	log_info(logCPU, "Agrego una entrada en el indice de stack en la posicion %d. Valores:\nPagina: %d\nOffset: %d\nSize: %d\n",pos,arg->page, arg->off, arg->size);
	int writeResult = memory_request_write(serverMemory, pcb->pid, currentPage, off, size, etiqueta);
	if (writeResult == -1)	log_error(logCPU, "[asignar] error al escribir en memoria");
	else					log_info(logCPU, "[asignar] se escribio bien en memoria");

}

//falta completar, pregunta en el foro hecha
//Preserva el contexto de ejecución actual para poder retornar luego al mismo, junto con la posicion de la variable entregada por donde_retornar.
	//	 * Modifica las estructuras correspondientes para mostrar un nuevo contexto vacío.
void AnSISOP_llamarConRetorno(t_nombre_etiqueta etiqueta, t_puntero donde_retornar){
	pos++;
		t_position* r=(t_position*)malloc(sizeof(t_position));
		//verifico que pueda guardar el contexto en la misma pagina
		if(off+size>pageSize){
				currentPage++;
				off=0;
		}else{
			off+=size;
		}
	t_indiceDelStack* ind=(t_indiceDelStack*)malloc(sizeof(t_indiceDelStack));
	//GUARDO LA POSICION DE RETORNO AL FINALIZAR LA PRIMITIVA
	ind->retPos=pcb->pc;
	div_t d=div(donde_retornar,pageSize);
	int pagina=d.quot;
	int off=d.rem;

	ind->retVar.page=pagina;
	ind->retVar.off=off;
	ind->retVar.size=size;

	list_add_in_index(pcb->indiceDeStack,pos,ind);
	log_info(logCPU, "Agrego una entrada en la posicion %d del indice de Stack. Los valores de 'args' son: Page %d, Off %d, Size %d\n", pos, r->page, r->off, r->size);
	//Me comunico con memoria para escribir
	int writeResult = memory_request_write(serverMemory, pcb->pid, currentPage, off, size, etiqueta);
		if (writeResult == -1)	log_error(logCPU, "[asignar] error al escribir en memoria");
		else					log_info(logCPU, "[asignar] se escribio bien en memoria");

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
	stack_context_pop();

	bool programCompleted = list_is_empty(pcb->indiceDeStack);

	if (programCompleted) // termino el main
	{
		printf("avisar a kernel el fin o marcar un bool, etc");
	}
	else // termino cualquier otra funcion
	{
		// usar ret pos para algo?
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










