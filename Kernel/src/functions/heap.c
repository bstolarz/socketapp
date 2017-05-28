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

#include "../interface/memory.h"

int heap_new_page(t_program* program){
	int resp;
	if((resp=memory_get_pages(program, 1))!= 0){
		program->interruptionCode = resp;
		return 0;
	}

	t_heap_page* pageMetadata = malloc(sizeof(t_heap_page));
	pageMetadata->freeSpace = pageSize - sizeof(t_heapmetadata);
	pageMetadata->page = program->pcb->cantPagsCodigo + configKernel->stack_size + list_size(program->heapPages);
	list_add(program->heapPages, pageMetadata);

	t_heapmetadata* metadata = malloc(sizeof(t_heapmetadata));
	metadata->isFree = true;
	metadata->size = pageSize - sizeof(t_heapmetadata);
	if(memory_write(program, pageMetadata->page, 0, metadata, sizeof(t_heapmetadata)) != sizeof(t_heapmetadata)){
		printf("heap_new_page\n");
		exit(EXIT_FAILURE);
	}

	return pageMetadata->page;
}

int heap_find_space_available(t_program* program, int size, int* page, int* offset){
	int locatedSpace = 0;

	void _hasFreeSpace(t_heap_page* pageMetadata){
		if(pageMetadata->freeSpace <= size){
			int currentOffset = 0;
			t_heapmetadata* metadata = NULL;
			while(currentOffset < pageSize && locatedSpace==0){
				if(memory_read(program, pageMetadata->page, currentOffset, sizeof(t_heapmetadata), metadata) == sizeof(t_heapmetadata)){
					if(size==metadata->size || (size+sizeof(t_heapmetadata))<=metadata->size){
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

void heap_defrag(t_program* program, int page){
	int offset = 0;
	t_heapmetadata* currentMetadata = NULL;
	t_heapmetadata* prevMetadata = NULL;

	if(memory_read(program, page, offset, sizeof(t_heapmetadata), prevMetadata) == sizeof(t_heapmetadata)){
		offset = sizeof(t_heapmetadata) + prevMetadata->size;
		while(offset < pageSize){
			if(memory_read(program, page, offset, sizeof(t_heapmetadata), currentMetadata) != sizeof(t_heapmetadata)){
				printf("heap_defrag read\n");
				exit(EXIT_FAILURE);
			}

			if(currentMetadata->isFree==1 && prevMetadata->isFree==1){
				offset = offset + sizeof(t_heapmetadata) + currentMetadata->size;
				prevMetadata->size = prevMetadata->size + sizeof(t_heapmetadata) + currentMetadata->size;
				if(memory_write(program, page, offset - sizeof(t_heapmetadata) - prevMetadata->size, prevMetadata, sizeof(t_heapmetadata)) != sizeof(t_heapmetadata)){
					printf("heap_defrag write\n");
					exit(EXIT_FAILURE);
				}
				free(currentMetadata);
			}else{
				free(prevMetadata);
				prevMetadata=currentMetadata;
				offset = offset + sizeof(t_heapmetadata) + currentMetadata->size;
			}
		}
		free(currentMetadata);
	}else{
		exit(EXIT_FAILURE);
	}
}

int heap_alloc(t_program* program, int size, int page, int offset){
	t_heapmetadata* metadata = NULL;
	if(memory_read(program, page, offset, sizeof(t_heapmetadata), metadata) == sizeof(t_heapmetadata)){
		if(size==metadata->size){
			metadata->isFree = 0;
			if(memory_write(program, page, offset, metadata, sizeof(t_heapmetadata)) != sizeof(t_heapmetadata)){
				printf("heap_alloc: PID:%i, Size:%i, Page:%i, Offset:%i - Tamaño igual\n", program->pcb->pid, size, page, offset);
				exit(EXIT_FAILURE);
			}
		}else if(size < metadata->size){
			t_heapmetadata* newMetadata = malloc(sizeof(t_heapmetadata));
			newMetadata->isFree = 0;
			newMetadata->size = size;
			if(memory_write(program, page, offset, newMetadata, sizeof(t_heapmetadata)) != sizeof(t_heapmetadata)){
				printf("heap_alloc: PID:%i, Size:%i, Page:%i, Offset:%i - Tamaño menor parte 1\n", program->pcb->pid, size, page, offset);
				exit(EXIT_FAILURE);
			}

			offset +=  sizeof(t_heapmetadata) + size;
			metadata->size -= newMetadata->size - sizeof(t_heapmetadata);
			if(memory_write(program, page, offset, metadata, sizeof(t_heapmetadata)) != sizeof(t_heapmetadata)){
				printf("heap_alloc: PID:%i, Size:%i, Page:%i, Offset:%i - Tamaño menor parte 2\n", program->pcb->pid, size, page, offset);
				exit(EXIT_FAILURE);
			}
		}else{
			printf("heap_alloc: PID:%i, Size:%i, Page:%i, Offset:%i\n", program->pcb->pid, size, page, offset);
			exit(EXIT_FAILURE);
		}
	}

	heap_defrag(program, page);

	return 1;
}

int heap_free(t_program* program, int page, int offset){
	if(offset<sizeof(t_heapmetadata)){
		program->interruptionCode = -5;
		return 0;
	}

	t_heapmetadata* metadata = NULL;
	if(memory_read(program, page, offset-sizeof(t_heapmetadata), sizeof(t_heapmetadata), metadata) != sizeof(t_heapmetadata)){
		printf("heap_free read\n");
		exit(EXIT_FAILURE);
	}

	metadata->isFree=1;

	if(memory_write(program, page, offset-sizeof(t_heapmetadata), metadata, sizeof(t_heapmetadata)) != sizeof(t_heapmetadata)){
		printf("heap_free write\n");
		exit(EXIT_FAILURE);
	}

	heap_defrag(program, page);

	return 1;
}
