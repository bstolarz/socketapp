#include "stack.h"
#include "../commons/structures.h"
#include "../commons/declarations.h"

t_indiceDelStack* stack_context_create()
{
	t_indiceDelStack* nivelStack = (t_indiceDelStack*) malloc(sizeof(t_indiceDelStack));
	
	nivelStack->vars = dictionary_create();
	nivelStack->retPos = -1;
	nivelStack->retVar = NULL;
	
	return nivelStack;
}

void stack_context_destroy(void* stackContextVoid)
{
	t_indiceDelStack* stackContext = (t_indiceDelStack*) stackContextVoid;

	dictionary_destroy_and_destroy_elements(stackContext->vars, &free);
	free(stackContext);
}

t_indiceDelStack* stack_context_current()
{
	int stackSize = list_size(pcb->indiceDeStack);
	t_indiceDelStack* stackExecContext = (t_indiceDelStack*) list_get(pcb->indiceDeStack, stackSize - 1);
	return stackExecContext;
}

void stack_pop()
{
	list_remove_and_destroy_element(pcb->indiceDeStack, list_size(pcb->indiceDeStack) - 1, stack_context_destroy);
}

void stack_add_arg(t_nombre_variable identificador_variable)
{
	t_indiceDelStack* currentContext = stack_context_current();

	int index = identificador_variable - '0';
	currentContext->args[index] = puntero_to_position(pcb->stackPosition);

	log_info(logCPU,"[definirVar] arg {%c} en (pag: %d, offset: %d y size: %d)\n",
			identificador_variable,
			currentContext->args[index].page,
			currentContext->args[index].off,
			currentContext->args[index].size);
}

void stack_add_var(t_nombre_variable identificador_variable)
{
	t_indiceDelStack* currentContext = stack_context_current();
	t_position* stackPositionAsPosition = malloc(sizeof(t_position));
	*stackPositionAsPosition = puntero_to_position(pcb->stackPosition);

	dictionary_put(currentContext->vars, string_from_format("%c", identificador_variable), stackPositionAsPosition);

	log_info(logCPU,"[definirVar] var local {%c} en (pag: %d, offset: %d y size: %d)\n",
			identificador_variable, stackPositionAsPosition->page, stackPositionAsPosition->off, stackPositionAsPosition->size);
}

t_position* stack_get_arg(t_nombre_variable identificador_variable)
{
	t_indiceDelStack* currentContext = stack_context_current();

	int index = identificador_variable - '0';
	return &currentContext->args[index];
}

t_position* stack_get_var(t_nombre_variable identificador_variable)
{
	t_indiceDelStack* currentContext = stack_context_current();

	char keyStr[2] = { identificador_variable, '\0' };
	return dictionary_get(currentContext->vars, keyStr);
}

t_puntero position_to_puntero(t_position* memoryPos)
{
	return memoryPos->page * pageSize + memoryPos->off;
}

t_position puntero_to_position(t_puntero direccion_variable)
{
	t_position pos;
	div_t values = div(direccion_variable, pageSize);
	pos.page = values.quot;
	pos.off = values.rem;
	pos.size = sizeof(t_valor_variable);

	return pos;
}
