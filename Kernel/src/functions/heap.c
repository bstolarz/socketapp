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
	//TODO
	return 0;
}

int heap_find_space_available(t_program* program, int size, int* page, int* offset){
	int locatedSpace = 0;

	void _hasFreeSpace(t_heap_page* pageMetadata){
		if(pageMetadata->freeSpace <= size){
			int currentOffset = 0;
			t_heapmetadata* metadata = NULL;
			if(memory_read(program, pageMetadata->page, currentOffset, sizeof(t_heapmetadata), metadata) == sizeof(t_heapmetadata)){
				while(currentOffset < pageSize && locatedSpace==0){
					if(size==metadata->size || (size+sizeof(t_heapmetadata))<=metadata->size){
						*page = pageMetadata->page;
						*offset = currentOffset;
						locatedSpace=1;
					}else{
						currentOffset += metadata->size + sizeof(t_heapmetadata);
					}
				}
			}
		}
	}
	list_iterate(program->heapPages, (void*)_hasFreeSpace);

	return locatedSpace;
}

int heap_alloc(t_program* program, int size, int page, int offset){
	//TODO
	return 0;
}

int heap_free(t_program* program, int page, int offset){
	//TODO
	return 0;
}

int heap_defrag(t_program* program, int page){
	//TODO
	return 0;
}
