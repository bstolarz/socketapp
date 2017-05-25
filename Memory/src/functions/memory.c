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

		// esta en cache?
		cache_access_lock();
		t_cache_entry* cacheEntry = cache_search(PID, page);

		if (cacheEntry) // cache hit
		{
			log_info(logMemory, "[cache hit] PID: %d, page: %d", PID, page);
			memcpy(bufferStart, cacheEntry->content + offset, currentPageSize);
			cache_access_unlock();
		}
		else // cache miss
		{
			cache_access_unlock();
			cacheMiss = true;
			char* ramFrame = ram_frame_lookup(PID, page);

			if (ramFrame == NULL)
			{
				// TODO: chequear que esto este bien
				free(buffer);
				buffer = NULL;
				break;
			}
			else
			{
				// cachear
				pthread_mutex_lock(&replaceLock);
				cache_cache_contents(PID, page, ramFrame);
				pthread_mutex_unlock(&replaceLock);

				// copiar desde ram lo que haya
				memcpy(bufferStart, ramFrame + offset, currentPageSize);
			}
		}

		bufferStart = bufferStart + currentPageSize;
		offset = 0;
		++page;
	}

	if (cacheMiss)
	{
		log_info(logMemory, "[memory_read] cache miss page: %d, offset: %d, size: %d\nsleeping %d\n", page, offset, size, configMemory->responseDelay);
		// printf("[cacheMiss pid = %d] before sleep...\n", PID);
		usleep(configMemory->responseDelay /* in ms */ * 1000 /* in microsecs */);
		// printf("cacheMiss pid = %d] ...after sleep\n", PID);
	}

	return buffer;
}

int memory_write(int PID, int page, int offset, int size, void* buffer)
{
	int wroteSize = 0;
	bool cacheMiss = false;

	while (size > 0)
	{
		int currentPageSize = (size < configMemory->frameSize - offset) ? size : (configMemory->frameSize - offset);
		size -= currentPageSize;

		// primero fijarse si existe el frame
		char* frame = ram_frame_lookup(PID, page);

		if (frame == NULL) return ERROR_MEMORY;

		// traer a cache si no esta
		cache_access_lock();
		t_cache_entry* cacheEntry = cache_search(PID, page);

		if (cacheEntry)
		{
			// actualizar cache
			memcpy(cacheEntry->content + offset, buffer + wroteSize, currentPageSize);
			cache_access_unlock();
		}
		else
		{
			cacheMiss = true;
			cache_access_unlock(); // dejar el lock de acceso porq necesito reemplazar o buscar

			pthread_mutex_lock(&replaceLock);
			cacheEntry = cache_cache_contents(PID, page, frame); // cachear, con el contenido del frame ya actualizado
			memcpy(cacheEntry->content + offset, buffer + wroteSize, currentPageSize);
			pthread_mutex_unlock(&replaceLock);
		}

		// actualizacion inmediata de ram
		memcpy(frame + offset, buffer + wroteSize, currentPageSize);

		wroteSize += currentPageSize;
		offset = 0;
		++page;
	}

	if (cacheMiss)
	{
		log_info(logMemory, "[memory_write] cache miss page: %d, offset: %d, size: %d\nsleeping %d\n", page, offset, size, configMemory->responseDelay);
		// printf("[cacheMiss pid = %d] before sleep...\n", PID);
		usleep(configMemory->responseDelay /* in ms */ * 1000 /* in microsecs */);
		// printf("cacheMiss pid = %d] ...after sleep\n", PID);
	}

	return wroteSize;
}
