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
#include <assert.h>
#include <parser/metadata_program.h>
#include <parser/metadata_program.h>
#include "../commons/structures.h"
#include "../commons/declarations.h"
#include <commons/collections/dictionary.h>
#include <commons/string.h>
#include <commons/string.h>
#include <commons/error.h>
#include "stack.h"
#include <parser/parser.h>
#include "../libSockets/client.h"
#include "../libSockets/recv.h"
#include "../libSockets/send.h"
#include "../libSockets/server.h"
#include "memory.h"

t_puntero VAR_SIZE = sizeof(t_valor_variable);

bool is_argument(t_nombre_variable identificador_variable)
{
	return identificador_variable >= '0' && identificador_variable <= '9';
}

t_puntero AnSISOP_definirVariable (t_nombre_variable identificador_variable) {
	printf("AnSISOP_definirVariable [%c]\n",identificador_variable);

	// checkear que haya lugar, si no marcar exit code en error
	if (pcb->stackPosition >= pcb->maxStackPosition)
	{
		printf("stackOverflow\n");
		pcb->exitCode = ERROR_MEMORY;
		// seguir ejecutando para que meta en el stack estas vars mas alla del stack (no pasa nada, todavia no va a memoria)
	}

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
	pcb->stackPosition += VAR_SIZE;
	printf("Finalizo AnSISOP_definirVariable\n");
 	return varStackPosition; // retorno donde empezaba la var que puse en stack*/
}

t_puntero AnSISOP_obtenerPosicionVariable(t_nombre_variable identificador_variable)
{
	printf("AnSISOP_obtenerPosicionVariable [%c]\n",identificador_variable);
	log_debug(logCPU, "[obtenerPosicionVariable] identificador = %c", identificador_variable);

	t_position* varPos;

	if (is_argument(identificador_variable))
	{
		log_info(logCPU, "ARG");
		varPos = stack_get_arg(identificador_variable);
	}
	else
	{
		log_info(logCPU, "VAR");
		varPos = stack_get_var(identificador_variable);
	}

	if (varPos == NULL){
		log_error(logCPU, "[obtenerPosicionVariable] no encontre %c", identificador_variable);
	}
	printf("Finalizo AnSISOP_obtenerPosicionVariable\n");
	return position_to_puntero(varPos);
}

t_valor_variable AnSISOP_dereferenciar(t_puntero direccion_variable){
	printf("AnSISOP_dereferenciar [%d]\n",direccion_variable);

	t_position pos = puntero_to_position(direccion_variable);

	log_debug(logCPU, "[dereferenciar] t_puntero = %d ---> page = %d, offset = %d", direccion_variable, pos.page, pos.off);

	void* readResult = memory_read(pcb->pid, pcb->cantPagsCodigo + pos.page, pos.off, VAR_SIZE);

	if (readResult == NULL)
	{
		pcb->exitCode = ERROR_MEMORY;
		log_error(logCPU, "[dereferenciar] no leyo bien de memoria");
	}

	log_debug(logCPU, "El valor de la variable ubicada en %d es: %d\n", direccion_variable, *((int*)readResult));
	printf("Finalizo AnSISOP_dereferenciar\n");
	return *((t_valor_variable*)readResult);
}

void AnSISOP_asignar (t_puntero direccion_variable, t_valor_variable valor){
	printf("AnSISOP_asignar a [%d] el valor [%d]\n", direccion_variable, valor);

	t_position pos = puntero_to_position(direccion_variable);
	log_info(logCPU, "[asignar] page = %d, offset = %d, valor = %d", pos.page, pos.off, valor);

	int writeResult = memory_write(pcb->pid, pcb->cantPagsCodigo + pos.page, pos.off, VAR_SIZE, &valor);

	if (writeResult == -1)
	{
		log_error(logCPU, "[asignar] error comunicandose con memoria");
	}
	else if (writeResult == ERROR_MEMORY) // leyo en algun lugar q no existia para el proceso (ej: trata de leer con var din y un offset que se pasa)
	{
		pcb->exitCode = ERROR_MEMORY;
	}

	printf("Finalizo AnSISOP_asignar\n");
}

