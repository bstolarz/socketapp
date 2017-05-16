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

 	return varStackPosition; // retorno donde empezaba la var que puse en stack*/
}

t_puntero AnSISOP_obtenerPosicionVariable(t_nombre_variable identificador_variable)
{
	log_debug(logCPU, "[obtenerPosicionVariable] identificador = %c", identificador_variable);

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
		log_error(logCPU, "[obtenerPosicionVariable] no encontre %c", identificador_variable);

	return position_to_puntero(varPos);
}

t_valor_variable AnSISOP_dereferenciar(t_puntero direccion_variable){
	printf("[dereferenciar]: %d\n", direccion_variable);
	t_position pos = puntero_to_position(direccion_variable);

	log_debug(logCPU, "[dereferenciar] t_puntero = %d ---> page = %d, offset = %d", direccion_variable, pos.page, pos.off);

	void* readResult = memory_read(pcb->pid, pcb->cantPagsCodigo + pos.page, pos.off, VAR_SIZE);

	if (readResult == NULL)	log_error(logCPU, "[dereferenciar] no leyo bien de memoria");
	log_debug(logCPU, "El valor de la variable ubicada en %d es: %d\n", direccion_variable, *((int*)readResult));

	return *((t_valor_variable*)readResult);
}

void AnSISOP_asignar (t_puntero direccion_variable, t_valor_variable valor){
	printf("[asignar]: %d en la direccion %d\n", valor, direccion_variable);
	t_position pos = puntero_to_position(direccion_variable);
	log_info(logCPU, "[asignar] page = %d, offset = %d, valor = %d", pos.page, pos.off, valor);

	int writeResult = memory_write(pcb->pid, pcb->cantPagsCodigo + pos.page, pos.off, VAR_SIZE, &valor);

	if (writeResult == -1)	log_error(logCPU, "[asignar] error al escribir en memoria");
}

// Cambia la linea de ejecucion a la correspondiente de la etiqueta buscada
void AnSISOP_irAlLabel (t_nombre_etiqueta etiqueta){
	log_debug(logCPU, "[irAlLabel] %s", etiqueta);
	pcb->pc = metadata_buscar_etiqueta(etiqueta, pcb->indiceDeEtiquetas, pcb->indiceDeEtiquetasCant);
}

// Preserva el contexto de ejecución actual para poder retornar luego al mismo.
// Modifica las estructuras correspondientes para mostrar un nuevo contexto vacío.
// Los parámetros serán definidos luego de esta instrucción de la misma manera que una variable local,
// con identificadores numéricos empezando por el 0.
void AnSISOP_llamarSinRetorno(t_nombre_etiqueta etiqueta){
	log_info(logCPU, "[llamarSinRetorno] Agrego nuevo contexto al indice de stack.");


	t_indiceDelStack* ind = stack_context_create();

	ind->retPos = pcb->pc;
	list_add(pcb->indiceDeStack, ind);

	// irallabel?
	// mandarlo a ejecutar la funcion
	pcb->pc = metadata_buscar_etiqueta(etiqueta, pcb->indiceDeEtiquetas, pcb->indiceDeEtiquetasCant);
}

//Preserva el contexto de ejecución actual para poder retornar luego al mismo, junto con la posicion de la variable entregada por donde_retornar.
//	 * Modifica las estructuras correspondientes para mostrar un nuevo contexto vacío.
void AnSISOP_llamarConRetorno(t_nombre_etiqueta etiqueta, t_puntero donde_retornar){

	// llamar sin retorno?
	log_info(logCPU, "[llamarConRetorno] Agrego nuevo contexto al indice de stack.");

	t_indiceDelStack* ind = stack_context_create();

	ind->retPos=pcb->pc; // guardar instruccion de retorno
	
	t_position returnToPos = puntero_to_position(donde_retornar);
	ind->retVar = malloc(sizeof(t_puntero));
	ind->retVar->page = returnToPos.page; // guardar variable donde poner retorno
	ind->retVar->off = returnToPos.off;
	ind->retVar->size = VAR_SIZE;

	list_add(pcb->indiceDeStack, ind);

	// mandarlo a ejecutar la funcion
	pcb->pc = metadata_buscar_etiqueta(etiqueta, pcb->indiceDeEtiquetas, pcb->indiceDeEtiquetasCant);
}

