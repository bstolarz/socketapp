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
#endif /* FUNCTIONS_PRIMITIVAS_H_ */