// Cambia la linea de ejecucion a la correspondiente de la etiqueta buscada
void AnSISOP_irAlLabel (t_nombre_etiqueta etiqueta){
	printf("AnSISOP_irAlLabel [%s]\n",etiqueta);
	log_debug(logCPU, "[irAlLabel] %s\n", etiqueta);
	pcb->pc = metadata_buscar_etiqueta(etiqueta, pcb->indiceDeEtiquetas, pcb->indiceDeEtiquetasCant);
	printf("Finalizo AnSISOP_irAlLabel\n");
}

// Preserva el contexto de ejecución actual para poder retornar luego al mismo.
// Modifica las estructuras correspondientes para mostrar un nuevo contexto vacío.
// Los parámetros serán definidos luego de esta instrucción de la misma manera que una variable local,
// con identificadores numéricos empezando por el 0.
void AnSISOP_llamarSinRetorno(t_nombre_etiqueta etiqueta){
	printf("AnSISOP_llamarSinRetorno [%s]\n", etiqueta);

	t_indiceDelStack* ind = stack_context_create();
	list_add(pcb->indiceDeStack, ind);
	log_info(logCPU, "[llamarSinRetorno] Agrego nuevo contexto al indice de stack (%d contextos).\n", list_size(pcb->indiceDeStack));

	ind->retPos = pcb->pc;

	// irallabel?
	// mandarlo a ejecutar la funcion
	pcb->pc = metadata_buscar_etiqueta(etiqueta, pcb->indiceDeEtiquetas, pcb->indiceDeEtiquetasCant);
	printf("Finalizo AnSISOP_llamarSinRetorno\n");
}

//Preserva el contexto de ejecución actual para poder retornar luego al mismo, junto con la posicion de la variable entregada por donde_retornar.
//	 * Modifica las estructuras correspondientes para mostrar un nuevo contexto vacío.
void AnSISOP_llamarConRetorno(t_nombre_etiqueta etiqueta, t_puntero donde_retornar){
	printf("AnSISOP_llamarConRetorno [etiqueta: %s] [Retorno: %d]\n",etiqueta, donde_retornar);
	// llamar sin retorno?
	log_info(logCPU, "[llamarConRetorno] Recibo por parametro: %s y %d", etiqueta, donde_retornar);
	log_info(logCPU, "Es necesario volver al PC: %d", pcb->pc);

	t_indiceDelStack* ind = stack_context_create();


	ind->retPos=pcb->pc; // guardar instruccion de retorno
	log_info(logCPU, "Se debe retornar a: %d",ind->retPos);
	
	t_position returnToPos = puntero_to_position(donde_retornar);

	ind->retVar = (t_position*)malloc(sizeof(t_position));
	ind->retVar->page = returnToPos.page; // guardar variable donde poner retorno
	ind->retVar->off = returnToPos.off;
	ind->retVar->size = VAR_SIZE;

	list_add(pcb->indiceDeStack, ind);
	log_info(logCPU, "[llamarConRetorno] Agrego nuevo contexto al indice de stack (%d contextos).", list_size(pcb->indiceDeStack));
	log_info(logCPU,"Se agrega en retVar: %d|%d|%d",ind->retVar->page, ind->retVar->off, ind->retVar->size);
	log_info(logCPU, "[llamarConRetorno] Agrego nuevo contexto al indice de stack (%d contextos).", list_size(pcb->indiceDeStack));

	// mandarlo a ejecutar la funcion
	pcb->pc = metadata_buscar_etiqueta(etiqueta, pcb->indiceDeEtiquetas, pcb->indiceDeEtiquetasCant);
	printf("Finalizo AnSISOP_llamarConRetorno\n");
}

void AnSISOP_retornar(t_valor_variable retorno){
	printf("AnSISOP_retornar [%d]\n", retorno);
	log_info(logCPU, "[retornar] con valor %d.", retorno);
	t_indiceDelStack* currentContext = stack_context_current();
	if (currentContext->retVar != NULL)
	{
		AnSISOP_asignar(position_to_puntero(currentContext->retVar), retorno);
	}
	printf("Finalizo AnSISOP_retornar\n");
}