void AnSISOP_retornar(t_valor_variable retorno){
	log_info(logCPU, "[retornar] con valor %d.", retorno);

	t_indiceDelStack* currentContext = stack_context_current();

	pcb->pc = currentContext->retPos;

	if (currentContext->retVar != NULL)
	{
		AnSISOP_asignar(position_to_puntero(currentContext->retVar), retorno);
	}
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

	if (list_is_empty(pcb->indiceDeStack)) // termino el main
	{
		log_info(logCPU, "[finalizar] fin programa.");
		pcb->exitCode = 0;
	}else{
		log_info(logCPU, "[finalizar] termina funcion.");
	}

}


//----------------------------------------------------------------------------------------------
//Dummies, algunas voy a ver de llenarlas estos dias
t_valor_variable AnSISOP_obtenerValorCompartida(t_nombre_compartida variable){
	log_info(logCPU, "Voy a obtener el valor de variable Compartida: %s.", variable);
	printf("Le pido al kernel el valor (copia) de la variable compartida.");
	socket_send_string(serverKernel,"getSharedVariable");
	int valorVariable;
	socket_recv_int(serverKernel, &valorVariable);
	log_info(logCPU, "El valor de la variable compartida %s es %d.", variable, valorVariable);

	return valorVariable;
}

t_valor_variable AnSISOP_asignarValorCompartida(t_nombre_compartida variable, t_valor_variable valor){
	log_info(logCPU, "Voy a asignar el valor %d a la variable compartida %s.", valor, variable);
	//Envio al kernel la variable
	if(socket_send_string(serverKernel,variable)>0){
		log_info(logCPU,"AnSISOP_asignarValorCompartida: Envio correctamente la variable %s\n",variable);
	}else{
		log_info(logCPU,"AnSISOP_asignarValorCompartida: Error enviando la variable %s\n",variable);
	}
	//Envio al kernel el valor que quiero que le setee a la variable
	if(socket_send_int(serverKernel,valor)>0){
		log_info(logCPU,"AnSISOP_asignarValorCompartida: Envio correctamente el valor %d de la variable %s\n",valor,variable);
	}else{
		log_info(logCPU,"AnSISOP_asignarValorCompartida: Error enviando el valor %d de la variable %s\n",valor,variable);
	}
	//Declaro la variable que voy a usar para recibir el valor seteado por kernel
	int valorAsignado;
	//Recibo del kernel el valor asignado
	if(socket_recv_int(serverKernel,&valorAsignado)>0){
		log_info(logCPU,"AnSISOP_asignarValorCompartida: Recibo correctamente el valor %d de la variable %s\n",valorAsignado,variable);;
	}
	return valorAsignado;
}

