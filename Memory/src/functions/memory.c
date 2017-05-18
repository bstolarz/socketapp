#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include "memory.h"
#include "ram.h"
#include "cache.h"
#include "../commons/structures.h"
#include "../commons/declarations.h"

void memory_init()
{
	ram_init();
	cache_init();
}

void program_end(int PID)
{
	ram_program_end(PID);
	cache_program_end(PID);
}

void* memory_read(int PID, int page, int offset, int size)
{
	char* buffer = malloc(sizeof(char) * size);
	char* bufferStart = buffer;
	bool cacheMiss = false;

	// voy copiando en buffer cada pedazo de pagina
	// puede pasar que me pidan una instruccion que empieza en pag 1 y siga en pag 2
	while (size > 0)
	{
		int currentPageSize = (size < (configMemory->frameSize - offset)) ? size : (configMemory->frameSize - offset);
		size -= currentPageSize;

		char* currentPageData;

		// esta en cache?
		t_cache_entry* cacheEntry = NULL;//= cache_search(PID, page);

		if (cacheEntry) // cache hit
		{
			log_info(logMemory, "[cache hit] PID: %d, page: %d", PID, page);
			currentPageData = cacheEntry->content;
		}
		else // cache miss
		{
			cacheMiss = true;
			currentPageData = ram_frame_lookup(PID, page);

			if (currentPageData == NULL)
			{
				// TODO: chequear que esto este bien
				free(buffer);
				buffer = NULL;
				break;
			}
			else
			{
				// cachear
				// cache_cache_contents(PID, page, currentPageData);
			}
		}

		memcpy(bufferStart, currentPageData + offset, currentPageSize);

		bufferStart = bufferStart + currentPageSize;
		offset = 0;
		++page;
	}

	if (cacheMiss)
	{
		log_info(logMemory, "[memory_read] cache miss page: %d, offset: %d, size: %d\nsleeping %d\n", page, offset, size, configMemory->responseDelay);
		//usleep(configMemory->responseDelay /* in ms */ * 1000);
	}

	return buffer;
}

int memory_write(int PID, int page, int offset, int size, void* buffer)
{
	return ram_write(PID, page, offset, size, buffer);
}
