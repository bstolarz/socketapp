#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <commons/collections/list.h>
#include "cache.h"
#include "../commons/structures.h"
#include "../commons/declarations.h"

// locks
int accessCount = 0;
pthread_spinlock_t accessCountLock; // lockea accessCount para que se aumente bien accessCount
pthread_mutex_t replaceLock = PTHREAD_MUTEX_INITIALIZER; // lockea reemplazar una entrada

// para facilitar el reemplazo lru
typedef struct
{
	int PID;
	t_list* entries;
} t_proccessEntries;

t_list* proccessesCacheEntries;
int cacheUseCount;

void proccess_entries_destroy(void* elem)
{
	t_proccessEntries* proccessEntries = (t_proccessEntries*) elem;
	list_destroy(proccessEntries->entries);
	free(proccessEntries);
}




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
		// pthread_spin_init(&cache[i].lock, 0);
	}

	pthread_spin_init(&accessCountLock, 0);

	proccessesCacheEntries = list_create();
	cacheUseCount = 0;

	return 0;
}

void cache_destroy()
{
	list_destroy_and_destroy_elements(proccessesCacheEntries, proccess_entries_destroy);
	free(cacheContent);
	free(cache);
}

void cache_access_lock()
{
	pthread_spin_lock(&accessCountLock);

	++accessCount;

	if (accessCount == 1)
		pthread_mutex_lock(&replaceLock);

	pthread_spin_unlock(&accessCountLock);
}

void cache_access_unlock()
{
	pthread_spin_lock(&accessCountLock);

	--accessCount;
	if (accessCount == 0)
			pthread_mutex_unlock(&replaceLock);

	pthread_spin_unlock(&accessCountLock);
}

t_cache_entry* cache_search(int PID, int page)
{
	int i;

	for (i = 0; i != configMemory->cacheEntryCount; ++i)
	{
		if (cache[i].PID == PID && cache[i].page == page)
		{
			cache[i].lastAccess = clock();
			return &cache[i];
		}
	}

	return NULL;
}


// LRU functions
t_cache_entry* lruProccess(t_proccessEntries* proccessEntries)
{
	time_t lruTime = time(NULL);
	t_cache_entry* lruEntry = NULL;

	void selectLru(void* elem)
	{
		t_cache_entry* entry = (t_cache_entry*) elem;

		if (entry->lastAccess < lruTime || lruEntry == NULL)
		{
			lruEntry = entry;
			lruTime = entry->lastAccess;
		}
	};

	list_iterate(proccessEntries->entries, selectLru);

	return lruEntry;
}

t_cache_entry* unusedEntry()
{
	return cache_search(-1, -1);
}

t_cache_entry* lruAnyProccess()
{
	int proccessMaxUsed = 0;
	time_t lruTime = time(NULL);
	t_cache_entry* lruEntry = NULL;

	void selectLru(void* elem)
	{
		t_proccessEntries* entries = (t_proccessEntries*) elem;
		int entryCount = list_size(entries->entries);

		if (entryCount < 1) return;

		t_cache_entry* lruCurrentProccess = lruProccess(entries);

		if ( entryCount > proccessMaxUsed ||
			(entryCount == proccessMaxUsed && lruCurrentProccess->lastAccess < lruTime))
		{
			//printf("[expropiating]");
			//if (entryCount > proccessMaxUsed) printf("mas entryCont\n");
			//else printf("older\n");

			proccessMaxUsed = entryCount;
			lruTime = lruCurrentProccess->lastAccess;
			lruEntry = lruCurrentProccess;

		}
	};

	list_iterate(proccessesCacheEntries, selectLru);

	return lruEntry;
}

t_cache_entry* lru(t_proccessEntries* proccessEntries)
{
	// ver si tengo la cant max de entradas y reemplazar propia
	if (list_size(proccessEntries->entries) == configMemory->cachePerProccess)
	{
		log_info(logMemory, "[lru (PID = %d)] use from same proccess\n", proccessEntries->PID);
		return lruProccess(proccessEntries);
	}
	// chequear si hay alguna vacia y usar alguna
	else if (cacheUseCount < configMemory->cacheEntryCount)
	{
		log_info(logMemory, "[lru (PID = %d)] use unused\n", proccessEntries->PID);
		return unusedEntry();
	}
	else
	{
		// buscar el que mas tiene y mas antigua sea
		log_info(logMemory, "[lru (PID = %d)] expropiated other proccess\n", proccessEntries->PID);
		return lruAnyProccess();
	}
}

