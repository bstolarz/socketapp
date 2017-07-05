#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <stdbool.h>

#include "../libSockets/server.h"
#include "../libSockets/client.h"
#include "../libSockets/recv.h"
#include "../libSockets/send.h"

#include "../commons/structures.h"
#include "../commons/declarations.h"
#include "../commons/error_codes.h"

#include "../interface/memory.h"

#include "heap.h"

int heap_max_page_num(t_program* program)
{
	int max = program->pcb->cantPagsCodigo + configKernel->stack_size - 1;

	void _get_max_page_num(t_heap_page* heapPage){
		if (heapPage->page > max){
			max = heapPage->page;
		}
	}

	list_iterate(program->heapPages, (void*)_get_max_page_num);

	return max;
}

int heap_new_page(t_program* program){
	int resp;
	if((resp=memory_get_pages(program, 1))!= 0){
		program->interruptionCode = resp;
		return resp;
	}

	t_heap_page* pageMetadata = malloc(sizeof(t_heap_page));
	pageMetadata->freeSpace = pageSize - sizeof(t_heapmetadata);
	pageMetadata->page = heap_max_page_num(program) + 1;
	list_add(program->heapPages, pageMetadata);

	t_heapmetadata* metadata = malloc(sizeof(t_heapmetadata)); // crea pagina con tamanio disponible toda la pag (- metadata)
	memset(metadata, 0, sizeof(t_heapmetadata)); // para q se vea mas claro en memoria
	metadata->isFree = true;
	metadata->size = pageSize - sizeof(t_heapmetadata);

	if(memory_write(program, pageMetadata->page, 0, metadata, sizeof(t_heapmetadata)) != sizeof(t_heapmetadata)){
		printf("heap_new_page\n");
		exit(EXIT_FAILURE);
	}

	return pageMetadata->page;
}

// revisa c/pag de heap para ver si tiene espacio
int heap_find_space_available(t_program* program, int size, int* page, int* offset){
	int locatedSpace = 0;

	void _hasFreeSpace(t_heap_page* pageMetadata){
		if(pageMetadata->freeSpace >= size){
			int currentOffset = 0;
			t_heapmetadata* metadata = NULL;
			while(currentOffset < pageSize && locatedSpace==0){
				if(memory_read(program, pageMetadata->page, currentOffset, sizeof(t_heapmetadata), (void**)&metadata) == sizeof(t_heapmetadata)){
					if(metadata->isFree==1 && (size==metadata->size || (size+sizeof(t_heapmetadata))<=metadata->size)){
						*page = pageMetadata->page;
						*offset = currentOffset;
						locatedSpace=1;
					}else{
						currentOffset += metadata->size + sizeof(t_heapmetadata);
					}
				}else{
					exit(EXIT_FAILURE);
				}
			}
		}
	}
	list_iterate(program->heapPages, (void*)_hasFreeSpace);
	return locatedSpace;
}

void heap_defrag(t_program* program, t_heap_page* heapPage){
	int offset = 0;
	t_heapmetadata* currentMetadata = NULL;
	t_heapmetadata* prevMetadata = NULL;

	if(memory_read(program, heapPage->page, offset, sizeof(t_heapmetadata), (void**)&prevMetadata) == sizeof(t_heapmetadata)){
		offset = sizeof(t_heapmetadata) + prevMetadata->size;
		while(offset < pageSize){
			if(memory_read(program, heapPage->page, offset, sizeof(t_heapmetadata), (void**)&currentMetadata) != sizeof(t_heapmetadata)){
				printf("heap_defrag read\n");
				exit(EXIT_FAILURE);
			}

			if(currentMetadata->isFree==1 && prevMetadata->isFree==1){
				offset = offset + sizeof(t_heapmetadata) + currentMetadata->size;
				prevMetadata->size = prevMetadata->size + sizeof(t_heapmetadata) + currentMetadata->size;
				if(memory_write(program, heapPage->page, offset - sizeof(t_heapmetadata) - prevMetadata->size, prevMetadata, sizeof(t_heapmetadata)) != sizeof(t_heapmetadata)){
					printf("heap_defrag write\n");
					exit(EXIT_FAILURE);
				}
				free(currentMetadata);

				heapPage->freeSpace = heapPage->freeSpace + sizeof(t_heapmetadata);
			}else{
				free(prevMetadata);
				prevMetadata=currentMetadata;
				offset = offset + sizeof(t_heapmetadata) + currentMetadata->size;
			}
		}
		// TODO: jonatan fijate si esto previene leaks
		// vi que en el if de mas arriba el q siempre queda no nulo es el prevMetadata
		// antes habia un free(currentMetadata) que rompia porq mas arriba a veces se libera currentMetadata
		// lo probe con heap.ansisop y no crasheaba
		free(prevMetadata);
	}else{
		exit(EXIT_FAILURE);
	}

	printf("Espacio libre: %d\n", heapPage->freeSpace);
	if(heapPage->freeSpace == (pageSize - sizeof(t_heapmetadata))){
		int resp;
		printf("liberamos pagina %d\n", heapPage->page);
		if((resp=memory_free_page(program, heapPage->page))!= 0){
			program->interruptionCode = resp;
		}
		// TODO: borrar la entrada de la lista de heap en program
	}
}