//Cambia el Contexto de Ejecución Actual para volver al Contexto anterior al que se está ejecutando,
// recuperando el Cursor de Contexto Actual y el Program Counter previamente apilados en el Stack.
// En caso de estar finalizando el Contexto principal (el ubicado al inicio del Stack),
// deberá finalizar la ejecución del programa.
void AnSISOP_finalizar (void)
{
	printf("AnSISOP_finalizar\n");
	//vuelvo el PC a la posicion de retorno de la primitiva
	t_indiceDelStack* currentStackContext = stack_context_current();

	pcb->pc = currentStackContext->retPos;

	// saco del stack el contexto actual
	stack_pop();

	if (list_is_empty(pcb->indiceDeStack)) // termino el main
	{
		log_info(logCPU, "[finalizar] fin programa.\n");
		pcb->exitCode = 0;
	}else{
		log_info(logCPU, "[finalizar] termina funcion. pc: %d. quedan %d contextos\n", pcb->pc, list_size(pcb->indiceDeStack));
	}
	printf("Finalizo el programa\n");
}


//----------------------------------------------------------------------------------------------
//Dummies, algunas voy a ver de llenarlas estos dias
t_valor_variable AnSISOP_obtenerValorCompartida(t_nombre_compartida variable){
	printf("AnSISOP_obtenerValorCompartida [%s]\n",variable);
	char* variable_compartida=string_new();
	string_append(&variable_compartida,"!");
	string_append(&variable_compartida,variable);
	int value;
	char* resp=string_new();
	//printf("Le pido al kernel el valor (copia) de la variable compartida.");
	if (socket_send_string(serverKernel,"getSharedVariable")>0){
		log_info(logCPU, "Le aviso al Kernel que necesito obtener el valor de una variable compartida");
	}else{
		log_info(logCPU, "Error al informar al Kernel que necesito obtener el valor de una variable compartida");
	}
	if (socket_send_string(serverKernel, variable_compartida)>0){
		log_info(logCPU, "Le envio al kernel el nombre de la variable compartida %s",variable_compartida)	;
	}else{
		log_info(logCPU, "Error al enviar al kernel el nombre de la variable compartida %s",variable_compartida)	;
	}
	if(socket_recv_string(serverKernel,&resp)>0){
		if (strcmp(resp,"Success")==0){
			if(socket_recv_int(serverKernel,&value)>0){
				log_info(logCPU, "Recibo el valor %d de variable %s", value, variable_compartida);
			}else{
				log_info(logCPU, "Error recibiendo el valor de la variable %s", variable_compartida);
			}
		}
	}else{
		log_info(logCPU, "Error recibiendo la respuesta del Kernel");
	}
	free(variable_compartida);
	free(resp);
	printf("Finalizo AnSISOP_obtenerValorCompartida\n");
	return value;
}

t_valor_variable AnSISOP_asignarValorCompartida(t_nombre_compartida variable, t_valor_variable valor){
	printf("AnSISOP_asignarValorCompartida [%s con valor %d]\n", variable, valor);

	log_info(logCPU, "Voy a asignar el valor %d a la variable compartida %s.", valor, variable);
	//Envio al kernel la variable
	char* variable_compartida=string_new();
	string_append(&variable_compartida,"!");
	string_append(&variable_compartida,variable);
	if (socket_send_string(serverKernel,"setSharedVariable")>0){
			log_info(logCPU, "Solicito al Kernel asignar el valor %d a la variable %s",valor,variable_compartida);
		}else{
			log_info(logCPU, "Error solicitando al Kernel asignal el valor %d a la variable %s", valor, variable_compartida);
		}
	if(socket_send_string(serverKernel,variable_compartida)>0){
		log_info(logCPU,"AnSISOP_asignarValorCompartida: Envio correctamente la variable %s\n",variable_compartida);
	}else{
		log_info(logCPU,"AnSISOP_asignarValorCompartida: Error enviando la variable %s\n",variable_compartida);
	}
	//Envio al kernel el valor que quiero que le setee a la variable
	if(socket_send_int(serverKernel,valor)>0){
		log_info(logCPU,"AnSISOP_asignarValorCompartida: Envio correctamente el valor %d de la variable %s\n",valor,variable);
	}else{
		log_info(logCPU,"AnSISOP_asignarValorCompartida: Error enviando el valor %d de la variable %s\n",valor,variable);
	}
	//Declaro la variable que voy a usar para recibir el valor seteado por kernel
	char* resultado=string_new();
	//Recibo del kernel el resultado de efectuar la operacion
	if(socket_recv_string(serverKernel,&resultado)>0){
		log_info(logCPU,"Se recibe correctamente el resultado de asignar el valor %d a la variable compartida %s",valor, variable_compartida);
		if (strcmp(resultado,"Success")==0){
			log_info(logCPU, "Se ha seteado correctamente el valor %d a la variable %s", valor, variable_compartida);
		}else{
			log_info(logCPU, "No se pudo setear el valor %d a la variable %s", valor, variable_compartida);
			EXIT_FAILURE;
		}
	}else{
		log_info(logCPU, "Error al recibir el resultado de asignar el valor %d a la variable compartida %s",valor, variable_compartida);
	}
	free(variable_compartida);
	free(resultado);
	printf("Finalizo AnSISOP_asignarValorCompartida\n");
	return valor;
}

