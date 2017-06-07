#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
#include <assert.h>

#include "../commons/declarations.h"
#include "../commons/structures.h"
#include "ram.h"
#include "hash_function.h"
#include "utils.h"
#include "frame.h"


pthread_mutex_t freeFrameMutex = PTHREAD_MUTEX_INITIALIZER;
hash_function hash = ayudante_hash;

int ram_init()
{
	pageTable = malloc(configMemory->frameCount * configMemory->frameSize);

    size_t i;
    for (i = 0; i < configMemory->frameCount; ++i){
        pageTable[i].PID = -1;
        pageTable[i].page = -1;
        pthread_spin_init(&pageTable[i].lock, 0);
    }

    size_t tableSizeInBytes = sizeof(t_pageTableEntry) * configMemory->frameCount;
    size_t adminSizeInBytes = tableSizeInBytes;
    size_t adminSizeInPages = bytes_to_pages(adminSizeInBytes);

    // como agrego bytes al comienzo del bloque de memoria casteo a char
    // (para que no los tome como 4 a cada uno)
    proccessPages = ((char*) pageTable) + (adminSizeInPages * configMemory->frameSize);
    proccessPageCount = configMemory->frameCount - adminSizeInPages;
    freeFrameCount = proccessPageCount;

    return 0;
}

void ram_destroy()
{
	free((void*) pageTable);
}

int insert_proccess_page(int PID, int page)
{
	size_t frameIndex = hash(PID, page) % proccessPageCount;
	size_t collisionCount = 0;

	pthread_spin_lock(&pageTable[frameIndex].lock);

	while (!is_frame_free(&pageTable[frameIndex]))
	{
		pthread_spin_unlock(&pageTable[frameIndex].lock);

		frameIndex = (frameIndex + 1) % proccessPageCount;
		++collisionCount;

		pthread_spin_lock(&pageTable[frameIndex].lock);
	}

	log_info(logMemory, "[insert] PID: %d, page: %d, hash: %zd, frame = %zd, collisionCount: %d\n", PID, page, hash(PID, page) % proccessPageCount, frameIndex, collisionCount);

	pageTable[frameIndex].PID = PID;
	pageTable[frameIndex].page = page;

	pthread_spin_unlock(&pageTable[frameIndex].lock);

	return frameIndex;
}


int ram_get_pages_for_proccess(int PID, size_t pageCount, size_t startPage)
{
	// puede que el kernel mande pedidos de iniciar proceso al mismo tiempo
	// lockeo aca para que no traten de usar los mismos frames vacios los 2 procesos
	// al tener el pool de los frames vacios el lockeo es mas corto porque solo se toman pageCount frames
	// antes tenia que lockear toda la pageTable o cada entrada 1 por 1.
	pthread_mutex_lock(&freeFrameMutex);

	if (freeFrameCount < pageCount)
	{
		pthread_mutex_unlock(&freeFrameMutex);
		log_error(logMemory, "no obtuvo frames para proceso [%d], cant pags %d", PID, pageCount);

		return ERROR_NO_RESOURCES_FOR_PROCCESS;
	}

	// hay frames para proceso
	freeFrameCount -= pageCount;
	pthread_mutex_unlock(&freeFrameMutex);

	size_t i;

	for (i = 0; i != pageCount; ++i)
	{
		insert_proccess_page(PID, startPage + i);
	}


	return 0;
}


// Operaciones de Memoria (pag 26)
int ram_program_init(int PID, size_t pageCount)
{
	return ram_get_pages_for_proccess(PID, pageCount, 0);
}

size_t proccess_page_max(int PID)
{
    size_t i;
    size_t pageMax = 0;

    for (i = 0; i != proccessPageCount; ++i)
    {
		if (pageTable[i].PID == PID &&
			pageTable[i].page > pageMax)
        {
			pageMax = pageTable[i].page;
        }
    }

    return pageMax;
}

int ram_get_pages(int PID, size_t pageCount)
{
	size_t pageMax = proccess_page_max(PID);

	return ram_get_pages_for_proccess(PID, pageCount, pageMax + 1);
}

void ram_program_end(int PID)
{
	size_t i;
	size_t pageCount = 0;

	// si un proceso quiere empezar, mejor que espere que se actualizen los freeFramesEntries
	// porque asi es mas seguro que encuentre frames para empezar
	// por eso hago este lock
	// serviria tambien como de sincronizacion (ademas de exclusion mutua)
	pthread_mutex_lock(&freeFrameMutex);

	// esto no va a pisar ni ser pisado por ningun otro proceso
	for (i = 0; i != proccessPageCount; ++i)
	{
		pthread_spin_lock(&pageTable[i].lock);// hacer esperar para que tengas mas chances de encontrar vacia

		if (pageTable[i].PID == PID)
		{
			pageTable[i].page = -1;
			pageTable[i].PID = -1;

			++pageCount;
		}

		pthread_spin_unlock(&pageTable[i].lock);
	}

	freeFrameCount += pageCount;

	pthread_mutex_unlock(&freeFrameMutex);
}

size_t ram_frame_index_lookup(int PID, int page, int* result)
{
	size_t frameIndex = hash(PID, page) % proccessPageCount;
	size_t collisionCount = 0;

	while (!entry_has_PID_page(&pageTable[frameIndex], PID, page) &&
			collisionCount < proccessPageCount)
	{
		frameIndex = (frameIndex + 1) % proccessPageCount;

		++collisionCount;
	}

	if (collisionCount < proccessPageCount)
	{
		*result = 0;
		return frameIndex;
	}
	else
	{
		// solo por las dudas. sacar cuando este estable
		size_t i;
		for (i = 0; i != proccessPageCount; ++i)
		{
			assert(	pageTable[i].PID != PID || pageTable[i].page != page);
		}

		*result = -1;
		log_error(logMemory, "[frame_lookup] no encontr frame para proceso [%d] pag %d", PID, page);
		return proccessPageCount;
	}
}

// returna una pagina o nulo
char* ram_frame_lookup(int PID, size_t page)
{
	int result;
	size_t frameIndex = ram_frame_index_lookup(PID, page, &result);

	if (result == 0)
		return get_frame(frameIndex);
	else
		return NULL;																	// que sume de 1 byte al puntero
}

int ram_free_page(int PID, size_t page)
{
	int result;
	size_t frameIndex = ram_frame_index_lookup(PID, page, &result);

	if (result == 0)
	{
		pthread_spin_lock(&pageTable[frameIndex].lock);// hacer esperar para que tengas mas chances de encontrar vacia
		pageTable[frameIndex].page = -1;
		pageTable[frameIndex].PID = -1;
		pthread_spin_unlock(&pageTable[frameIndex].lock);// hacer esperar para que tengas mas chances de encontrar vacia

		pthread_mutex_lock(&freeFrameMutex);
		++freeFrameCount;
		pthread_mutex_unlock(&freeFrameMutex);
	}

	return result;
}

size_t frame_count(_Bool (*framePredicate)(t_pageTableEntry*))
{
    size_t i;
    size_t count = 0;

    for (i = 0; i != proccessPageCount; ++i)
    {
		// TODO: lockear?
        if ((*framePredicate)(pageTable + i))
        {
        	++count;
        }
    }

    return count;
}

char* get_frame(size_t i)
{
	return proccessPages + (i * configMemory->frameSize);
}