// lo lockeo desde el llamador de esta fun (memory.c)
t_cache_entry* cache_cache_contents(int PID, int page, char* content)
{
	bool hasPID(void* elem)
	{
		return ((t_proccessEntries*) elem)->PID == PID;
	};

	// inicializar lista de entradas del proceso
	t_proccessEntries* proccessEntries = list_find(proccessesCacheEntries, hasPID);

	if (proccessEntries == NULL)
	{
		proccessEntries = malloc(sizeof(t_proccessEntries));
		proccessEntries->entries = list_create();
		proccessEntries->PID = PID;
		list_add(proccessesCacheEntries, proccessEntries);
	}

	t_cache_entry* cacheEntry = lru(proccessEntries);

	if (cacheEntry->PID == -1) // new cache entry, increment useCount and add to list
	{
		++cacheUseCount;
		list_add(proccessEntries->entries, cacheEntry);
		log_info(logMemory, "[lru (PID = %d)] unused] entries: %d, cacheUseCount = %d\n", PID, list_size(proccessEntries->entries), cacheUseCount);
	}
	else if (cacheEntry->PID != PID)
	{
		int expropiatedProccessPID = cacheEntry->PID;

		bool hasExpropiatedProccessPID(void* elem)
		{
			return ((t_proccessEntries*) elem)->PID == expropiatedProccessPID;
		};

		bool sameCacheEntry(void* elem)
		{
			return ((t_cache_entry*) elem) == cacheEntry;
		};

		// remover del proceso del que la saco
		t_proccessEntries* proccessEntriesExpropiated = list_find(proccessesCacheEntries, hasExpropiatedProccessPID);
		list_remove_by_condition(proccessEntriesExpropiated->entries, sameCacheEntry);

		// agregar al nuevo proceso
		list_add(proccessEntries->entries, cacheEntry);

		log_info(logMemory, "[lru (PID = %d)] expropiated %d] entries: %d, otherEntries = %d, cacheUseCount = %d\n",
				PID, proccessEntriesExpropiated->PID, list_size(proccessEntries->entries), list_size(proccessEntriesExpropiated->entries), cacheUseCount);
	}
	else
	{
		log_info(logMemory, "[lru (PID = %d)] itself] entries: %d, cacheUseCount = %d\n", PID, list_size(proccessEntries->entries), cacheUseCount);
	}

	cacheEntry->PID = PID;
	cacheEntry->page = page;
	cacheEntry->lastAccess = clock();

	memcpy(cacheEntry->content, content, configMemory->frameSize);

	return cacheEntry;
}

void cache_program_end(int PID)
{
	int i;

	pthread_mutex_lock(&replaceLock);

	for (i = 0; i != configMemory->cacheEntryCount; ++i)
	{
		if (cache[i].PID == PID)
		{
			cache[i].PID = -1;
			cache[i].page = -1;
			--cacheUseCount;
		}
	}

	bool hasPID(void* elem)
	{
		return ((t_proccessEntries*) elem)->PID == PID;
	};

	list_remove_and_destroy_by_condition(proccessesCacheEntries, hasPID, proccess_entries_destroy);

	log_info(logMemory, "[cache (PID = %d)] program end] cacheUseCount = %d\n", PID, cacheUseCount);

	pthread_mutex_unlock(&replaceLock);

	assert(cacheUseCount >= 0);
}

void cache_flush()
{
	int i;

	pthread_mutex_lock(&replaceLock);

	for (i = 0; i != configMemory->cacheEntryCount; ++i)
	{
		cache[i].PID = -1;
		cache[i].page = -1;
		cache[i].lastAccess = 0;
		memset(cache[i].content, 0, configMemory->frameSize);
	}

	list_destroy_and_destroy_elements(proccessesCacheEntries, proccess_entries_destroy);
	proccessesCacheEntries = list_create();
	cacheUseCount = 0;

	pthread_mutex_unlock(&replaceLock);
}
