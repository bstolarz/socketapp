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


int off=0;
int currentPage=0;
int size=4;
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

t_pos* find_identificador(t_nombre_variable ident){
	int _is_the_id(t_indiceDelStack* s){
		return s->ID==ident;
	}
	return list_find(pcb->indiceDeStack, (void*)find_identificador);
}
t_valor_variable* find_position(t_puntero direccion_variable){
	int _is_the_position(t_indiceDelStack* s){
		return s->pos->page+s->pos->off==direccion_variable;
	}
	return list_find(pcb->indiceDeStack,(void*)find_position);
}
t_puntero* AnSISOP_obtenerPosicionVariable(t_nombre_variable identificador_variable){
	t_puntero* p = (t_puntero*)malloc(sizeof(t_puntero));
	//t_pos* data=dictionary_get(pcb->indiceDeStack,&identificador_variable);
	t_pos* data=(t_pos*)find_identificador(identificador_variable);
	*p=data->page+data->off;
	return p;
}
t_valor_variable *AnSISOP_dereferenciar(t_puntero direccion_variable){
	return find_position(direccion_variable);

}