void AnSISOP_wait(t_nombre_semaforo identificador_semaforo){
	printf("AnSISOP_wait [%s]\n", identificador_semaforo);
	if(socket_send_string(serverKernel,"wait")>0){
		printf("Le solicito al kernel que se haga WAIT al semaforo %s\n", identificador_semaforo);
		log_info(logCPU, "Le solicito al kernel que se haga WAIT al semaforo %s\n", identificador_semaforo);
	}

	if(socket_send_string(serverKernel,identificador_semaforo)>0){
		log_info(logCPU,"Envio al kernel el semaforo al que quiero hacer WAIT: %s\n",identificador_semaforo);
	}else{
		log_info(logCPU,"Error enviando al kernel el semaforo al que quiero hacer WAIT: %s\n",identificador_semaforo);
	}

	char* answerFromKernel=string_new();
	if (socket_recv_string(serverKernel,&answerFromKernel)>0){
		if (string_equals_ignore_case(answerFromKernel,"Failure")){
			log_info(logCPU, "No es posible hacer WAIT al semaforo %s porque no existe en Kernel. El programa finalizara", identificador_semaforo);
			// no hace falta poner el exit code porq el kernel se lo guarda
		}else{
			int resp;
			if(socket_recv_int(serverKernel,&resp)>0){
					if(resp==1){
						log_info(logCPU, "Se hizo el wait del semaforo %s", identificador_semaforo);
					}else{
						log_info(logCPU,"PID: %d se acaba de bloquear por hacer wait al semaforo %s\n",pcb->pid, identificador_semaforo);
					}
			}

		}
	}else{
		log_info(logCPU, "Error recibiendo respuesta del Kernel al haber pedido hacer el WAIT al semaforo %s\n", identificador_semaforo);
		exit(EXIT_FAILURE); // el kernel desconecta la cpu
	}

	free(answerFromKernel);
	printf("Finalizo AnSISOP_wait\n");
}

void AnSISOP_signal(t_nombre_semaforo identificador_semaforo){
	printf("AnSISOP_signal\n");
	log_info(logCPU, "Signal del semaforo: %s", identificador_semaforo);

	if (socket_send_string(serverKernel,"signal")>0){
		log_info(logCPU, "Le solicito al Kernel hacer Signal");
	}else{
		log_info(logCPU,"Error solicitandole al Kernel que haga Signal");
	}

	if (socket_send_string(serverKernel, identificador_semaforo)>0){
		log_info(logCPU, "Le envio al Kernel el semaforo al que quiero que le haga Signal: %s\n",identificador_semaforo);
	}else{
		log_info(logCPU, "Error enviando al Kernel el semaforo al que quiero que le haga Signal: %s\n",identificador_semaforo);
	}

	char* answerFromKernel=string_new();
	if (socket_recv_string(serverKernel,&answerFromKernel)>0){
			if (string_equals_ignore_case(answerFromKernel,"Success")){
				log_info(logCPU, "Se hizo SIGNAL al semaforo %s", identificador_semaforo);
			}else{
				log_info(logCPU, "No se pudo hacer SIGNAL al semaforo %s porque no existe en Kernel",identificador_semaforo);
			}
		}else{
			log_info(logCPU, "Error recibiendo respuesta del Kernel al haber pedido hacer el SIGNAL al semaforo %s\n", identificador_semaforo);
		}
	free(answerFromKernel);
	printf("Finalizo AnSISOP_signal\n");
}