void AnSISOP_imprimirValor(t_valor_variable valor_mostrar){
	log_info(logCPU, "Se solicito imprimir el valor: %d", valor_mostrar);

	if(socket_send_string(serverKernel, "imprimirValor")>0){
		if(socket_send_int(serverKernel, valor_mostrar)>0){

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

	if(socket_send_string(serverKernel, "imprimirLiteral")>0){
		if(socket_send_string(serverKernel, texto)>0){

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
	char* answerFromKernel=string_new();
	if(socket_send_string(serverKernel,"wait")>0){
		printf("Le solicito al kernel que se haga WAIT al semaforo %s\n", identificador_semaforo);
		log_info(logCPU, "Le solicito al kernel que se haga WAIT al semaforo %s\n", identificador_semaforo);
	}
	if(socket_send_string(serverKernel,identificador_semaforo)>0){
		log_info(logCPU,"Envio al kernel el semaforo al que quiero hacer WAIT: %s\n",identificador_semaforo);
	}else{
		log_info(logCPU,"Error enviando al kernel el semaforo al que quiero hacer WAIT: %s\n",identificador_semaforo);
	}
	if (socket_recv_string(serverKernel,&answerFromKernel)>0){
		printf("PID: %d se acaba de bloquear por hacer wait al semaforo %s\n",pcb->pid, identificador_semaforo);
	}else{
		log_info(logCPU, "Error recibiendo respuesta del Kernel al haber pedido hacer el WAIT al semaforo %s\n", identificador_semaforo);
	}
	free(answerFromKernel);
}

void AnSISOP_signal(t_nombre_semaforo identificador_semaforo){
	log_info(logCPU, "Signal del semaforo: %s", identificador_semaforo);
	if (socket_send_string(serverKernel,"signal")>0){
		log_info(logCPU, "Le solicito al Kernel hacer Signal");
	}else{
		log_info(logCPU,"Error solicitandole al Kernel que haga Signal");
	}

	if(socket_send_string(serverKernel, "SIGNAL")>0){
		if(socket_send_string(serverKernel, identificador_semaforo)>0){

		}
		else{
			log_info(logCPU, "No se pudo enviar el identificador del semaforo para que el kernel le haga signal.");
		}

	}
	if (socket_send_string(serverKernel, identificador_semaforo)>0){
		log_info(logCPU, "Le envio al Kernel el semaforo al que quiero que le haga Signal: %s\n",identificador_semaforo);
	}else{
		log_info(logCPU, "Error enviando al Kernel el semaforo al que quiero que le haga Signal: %s\n",identificador_semaforo);
	}
}

t_puntero AnSISOP_alocar(t_valor_variable espacio){
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
	return punteroFinal;
}

void AnSISOP_liberar(t_puntero puntero){
	//COMO VALIDAMOS QUE PUNTERO HAYA SIDO PREVIAMENTE ALOCADO?
	log_info(logCPU, "Se va a liberar el puntero: %d", puntero);
	if(socket_send_int(serverKernel,puntero)>0){
		log_info(logCPU,"Envio al kernel el puntero que quiero liberar: %d\n",puntero);
	}else{
		log_info(logCPU, "Error enviando al kernel el puntero que quiero liberar: %d\n", puntero);
	}
}
t_descriptor_archivo AnSISOP_abrir (t_direccion_archivo direccion, t_banderas flags){
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
	if (socket_send(serverKernel,&flags,sizeof(flags))>0){
		log_info(logCPU, "Envio correctamente los permisos de apertura del archivo: %s\n",flags);
	}else{
		log_info(logCPU, "Error enviando los permisos de apertura del archivo: %s\n",flags);
	}
	//Recibo el descriptor de archivo
	int descriptor;
	t_descriptor_archivo descriptorPosta;
	if(socket_recv_int(serverKernel,&descriptor)>0){
		descriptorPosta=(t_descriptor_archivo)descriptor;
		log_info(logCPU,"Recibo correctamente el descriptor de archivo: %d\n", descriptorPosta);
	}else{
		log_info(logCPU, "Error recibiendo el descriptor de archivo\n");
	}
	return descriptorPosta;
}
void AnSISOP_borrar (t_descriptor_archivo direccion){
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
}
void AnSISOP_cerrar (t_descriptor_archivo descriptor_archivo){
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
}
void AnSISOP_moverCursor(t_descriptor_archivo descriptor_archivo, t_valor_variable posicion){
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
}
void AnSISOP_escribir(t_descriptor_archivo descriptor_archivo, void* informacion, t_valor_variable tamanio){
	//Informo al kernel que quiero escribir archivo
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
}
void AnSISOP_leer(t_descriptor_archivo descriptor_archivo, t_puntero informacion, t_valor_variable tamanio){
	//Informo al kernel que quiero leer archivo
	if (socket_send_string(serverKernel,"leer")>0){
		log_info(logCPU, "Informo correctamente al kernel que el programa %d quiere leer un archivo\n", pcb->pid);
	}else{
		log_info(logCPU, "Error informando al kernel que el programa %d quiere leer un archivo\n", pcb->pid);
	}

	//Envio al kernel el descriptor de archivo
	if (socket_send_int(serverKernel, descriptor_archivo)>0){
		log_info(logCPU, "Envio correctamente el descriptor de archivo: %d\n", descriptor_archivo);
	}else{
		log_info(logCPU, "Error enviando el descriptor de archivo: %d\n", descriptor_archivo);
	}

	//Envio al kernel el lugar donde quiero que se almacene la informacion leida
	if (socket_send_int(serverKernel,informacion)>0){
		log_info(logCPU, "Envio correctamente el puntero donde quiero se guarde la informacion leida: %d\n", informacion);
	}else{
		log_info(logCPU, "Error enviando el puntero donde quiero se guarde la informacion leida: %d\n", informacion);
	}

	//Envio al kernel el tamanio de lo que quiero leer
	if (socket_send_int(serverKernel, tamanio)>0){
		log_info(logCPU, "Envio correctamente el tamanio de lo que quiero leer: %d\n", tamanio);
	}else{
		log_info(logCPU, "Error enviando el tamanio de lo que quiero leer: %d\n", tamanio);
	}
}
