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
	t_pos* pos= (t_pos*)malloc(sizeof(t_pos));
	if(off+size>pageSize){
		currentPage++;
	}
	pos->page=currentPage;
	pos->off=off;
	pos->size=size;
 	dictionary_put(pcb->indiceDeStack,&identificador_variable,(void*)pos);
 	off+=size;
 	t_puntero* p=(t_puntero*)malloc(sizeof(t_puntero));
 	*p=currentPage*pageSize+off;
 	return p;
}

