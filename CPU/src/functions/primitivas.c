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
int pos=0;
// cambiar a t_puntero (sin *)
// se puede entonces retornar un int pos->data * pageSize + pos->offset
t_puntero AnSISOP_definirVariable (t_nombre_variable identificador_variable){
	//Hay que liberar pos por los memory leaks
	t_var* var= (t_var*)malloc(sizeof(t_var));
	if(off+size>pageSize){
		currentPage++;
	}
	var->page=currentPage;
	var->off=off;
	var->size=size;
	t_indiceDelStack* ind=(t_indiceDelStack*)list_get(pcb->indiceDeStack,0);
	dictionary_put(ind->vars,string_from_format("%c",identificador_variable),var);
	int off_defined=off;
	off+=size;
 	//Hay que liberar p por los mem leaks
 	return currentPage*pageSize+off_defined;

}

// esto te lo ahorras si usas diccionario
/*t_pos* find_identificador(t_nombre_variable ident){
	int _is_the_id(t_pos* pos){
		return pos==ident;
	}
	return list_find(pcb->indiceDeStack, (void*)find_identificador);
}*/

// cambiar a t_puntero (sin *)
// se puede entonces retornar un int pos->data * pageSize + pos->offset
// no hace falta allocar nada
// acordate de multiplicar pos->page por pageSize!s
t_puntero AnSISOP_obtenerPosicionVariable(t_nombre_variable identificador_variable){
	t_indiceDelStack* ind=(t_indiceDelStack*)list_get(pcb->indiceDeStack,0);
	//t_var* data=dictionary_get(pcb->indiceDeStack,&identificador_variable);
	t_var* data=dictionary_get(ind->vars,string_from_format("%c",identificador_variable));
	//t_var* data = (t_var*)dictionary_get(varsAux,string_from_format("%c",identificador_variable));
	return data->page*pageSize+data->off;

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
/*t_valor_variable *AnSISOP_obtenerValorCompartida(t_nombre_compartida variable){
	int* value=(int*)malloc(sizeof(int));
	if (socket_send_string(serverKernel, "ValueOfSharedVariable")>0){
		if (socket_send_string(serverKernel,variable)>0){

			socket_recv_int(serverKernel,value);
		}
	}
	return value;
}*/
void AnSISOP_irAlLabel (t_nombre_etiqueta t_nombre_etiqueta){
	pcb->pc=(int)dictionary_get(pcb->indiceDeEtiquetas,t_nombre_etiqueta);
}
void AnSISOP_llamarSinRetorno(t_nombre_etiqueta etiqueta){
	int offset
	t_var* v=(t_var*)malloc(sizeof(t_var));
	//verifico que pueda guardar el contexto en la misma pagina
	if(off+size>pageSize){
			currentPage++;
			offset=0;
	}else{
		offset+=size;
	}
	int writeResult = memory_request_write(serverMemory, pcb->pid, currentPage, offsetToMemory, size, &valor);
	if (writeResult == -1)	log_error(logCPU, "[asignar] error al escribir en memoria");
	else					log_info(logCPU, "[asignar] se escribio bien en memoria");

}
