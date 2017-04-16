#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "memory.h"
#include "../commons/declarations.h"
#include "../commons/structures.h"
#include "frame.h"


//extern t_memory* configMemory;
//t_pageTableEntry* pageTable;
//void* proccessPages;
//int proccessPageCount;
int ERROR_NO_RESOURCES_FOR_PROCCESS = -1;
int ERROR_MEMORY = -5;

int memory_init()
{
    void* memoryBits = malloc(configMemory->frameCount * configMemory->frameSize);

    if (memoryBits == NULL)
        return -1;
    
    pageTable = (t_pageTableEntry*) memoryBits;
    int i;
    for (i = 0; i != configMemory->frameCount; ++i)
    {
        pageTable[i].PID = -1;
        pageTable[i].page = 0;
    }
    // las ultimas entradas de la tabla no serían usadas, medio q sobran. por ahí después se me ocurre algo

    // calculo cuántas páginas ocupa tabla para saber donde empiezan las páginas para procesos
    int tableSizeInBytes = sizeof(t_pageTableEntry) * configMemory->frameCount;
    int tableSizeInPages = bytes_to_pages(tableSizeInBytes); // al menos 1 pagina

    proccessPages = memoryBits + (tableSizeInPages * configMemory->frameSize);
    //proccessFirstPage = tableSizeInPages;
    proccessPageCount = configMemory->frameCount - tableSizeInPages;

    return 0;
}


// Operaciones de Memoria (pag 26)
int program_init(int PID, int pageCount)
{
    int* frameIndices = get_non_continguous_frames(pageCount);
    int i;

    if (frameIndices == NULL)
    {
        log_error(logMemory, "no obtuvo frames para proceso [%d], cant pags %d", PID, pageCount);
        return ERROR_NO_RESOURCES_FOR_PROCCESS;
    }
    
    for (i = 0; i != pageCount; ++i)
    {
        t_pageTableEntry* entry = pageTable + frameIndices[i];
        entry->page = i;
        entry->PID = PID;
    }

    free(frameIndices);

    return 0;
}

int* get_continguous_frames(int count)
{
    int begin, end;

    for (begin = 0; begin != proccessPageCount; ++begin)
    {
        for (end = 0; end != count; ++end)
        {
            if (is_frame_occupied(pageTable + begin + end))
                break;
        }

        if (end == count)
        {
            int* frames = (int*) malloc(sizeof(int) * count);
            int i;
            
            if (frames != NULL)
            {
                for (i = 0; i != count; ++i)
                    frames[i] = begin + i;
            }

            return frames;
        }
    }

    return NULL;
}

int* get_non_continguous_frames(int count)
{
    int* frames = (int*) malloc(sizeof(int) * count);
    
    if (frames == NULL)
        return NULL;
    
    int i, j = 0;
    
    for (i = 0; i != proccessPageCount; ++i)
    {
        if (is_frame_free(pageTable + i))
        {
            frames[j++] = i;

            if (j == count)
                return frames; 
        }
    }

    free(frames);
    return NULL;
}

void program_end(int PID)
{
	int i;

	for (i = 0; i != proccessPageCount; ++i)
        if (pageTable[i].PID == PID)
            pageTable[i].PID = -1;
}

// returna una pagina o nulo
void* frame_lookup(int PID, int page)
{
    // TODO: hash function
    int i;

    for (i = 0; i != proccessPageCount; ++i)
        if (pageTable[i].PID == PID && pageTable[i].page == page)
            return proccessPages + (i * configMemory->frameSize);

    log_error(logMemory, "no encontro frame para proceso [%d] pag %d", PID, page);
    return NULL;
}

void* memory_read(int PID, int page, int offset, int size)
{
    void* frame = frame_lookup(PID, page);

    if (frame == NULL) return NULL;

    // hay que hacer copia de esta data?
    // la data puede seguir en otro frame?
    return frame + offset;
}

int memory_write(int PID, int page, int offset, int size, void* buffer)
{
    // hace falta chequear que hay lugar?
    void* frame = frame_lookup(PID, page);
    
    if (frame == NULL) return ERROR_MEMORY;
    
    memcpy(frame + offset, buffer, size);
    
    return 0;
}



// utils
int bytes_to_pages(int byteCount)
{
	return (byteCount / configMemory->frameSize) + 1;
}
