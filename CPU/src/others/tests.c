#include <stdio.h>
#include <parser/metadata_program.h>
#include <commons/collections/list.h>
#include "../functions/stack.h"
#include "../commons/declarations.h"
#include "tests.h"

void pcb_to_test_primitives()
{
	pcb = malloc(sizeof(t_pcb));

	pcb->pid = 0;
	pcb->pc = 0;
	pcb->cantPagsCodigo = 1;
	pcb->stackPosition = 0;
	pcb->maxStackPosition = pageSize;
	pcb->exitCode = 0;
	pcb->indiceDeStack = list_create();
	list_add(pcb->indiceDeStack, stack_context_create());

	char* code = 0;
	FILE *f = fopen("../programas-ejemplo/facil.ansisop", "rb");
	int fileSize;

	if (f)
	{
		fseek (f, 0, SEEK_END);

		fileSize = ftell (f);
		fseek (f, 0, SEEK_SET);
		code = malloc(fileSize);

		if (code){
			fread (code, 1, fileSize, f);
		}

		fclose (f);

		printf("codigo leido: %s\n", code);
	}
	else
	{
		printf("lei mal el archivo!");
		exit(EXIT_FAILURE);
	}

	t_metadata_program* metadata = metadata_desde_literal(code);

    pcb->cantPagsCodigo = 1; //fileSize / pageSize;
	pcb->indiceDeCodigoCant = metadata->instrucciones_size;
	pcb->indiceDeCodigo = metadata->instrucciones_serializado;
	pcb->indiceDeEtiquetasCant = metadata->etiquetas_size;
	pcb->indiceDeEtiquetas = metadata->etiquetas;

	free(code);
}
