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
#include <parser/metadata_program.h>
#include "../libSockets/client.h"
#include "../libSockets/send.h"
#include "../libSockets/recv.h"
#include "../libSockets/server.h"

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
void AnSISOP_imprimirValor(t_valor_variable valor_mostrar);
void AnSISOP_imprimirLiteral(char* texto);
void AnSISOP_entradaSalida(t_nombre_dispositivo dispositivo, int tiempo);


void AnSISOP_wait(t_nombre_semaforo identificador_semaforo);
void AnSISOP_signal(t_nombre_semaforo identificador_semaforo);
t_puntero AnSISOP_alocar(t_valor_variable espacio);
void AnSISOP_liberar(t_puntero puntero);

#endif /* FUNCTIONS_PRIMITIVAS_H_ */
