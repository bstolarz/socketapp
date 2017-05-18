#include <string.h>
#include <stdlib.h>
#include "cache.h"
#include "../commons/structures.h"
#include "../commons/declarations.h"

int cache_init()
{
	int i;
	cacheContent = malloc(configMemory->cacheEntryCount * configMemory->frameSize);
	cache = malloc(configMemory->cacheEntryCount * sizeof(t_cache_entry));

	for (i = 0; i != configMemory->cacheEntryCount; ++i)
	{
		cache[i].PID = -1;
		cache[i].page = -1;
		cache[i].content = cacheContent + (i * configMemory->frameSize);
	}

	return 0;
}

t_cache_entry* cache_search(int PID, int page)
{
	int i;

	for (i = 0; i != configMemory->cacheEntryCount; ++i)
	{
		if (cache[i].PID == PID && cache[i].page == page)
		{
			return &cache[i];
		}
	}

	return NULL;
}

int fifoReplaceIndex = 0;

void cache_cache_contents(int PID, int page, char* content)
{
	cache[fifoReplaceIndex].PID = PID;
	cache[fifoReplaceIndex].page = page;
	memcpy(cache[fifoReplaceIndex].content, content, configMemory->frameSize);

	fifoReplaceIndex = (fifoReplaceIndex + 1) % configMemory->cacheEntryCount;
}

void cache_program_end(int PID)
{
	int i;

	for (i = 0; i != configMemory->cacheEntryCount; ++i)
	{
		if (cache[i].PID == PID)
		{
			cache[i].PID = -1;
			cache[i].page = -1;
		}
	}
}
