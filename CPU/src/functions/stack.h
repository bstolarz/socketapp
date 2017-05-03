#ifndef FUNCTIONS_STACK_H_
#define FUNCTIONS_STACK_H_

#include <parser/parser.h>
#include "../commons/structures.h"

t_indiceDelStack* stack_context_create();
t_indiceDelStack* stack_context_current();
void stack_context_destroy(void*);
void stack_pop();

void stack_add_var(t_nombre_variable varName); // agrega var en contexto actual
void stack_add_arg(t_nombre_variable argName); // igual arg
t_position* stack_get_var(t_nombre_variable); // obtiene var de contexto actual
t_position* stack_get_arg(t_nombre_variable); // igual arg

t_puntero position_to_puntero(t_position*);
t_position puntero_to_position(t_puntero);

#endif /* FUNCTIONS_STACK_H_ */