int heap_alloc(t_program* program, int size, int page, int offset){
	t_heapmetadata* metadata = NULL;

	bool _findPage(t_heap_page* hp){
		return hp->page==page;
	}
	t_heap_page* heapPage = list_find(program->heapPages, (void*)_findPage);

	if(memory_read(program, page, offset, sizeof(t_heapmetadata), (void**)&metadata) == sizeof(t_heapmetadata)){
		if(size==metadata->size){
			metadata->isFree = 0;
			if(memory_write(program, page, offset, metadata, sizeof(t_heapmetadata)) != sizeof(t_heapmetadata)){
				printf("heap_alloc: PID:%i, Size:%i, Page:%i, Offset:%i - Tamaño igual\n", program->pcb->pid, size, page, offset);
				exit(EXIT_FAILURE);
			}
			heapPage->freeSpace = heapPage->freeSpace - metadata->size;
		}else if(size < metadata->size){
			t_heapmetadata* newMetadata = malloc(sizeof(t_heapmetadata));
			memset(newMetadata, 0, sizeof(t_heapmetadata)); // para q se vea mas claro en memoria
			newMetadata->isFree = 0;
			newMetadata->size = size;

			if(memory_write(program, page, offset, newMetadata, sizeof(t_heapmetadata)) != sizeof(t_heapmetadata)){
				printf("heap_alloc: PID:%i, Size:%i, Page:%i, Offset:%i - Tamaño menor parte 1\n", program->pcb->pid, size, page, offset);
				exit(EXIT_FAILURE);
			}

			offset +=  sizeof(t_heapmetadata) + size;
			metadata->size = metadata->size - newMetadata->size - sizeof(t_heapmetadata);
			heapPage->freeSpace = heapPage->freeSpace -newMetadata->size - sizeof(t_heapmetadata);
			if(memory_write(program, page, offset, metadata, sizeof(t_heapmetadata)) != sizeof(t_heapmetadata)){
				printf("heap_alloc: PID:%i, Size:%i, Page:%i, Offset:%i - Tamaño menor parte 2\n", program->pcb->pid, size, page, offset);
				exit(EXIT_FAILURE);
			}

			free(newMetadata);
		}else{
			printf("heap_alloc: PID:%i, Size:%i, Page:%i, Offset:%i\n", program->pcb->pid, size, page, offset);
			exit(EXIT_FAILURE);
		}

		free(metadata);
	}

	heap_defrag(program, heapPage);

	return 1;
}

int heap_free(t_program* program, int page, int offset){
	if(offset<sizeof(t_heapmetadata)){
		program->interruptionCode = ERROR_MEMORY;
		return 0;
	}

	t_heapmetadata* metadata = NULL;
	if(memory_read(program, page, offset-sizeof(t_heapmetadata), sizeof(t_heapmetadata), (void**)&metadata) != sizeof(t_heapmetadata)){
		printf("heap_free read\n");
		exit(EXIT_FAILURE);
	}

	metadata->isFree=1;

	if(memory_write(program, page, offset-sizeof(t_heapmetadata), metadata, sizeof(t_heapmetadata)) != sizeof(t_heapmetadata)){
		printf("heap_free write\n");
		exit(EXIT_FAILURE);
	}

	bool _findPage(t_heap_page* hp){
		return hp->page==page;
	}
	t_heap_page* heapPage = list_find(program->heapPages, (void*)_findPage);
	heapPage->freeSpace = heapPage->freeSpace + metadata->size;

	heap_defrag(program, heapPage);

	return 1;
}
