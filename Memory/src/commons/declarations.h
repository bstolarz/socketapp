#ifndef COMMONS_DECLARATIONS_H_
#define COMMONS_DECLARATIONS_H_

#include "structures.h"
#include <commons/log.h>
#include <pthread.h>

t_memory* configMemory;

t_pageTableEntry* pageTable; // la tabla para buscar frames dado un pid y una página del proceso.
                                    // la primer entry describiría proccessPage[0]
                                    // (no la page en la que esta este array de entradas que sería la verdadera pag 0)

int freeFrameCount;
t_pageTableEntry** freeFramesEntries; 	// t_pageTableEntry*[proccessPageCount]
										// guarda las pageTableEntry disponibles
										// para que cuando hay que buscar vacios el lockeo sea minimo
extern pthread_mutex_t freeFrameMutex;

int proccessPageCount;
char* proccessPages; // la memoria RAM donde guardar y buscar datos
					 // es char* para que se pueda sumar de a 1 byte

t_log* logMemory;

int ERROR_NO_RESOURCES_FOR_PROCCESS;
int ERROR_MEMORY;

t_queue * threadsList;

#endif /* COMMONS_DECLARATIONS_H_ */
