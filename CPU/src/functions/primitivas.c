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
	//Alloco memoria para almacenar una variable
	t_var* var= (t_var*)malloc(sizeof(t_var));
	if(off+size>pageSize){
		currentPage++;
		off=0;
	}

	//Como es variable local, le asigno pagina, offset y size
	var->pos->page=currentPage;
	var->pos->off=off;
	var->pos->size=size;

	//Alloco una entrada en el indice de stack
	t_indiceDelStack* ind=(t_indiceDelStack*)malloc(sizeof(t_indiceDelStack));

	//Almaceno los valores de la variable local en el diccionario vars de la entrada que recien aloqué
	dictionary_put(ind->vars,string_from_format("%c",identificador_variable),var);

	//Agrego la entrada en el indice del stack
	list_add(pcb->indiceDeStack,ind);

	//Guardo el valor a retornar por la funcion AnSISOP
	int off_defined=off;

	//Actualizo el offset
	off+=size;

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
	return data->pos->page*pageSize+data->pos->off;

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
	int offset;
	t_var* v=(t_var*)malloc(sizeof(t_var));
	//verifico que pueda guardar el contexto en la misma pagina
	if(off+size>pageSize){
			currentPage++;
			offset=0;
	}else{
		offset=off+size;
	}
	int writeResult = memory_request_write(serverMemory, pcb->pid, currentPage, offset, size, etiqueta);
	if (writeResult == -1)	log_error(logCPU, "[asignar] error al escribir en memoria");
	else					log_info(logCPU, "[asignar] se escribio bien en memoria");

}

//falta completar, pregunta en el foro hecha
void AnSISOP_llamarConRetorno(t_nombre_etiqueta etiqueta, t_puntero donde_retornar){
	int offset;
		t_var* v=(t_var*)malloc(sizeof(t_var));
		//verifico que pueda guardar el contexto en la misma pagina
		if(off+size>pageSize){
				currentPage++;
				offset=0;
		}else{
			offset=off+size;
		}
	t_indiceDelStack* ind=(t_indiceDelStack*)malloc(sizeof(t_indiceDelStack));
	//GUARDO LA POSICION DE RETORNO AL FINALIZAR LA PRIMITIVA
	ind->retPos=pcb->pc;
	div_t d=div(donde_retornar,pageSize);
	int pagina=d.quot;
	int off=d.rem;
	v->pos->page=pagina;
	v->pos->off=off;
	v->pos->size=size;
	//GUARDO LA POSICION A LA QUE RETORNA LA FUNCION
	ind->retVar=v;
}
//Cambia el Contexto de Ejecución Actual para volver al Contexto anterior al que se está ejecutando, recuperando el Cursor de Contexto Actual y el Program Counter previamente apilados en el Stack.
// En caso de estar finalizando el Contexto principal (el ubicado al inicio del Stack), deberá finalizar la ejecución del programa.
void args_destroyer(t_var* v){
	free(v->pos);
	free(v);
}
void retVar_destroyer (t_var* r){
	free(r->pos);
	free(r);
}
void vars_destroyer(t_var* v){
	free(v->pos);
	free(v);
}
void AnSISOP_finalizar (void){
	//busco retPos
	int ult=list_size(pcb->indiceDeStack)-1;
	t_indiceDelStack* ind=list_get(pcb->indiceDeStack,ult);
	//vuelvo el PC a la posicion de retorno de la primitiva
	pcb->pc=ind->retPos;
	if (pcb->pc==0){
		//limpio vars (Variables locales del programa
		dictionary_clean_and_destroy_elements(ind->args,(void*)vars_destroyer);
	}else{
		//limpio las variables locales de la funcion que finaliza
		dictionary_clean_and_destroy_elements(ind->args,(void*)args_destroyer);
		//limpio los retornos locales
		dictionary_clean_and_destroy_elements(ind->args,(void*)retVar_destroyer);
	}
}
