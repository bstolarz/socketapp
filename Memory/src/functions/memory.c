#include <stddef.h>
#include <stdlib.h>
#include "memory.h"
#include "../commons/declarations.h"
#include "../commons/structures.h"


//extern t_memory* configMemory;
//t_pageTableEntry* pageTable;
//void* proccessPages;
//int proccessPageCount;

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

int* get_continguous_frames(int count)
{
    int begin, end;

    for (begin = 0; begin != proccessPageCount; ++begin)
    {
        for (end = 0; end != count; ++end)
        {
            if (pageTable[begin + end].PID != -1)
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
        if (pageTable[i].PID == -1)
        {
            frames[j++] = i;

            if (j == count)
                return frames; 
        }
    }

    free(frames);
    return NULL;
}

int bytes_to_pages(int byteCount)
{
	return (byteCount / configMemory->frameSize) + 1;
}
