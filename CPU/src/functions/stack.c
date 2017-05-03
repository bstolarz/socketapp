#include "stack.h"
#include "../commons/structures.h"
#include "../commons/declarations.h"

t_indiceDelStack* stack_context_create()
{
	t_indiceDelStack* nivelStack = malloc(sizeof(t_indiceDelStack));

	nivelStack->vars = dictionary_create();
	nivelStack->retPos = 0;
	nivelStack->retVar.off = 0;
	nivelStack->retVar.page = 0;
	nivelStack->retVar.size = 0;

	return nivelStack;
}

void stack_context_destroy(void* stackContextVoid)
{
	t_indiceDelStack* stackContext = (t_indiceDelStack*) stackContextVoid;

	dictionary_destroy(stackContext->vars);
	// TODO: destruir lo que falta

	free(stackContext);
}

t_indiceDelStack* stack_context_current()
{
	int stackSize = list_size(pcb->indiceDeStack);
	t_indiceDelStack* stackExecContext = (t_indiceDelStack*) list_get(pcb->indiceDeStack, stackSize - 1);
	return stackExecContext;
}

void stack_context_pop()
{
	list_remove_and_destroy_element(pcb->indiceDeStack, list_size(pcb->indiceDeStack) - 1, stack_context_destroy);
}
