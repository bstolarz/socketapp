#ifndef FUNCTIONS_STACK_H_
#define FUNCTIONS_STACK_H_

#include "../commons/structures.h"

t_indiceDelStack* stack_context_create();
t_indiceDelStack* stack_context_current();
void stack_context_destroy(void*);
void stack_context_pop();
#endif /* FUNCTIONS_STACK_H_ */
