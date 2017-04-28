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



int off=0;
int currentPage=0;
int size=4;

// cambiar a t_puntero (sin *)
// se puede entonces retornar un int pos->data * pageSize + pos->offset
t_puntero* AnSISOP_definirVariable (t_nombre_variable identificador_variable){
	//Hay que liberar pos por los memory leaks
	t_pos* pos= (t_pos*)malloc(sizeof(t_pos));
	if(off+size>pageSize){
		currentPage++;
	}
	pos->page=currentPage;
	pos->off=off;
	pos->size=size;
	list_add(pcb->indiceDeStack,pos);
 	//dictionary_put(pcb->indiceDeStack,&identificador_variable,(void*)pos);
 	off+=size;
 	//Hay que liberar p por los mem leaks
 	t_puntero* p=(t_puntero*)malloc(sizeof(t_puntero));
 	*p=currentPage*pageSize+off;
 	return p;
}

// esto te lo ahorras si usas diccionario
t_pos* find_identificador(t_nombre_variable ident){
	int _is_the_id(t_indiceDelStack* s){
		return s->ID==ident;
	}
	return list_find(pcb->indiceDeStack, (void*)find_identificador);
}

// cambiar a t_puntero (sin *)
// se puede entonces retornar un int pos->data * pageSize + pos->offset
// no hace falta allocar nada
// acordate de multiplicar pos->page por pageSize!s
t_puntero* AnSISOP_obtenerPosicionVariable(t_nombre_variable identificador_variable){
	t_puntero* p = (t_puntero*)malloc(sizeof(t_puntero));
	//t_pos* data=dictionary_get(pcb->indiceDeStack,&identificador_variable);
	t_pos* data = (t_pos*)find_identificador(identificador_variable);
	*p=data->page+data->off;
	return p;
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

// cambiar a t_valor_variable (sin *), (un int)
// alcanza con devolver lo que te manda el kernel
t_valor_variable *AnSISOP_obtenerValorCompartida(t_nombre_compartida variable){
	int* value=(int*)malloc(sizeof(int));
	if (socket_send_string(serverKernel, "ValueOfSharedVariable")>0){
		if (socket_send_string(serverKernel,variable)>0){

			socket_recv_int(serverKernel,value);
		}
	}
	return value;
}
