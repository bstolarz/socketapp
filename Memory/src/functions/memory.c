#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "memory.h"
#include "../commons/declarations.h"
#include "../commons/structures.h"
#include "utils.h"

int memory_init()
{
	pageTable = malloc(configMemory->frameCount * configMemory->frameSize);

    int i;
    for (i = 0; i<configMemory->frameCount; ++i){
        pageTable[i].PID = -1;
        pageTable[i].page = 0;
    }

    int tableSizeInBytes = sizeof(t_pageTableEntry) * configMemory->frameCount;
    int tableSizeInPages = bytes_to_pages(tableSizeInBytes);

    proccessPages = pageTable + (tableSizeInPages * configMemory->frameSize);
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

int memory_write(int PID, int page, int offset, int size, void* buffer){
    // hace falta chequear que hay lugar?
    void* frame = frame_lookup(PID, page);

    if (frame == NULL) return ERROR_MEMORY;

    memcpy(frame + offset, buffer, size);

    return 0;
}
