#include "stack.h"
#include "../commons/structures.h"
#include "../commons/declarations.h"

t_indiceDelStack* stack_context_create()
{
	t_indiceDelStack* nivelStack = (t_indiceDelStack*) malloc(sizeof(t_indiceDelStack));
	
	nivelStack->vars = dictionary_create();
	nivelStack->argCount = 0;
	nivelStack->retPos = -1;
	nivelStack->retVar = NULL;
	nivelStack->argCount=0;
	
	return nivelStack;
}

void stack_context_destroy(void* stackContextVoid)
{
	t_indiceDelStack* stackContext = (t_indiceDelStack*) stackContextVoid;

	dictionary_destroy_and_destroy_elements(stackContext->vars, &free);
	if (stackContext->retVar != NULL) free(stackContext->retVar);
	int i;
	for (i=0;i!=9;i++){
	stackContext->args[i].off=0;
	stackContext->args[i].page=0;
	stackContext->args[i].size=0;
	}
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
	t_indiceDelStack* indiceToDelete=list_get(pcb->indiceDeStack, list_size(pcb->indiceDeStack) - 1);
	
	int varsUsed=dictionary_size(indiceToDelete->vars);
	//Reduzco el stackPosition en la cantidad de argumentos y de variables locales multiplicadas por su tamanio
	log_info(logCPU, "[stack_pop] stackPos estaba en %d", pcb->stackPosition);
	pcb->stackPosition-=(varsUsed + indiceToDelete->argCount) * sizeof(t_valor_variable);
	log_info(logCPU, "y paso a %d\n", pcb->stackPosition);

	list_remove_and_destroy_element(pcb->indiceDeStack, list_size(pcb->indiceDeStack) - 1, stack_context_destroy);
}

void stack_add_arg(t_nombre_variable identificador_variable)
{
	t_indiceDelStack* currentContext = stack_context_current();

	int index = identificador_variable - '0';
	currentContext->args[index] = puntero_to_position(pcb->stackPosition);
	currentContext->args[index].page += pcb->cantPagsCodigo; // agrego offset del stack (en pags) para no agregarlo en dereferenciar ni asignar
	++(currentContext->argCount);

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
	stackPositionAsPosition->page += pcb->cantPagsCodigo; // agrego offset del stack (en pags) para no agregarlo en dereferenciar ni asignar

	dictionary_put(currentContext->vars, string_from_format("%c", identificador_variable), stackPositionAsPosition);

	log_info(logCPU,"[definirVar] var local {%c} en (pag: %d, offset: %d y size: %d)\n",
			identificador_variable, stackPositionAsPosition->page, stackPositionAsPosition->off, stackPositionAsPosition->size);
}

t_position* stack_get_arg(t_nombre_variable identificador_variable)
{
	t_indiceDelStack* currentContext = stack_context_current();
	currentContext->argCount++;
	int index = identificador_variable - '0';
	return &currentContext->args[index];
}
int is_local_from_function(t_nombre_variable id){
	t_indiceDelStack* context=stack_context_current();
	char* id_aux=string_from_format("%c",id);
	int answer= dictionary_has_key(context->vars,id_aux);
	free(id_aux);
	return answer;
}
t_indiceDelStack* stack_get_first_context(){
	return (t_indiceDelStack*)list_get(pcb->indiceDeStack,0);
}
t_position* stack_get_var(t_nombre_variable identificador_variable)
{
	t_position* p;
	char* id=string_from_format("%c",identificador_variable);
	if (is_local_from_function(identificador_variable)){
		log_info(logCPU, "La variable es local de la funcion");
		t_indiceDelStack* currentContext = stack_context_current();
		//	char keyStr[2] = { identificador_variable, '\0' };
		p= (t_position*) dictionary_get(currentContext->vars, id);
		log_info(logCPU, "%d|%d",p->page,p->off);
		free(id);
	}else{
		log_info(logCPU,"La variable pertenece al begin principal y es %c",identificador_variable);
		t_indiceDelStack* firstContext= stack_get_first_context();
		p= (t_position*)dictionary_get(firstContext->vars,id);
		log_info(logCPU,"%d|%d", p->page, p->off);
		free(id);
	}
	return p;
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
