#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <math.h>

#include "../commons/declarations.h"
#include "../commons/structures.h"
#include "ram.h"
#include "utils.h"

pthread_mutex_t freeFrameMutex = PTHREAD_MUTEX_INITIALIZER;

int ram_init()
{
	pageTable = malloc(configMemory->frameCount * configMemory->frameSize);
	freeFramesEntries = (t_pageTableEntry**)(pageTable + configMemory->frameCount);

    int i;
    for (i = 0; i < configMemory->frameCount; ++i){
        pageTable[i].PID = -1;
        pageTable[i].page = 0;
        freeFramesEntries[i] = pageTable + i;
    }

    int tableSizeInBytes = sizeof(t_pageTableEntry) * configMemory->frameCount;
    int freeFrameArraySizeInBytes = sizeof(t_pageTableEntry*) * configMemory->frameCount;
    int adminSizeInBytes = tableSizeInBytes + freeFrameArraySizeInBytes;
    int adminSizeInPages = bytes_to_pages(adminSizeInBytes);

    // como agrego bytes al comienzo del bloque de memoria casteo a char
    // (para que no los tome como 4 a cada uno)
    proccessPages = ((char*) pageTable) + (adminSizeInPages * configMemory->frameSize);
    proccessPageCount = configMemory->frameCount - adminSizeInPages;
    freeFrameCount = proccessPageCount;

    return 0;
}


// Operaciones de Memoria (pag 26)
int ram_program_init(int PID, int pageCount)
{
	// puede que el kernel mande pedidos de iniciar proceso al mismo tiempo
	// lockeo aca para que no traten de usar los mismos frames vacios los 2 procesos
	// al tener el pool de los frames vacios el lockeo es mas corto porque solo se toman pageCount frames
	// antes tenia que lockear toda la pageTable o cada entrada 1 por 1.
	pthread_mutex_lock(&freeFrameMutex);

	if (freeFrameCount < pageCount)
	{
		log_error(logMemory, "no obtuvo frames para proceso [%d], cant pags %d", PID, pageCount);
		pthread_mutex_unlock(&freeFrameMutex);
		return ERROR_NO_RESOURCES_FOR_PROCCESS;
	}

	// hay frames para proceso
	freeFrameCount -= pageCount;
	int i;

	for (i = 0; i != pageCount; ++i)
	{
		t_pageTableEntry* freeFrameEntry = freeFramesEntries[freeFrameCount + i];
		freeFrameEntry->PID = PID;
		freeFrameEntry->page = i;
	}

	pthread_mutex_unlock(&freeFrameMutex);

    return 0;
}

void ram_program_end(int PID)
{
	int i;
	int pageCount = 0;

	// si un proceso quiere empezar, mejor que espere que se actualizen los freeFramesEntries
	// porque asi es mas seguro que encuentre frames para empezar
	// por eso hago este lock
	// serviria tambien como de sincronizacion (ademas de exclusion mutua)
	pthread_mutex_lock(&freeFrameMutex);

	// esto no va a pisar ni ser pisado por ningun otro proceso
	for (i = 0; i != proccessPageCount; ++i)
	{
		if (pageTable[i].PID == PID)
		{
			pageTable[i].PID = -1;

			freeFramesEntries[freeFrameCount + pageCount] = pageTable + i;
			++pageCount;
		}
	}

	freeFrameCount += pageCount;

	pthread_mutex_unlock(&freeFrameMutex);
}

// returna una pagina o nulo
char* ram_frame_lookup(int PID, int page)
{
    // TODO: hash function
    int i;

    for (i = 0; i != proccessPageCount; ++i)
        if (pageTable[i].PID == PID && pageTable[i].page == page)
            return get_frame(i); // casteo a puntero de char para que sume de 1 byte
    																		// que sume de 1 byte al puntero
    log_error(logMemory, "[frame_lookup] no encontr frame para proceso [%d] pag %d", PID, page);
    return NULL;
}

// no hace falta lockear porque va a ser 1 sola computadora la que acceda a este frame
int ram_write(int PID, int page, int offset, int size, void* buffer)
{
	int wroteSize = 0;

	while (size > 0)
	{
		int currentPageSize = (size < configMemory->frameSize - offset) ? size : (configMemory->frameSize - offset);
		size -= currentPageSize;

		char* frame = ram_frame_lookup(PID, page);

		if (frame == NULL) return ERROR_MEMORY;

		memcpy(frame + offset, buffer + wroteSize, currentPageSize);

		wroteSize += currentPageSize;
		offset = 0;
		++page;
	}

    return wroteSize;
}

int frame_count(_Bool (*framePredicate)(t_pageTableEntry*))
{
    int i;
    int count = 0;

    // TODO: lockear?
    for (i = 0; i != proccessPageCount; ++i)
        if ((*framePredicate)(pageTable + i))
            ++count;

    return count;
}

char* get_frame(int i)
{
	return proccessPages + (i * configMemory->frameSize);
}