t_puntero AnSISOP_alocar(t_valor_variable espacio){
	printf("AnSISOP_alocar [Espacio: %d]\n", espacio);
	log_info(logCPU, "Se llamo a la funcion alocar, para un espacio de: %d", espacio);

	if (socket_send_string(serverKernel, "alocar")>0){
		log_info(logCPU,"Le informo al Kernel que quiero alocar\n");
	}else{
		log_info(logCPU,"Error informando al Kernel que quiero alocar\n");
	}

	if(socket_send_int(serverKernel,espacio)>0){
		log_info(logCPU,"Le envio al Kernel el tamanio que necesito reservar en Heap: %d\n",espacio);
	}else{
		log_info(logCPU,"Error al Kernel el tamanio que necesito reservar en Heap: %d\n", espacio);
	}

	int puntero;
	t_puntero punteroFinal;
	if (socket_recv_int(serverKernel,&puntero)>0){
		log_info(logCPU,"Recibo el puntero donde se aloco el espacio: %d\n", puntero);
		 punteroFinal=(int)puntero;
	}

	printf("Finalizo AnSISOP_alocar\n");
	return punteroFinal;
}

void AnSISOP_liberar(t_puntero puntero){
	printf("AnSISOP_liberar el puntero [%d]\n",puntero);

	if (socket_send_string(serverKernel, "liberar")>0){
		log_info(logCPU,"[liberar] mensaje enviado\n");
	}else{
		log_info(logCPU,"[liberar] error mandando liberar\n");
	}

	//COMO VALIDAMOS QUE PUNTERO HAYA SIDO PREVIAMENTE ALOCADO?
	log_info(logCPU, "Se va a liberar el puntero: %d", puntero);

	if(socket_send_int(serverKernel,puntero)>0){
		log_info(logCPU,"Envio al kernel el puntero que quiero liberar: %d\n",puntero);
	}else{
		log_info(logCPU, "Error enviando al kernel el puntero que quiero liberar: %d\n", puntero);
	}

	printf("Finalizo AnSISOP_liberar\n");
}

void flagsAppend(char* string, t_banderas flags){
	if(flags.creacion){
		string_append(&string,string_from_format("%c",'c'));
	}
	if(flags.escritura){
		string_append(&string,string_from_format("%c",'w'));
	}
	if(flags.lectura){
		string_append(&string,string_from_format("%c",'r'));
	}
}

