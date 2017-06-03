#ifndef COMMONS_DECLARATIONS_H_
#define COMMONS_DECLARATIONS_H_

#include "structures.h"
#include <commons/log.h>
#include <pthread.h>
#include <stddef.h>

t_memory* configMemory;

t_pageTableEntry* pageTable; // la tabla para buscar frames dado un pid y una página del proceso.
                                    // la primer entry describiría proccessPage[0]
                                    // (no la page en la que esta este array de entradas que sería la verdadera pag 0)

size_t freeFrameCount;
extern pthread_mutex_t freeFrameMutex;

size_t proccessPageCount;
char* proccessPages; // la memoria RAM donde guardar y buscar datos
					 // es char* para que se pueda sumar de a 1 byte

t_cache_entry* cache; // cacheEntryCount entries
char* cacheContent; // cacheEntryCount * frameSize block

t_log* logMemory;

int ERROR_NO_RESOURCES_FOR_PROCCESS;
int ERROR_MEMORY;

t_queue * threadsList;

#endif /* COMMONS_DECLARATIONS_H_ */
