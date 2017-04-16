#ifndef COMMONS_DECLARATIONS_H_
#define COMMONS_DECLARATIONS_H_

#include "structures.h"
#include <commons/log.h>

/*extern*?*/ t_memory* configMemory;

/*extern*?*/ t_pageTableEntry* pageTable; // la tabla para buscar frames dado un pid y una página del proceso.
                                    // la primer entry describiría proccessPage[0]
                                    // (no la page en la que esta este array de entradas que sería la verdadera pag 0)
/*extern*?*/ void* proccessPages; // la memoria RAM donde guardar y buscar datos
/*extern*?*/ int proccessPageCount;
t_log* memoryLog;

#endif /* COMMONS_DECLARATIONS_H_ */