t_descriptor_archivo AnSISOP_abrir (t_direccion_archivo direccion, t_banderas flags){
	printf("AnSISOP_abrir [%s]\n", direccion);
	//Envio orden: abrir
	if (socket_send_string(serverKernel,"abrir")>0){
		log_info(logCPU,"Informo al Kernel que el programa %d quiere abrir un archivo\n", pcb->pid);
	}else{
		log_info(logCPU,"Error informando al Kernel que el programa %d quiero abrir un archivo\n", pcb->pid);
	}
	//Envio la ruta del archivo a abrir
	if(socket_send_string(serverKernel,direccion)>0){
		log_info(logCPU, "Envio correctamente la ruta del archivo: %s\n", direccion);
	}else{
		log_info(logCPU, "Error enviando la ruta del archivo: %s\n", direccion);
	}
	//Envio flags
	char* flagString=string_new();
	flagsAppend(flagString, flags);
	socket_send_string(serverKernel, flagString);
	//Recibo el descriptor de archivo
	int descriptor;
	t_descriptor_archivo descriptorPosta;
	if(socket_recv_int(serverKernel,&descriptor)>0){
		descriptorPosta=(t_descriptor_archivo)descriptor;
		log_info(logCPU,"Recibo correctamente el descriptor de archivo: %d\n", descriptorPosta);
	}else{
		log_info(logCPU, "Error recibiendo el descriptor de archivo\n");
	}
	printf("Finalizo AnSISOP_abrir\n");
	return descriptorPosta;
}
void AnSISOP_borrar (t_descriptor_archivo direccion){
	printf("AnSISOP_borrar [%d]\n", direccion);
	//Informo al kernel que el programa quiere borrar el archivo
	if (socket_send_string(serverKernel,"borrar")>0){
		log_info(logCPU, "Informo correctamente al Kernel que el programa %d quiere borrar el archivo ubicado en: %d\n",pcb->pid,direccion);
	}

	//Le envio al kernel la direccion del archivo que quiero borrar
	if (socket_send_int(serverKernel,direccion)>0){
		log_info(logCPU, "Envio correctamente la direccion al kernel: %d\n", direccion);
	}else{
		log_info(logCPU, "Error enviando la direccion al kernel: %d\n", direccion);
	}
	int resultado;
	if (socket_recv_int(serverKernel,&resultado)>0){
		if(resultado==1){
			log_info(logCPU, "Se borro el archivo con file descriptor %d con exito",direccion);
		}else{
			log_info(logCPU, "No se pudo borrar el archivo con file descriptor %d",direccion);
		}
	}else{
		log_info(logCPU, "Error enviando la direccion al kernel: %d\n", direccion);
	}
	printf("Finalizo AnSISOP_borrar\n");
}
void AnSISOP_cerrar (t_descriptor_archivo descriptor_archivo){
	printf("AnSISOP_cerrar [%d]\n",descriptor_archivo);
	//Informo al kernel que el programa quiere cerrar un archivo
	if (socket_send_string(serverKernel,"cerrar")>0){
		log_info(logCPU, "Informo correctamente que el programa %d quiere cerrar el archivo ubicado en %d\n", pcb->pid, descriptor_archivo);
	}else{
		log_info(logCPU, "Error informando que el programa %d quiere cerrar el archivo ubicado en %d\n", pcb->pid, descriptor_archivo);
	}

	//Envio el descriptor de archivo al kernel
	if (socket_send_int(serverKernel, descriptor_archivo)>0){
		log_info(logCPU, "Envio correctamente el descriptor de archivo: %d\n", descriptor_archivo);
	}else{
		log_info(logCPU, "Error enviando el descriptor de archivo: %d\n", descriptor_archivo);
	}

	//Recibo el resultado
	int resultado;
	if(socket_recv_int(serverKernel,&resultado)>0){
		if(resultado==1){
			log_info(logCPU, "Se cerro el archivo con file descriptor %d con exito", descriptor_archivo);
		}else{
			log_info(logCPU, "Error al solicitar cerrar archivo, Se recibio el error: %d", resultado);
		}
	}
	printf("Finalizo AnSISOP_cerrar\n");
}
void AnSISOP_moverCursor(t_descriptor_archivo descriptor_archivo, t_valor_variable posicion){
	printf("AnSISOP_moverCursor [descriptor: %d] [posicion: %d]\n", descriptor_archivo, posicion);
	//Informo al kernel que quiero mover cursor
	if (socket_send_string(serverKernel, "moverCursor")>0){
		log_info(logCPU, "Informo correctamente al kernel que el programa %d quiere mover el cursor en el archivo ubicado en %d a la posicion %d\n", pcb->pid, descriptor_archivo, posicion);
	}else{
		log_info(logCPU, "Error informando al kernel que el programa %d quiere mover el cursor en el archivo ubicado en %d a la posicion %d\n", pcb->pid, descriptor_archivo, posicion);
	}

	//Envio el descriptor de archivo
	if (socket_send_int(serverKernel, descriptor_archivo)>0){
		log_info(logCPU, "Envio correctamente el descriptor de archivo: %d\n", descriptor_archivo);
	}else{
		log_info(logCPU, "Error enviando el descriptor de archivo: %d\n", descriptor_archivo);
	}

	//Envio la posicion a la que quiero mover el cursor
	if (socket_send_int(serverKernel,posicion)>0){
		log_info(logCPU, "Envio correctamente la posicion a la que quiero mover el cursor: %d\n", posicion);
	}else{
		log_info(logCPU, "Error enviando la posicion a la que quiero mover el cursor: %d\n", posicion);
	}
	printf("Finalizo AnSISOP_moverCursor\n");
}
void AnSISOP_escribir(t_descriptor_archivo descriptor_archivo, void* informacion, t_valor_variable tamanio){
	printf("AnSISOP_escribir [Descriptor: %d] [Tamanio: %d]\n",descriptor_archivo, tamanio);
	//Informo al kernel que quiero escribir archivo
	printf("escribir\n");
	log_info(logCPU, "Descriptor: %d Tamanio: %d", descriptor_archivo, tamanio);
	if (socket_send_string(serverKernel, "escribir")>0){
		log_info(logCPU, "Informo correctamente al kernel que el programa %d quiere escribir un archivo\n", pcb->pid);
	}else{
		log_info(logCPU, "Error informando al kernel que el programa %d quiere escribir un archivo\n", pcb->pid);
	}

	//Envio al kernel el descriptor de archivo
	if (socket_send_int(serverKernel,descriptor_archivo)>0){
		log_info(logCPU, "Envio correctamente el descriptor de archivo al Kernel: %d\n", descriptor_archivo);
	}else{
		log_info(logCPU, "Error enviando el descriptor de archivo al Kernel: %d\n", descriptor_archivo);
	}

	//Envio al kernel la informacion con su tamanio
	if (socket_send(serverKernel, informacion,tamanio)>0){
		log_info(logCPU, "Envio correctamente la informacion %s cuyo tamanio es %d\n", (char*)informacion,tamanio);
	}else{
		log_info(logCPU, "Error enviando la informacion %s cuyo tamanio es %d\n", (char*)informacion,tamanio);
	}

	printf("Finalizo AnSISOP_escribir\n");
}
void AnSISOP_leer(t_descriptor_archivo descriptor_archivo, t_puntero posicionMemoria, t_valor_variable tamanio){
	printf("AnSISOP_leer [Descriptor: %d] [Donde: %d] [Tamanio: %d]\n",descriptor_archivo, posicionMemoria, tamanio);
	//Informo al kernel que quiero leer archivo
	if (socket_send_string(serverKernel,"leer")>0){
		log_info(logCPU, "Informo correctamente al kernel que el programa %d quiere leer un archivo\n", pcb->pid);
	}else{
		log_info(logCPU, "Error informando al kernel que el programa %d quiere leer un archivo\n", pcb->pid);
	}

	// descriptor de archivo
	if (socket_send_int(serverKernel, (int)descriptor_archivo)>0){
		log_info(logCPU, "Envio correctamente el descriptor de archivo: %d\n", descriptor_archivo);
	}else{
		log_info(logCPU, "Error enviando el descriptor de archivo: %d\n", descriptor_archivo);
	}

	// pos memoria
	if (socket_send_int(serverKernel,posicionMemoria)>0){
		log_info(logCPU, "Envio correctamente el puntero donde quiero se guarde la informacion leida: %d\n", posicionMemoria);
	}else{
		log_info(logCPU, "Error enviando el puntero donde quiero se guarde la informacion leida: %d\n", posicionMemoria);
	}

	// tamanio de lo que quiero leer
	if (socket_send_int(serverKernel, tamanio)>0){
		log_info(logCPU, "Envio correctamente el tamanio de lo que quiero leer: %d\n", tamanio);
	}else{
		log_info(logCPU, "Error enviando el tamanio de lo que quiero leer: %d\n", tamanio);
	}

	//Recibo la respuesta del Kernel
	int resp;
	if (socket_recv_int(serverKernel,&resp)>0){
		if (resp==1){
			log_info(logCPU, "Se leyo correctamente el archivo solicitado");
		}else{
			log_info(logCPU, "El programa intentó leer un archivo sin permisos");
			EXIT_FAILURE;
		}
	}
	printf("Finalizo AnSISOP_leer\n");
}
