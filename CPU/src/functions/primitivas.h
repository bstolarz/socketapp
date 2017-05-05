/*
 * primitivas.h
 *
 *  Created on: 20/4/2017
 *      Author: utnso
 */

#ifndef FUNCTIONS_PRIMITIVAS_H_
#define FUNCTIONS_PRIMITIVAS_H_
//#include "sintax.h"
#include <parser/metadata_program.h>
#include <parser/parser.h>

t_puntero AnSISOP_definirVariable (t_nombre_variable identificador_variable);
t_puntero AnSISOP_obtenerPosicionVariable(t_nombre_variable identificador_variable);
t_valor_variable AnSISOP_dereferenciar(t_puntero direccion_variable);
void AnSISOP_asignar (t_puntero direccion_variable, t_valor_variable valor);
void AnSISOP_irAlLabel (t_nombre_etiqueta t_nombre_etiqueta);
void AnSISOP_finalizar (void);

t_valor_variable AnSISOP_obtenerValorCompartida(t_nombre_compartida variable);
t_valor_variable AnSISOP_asignarValorCompartida(t_nombre_compartida variable, t_valor_variable valor);
void AnSISOP_llamarSinRetorno(t_nombre_etiqueta etiqueta);
void AnSISOP_llamarConRetorno(t_nombre_etiqueta etiqueta, t_puntero donde_retornar);
void AnSISOP_retornar(t_valor_variable retorno);


// kernel
void AnSISOP_imprimirValor(t_valor_variable valor_mostrar);
void AnSISOP_imprimirLiteral(char* texto);

void AnSISOP_wait(t_nombre_semaforo identificador_semaforo);
void AnSISOP_signal(t_nombre_semaforo identificador_semaforo);
t_puntero AnSISOP_alocar(t_valor_variable espacio);
void AnSISOP_liberar(t_puntero puntero);
t_descriptor_archivo AnSISOP_abrir (t_direccion_archivo direccion, t_banderas flags);
void AnSISOP_borrar (t_descriptor_archivo direccion);
void AnSISOP_borrar (t_descriptor_archivo direccion);
void AnSISOP_cerrar (t_descriptor_archivo descriptor_archivo);
void AnSISOP_moverCursor(t_descriptor_archivo descriptor_archivo, t_valor_variable posicion);
void AnSISOP_escribir(t_descriptor_archivo descriptor_archivo, void* informacion, t_valor_variable tamanio);
void AnSISOP_leer(t_descriptor_archivo descriptor_archivo, t_puntero informacion, t_valor_variable tamanio);
#endif /* FUNCTIONS_PRIMITIVAS_H_ */
