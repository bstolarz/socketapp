#ifndef FUNCTIONS_CACHE_H_
#define FUNCTIONS_CACHE_H_

#include "../commons/structures.h"

extern pthread_mutex_t replaceLock;

int cache_init();
t_cache_entry* cache_search(int PID, int page);
t_cache_entry* cache_cache_contents(int PID, int page, char* content); // por ahora fifo trucho, despues lru
void cache_program_end(int PID);

void cache_access_lock();
void cache_access_unlock();

void cache_flush();

void cache_destroy();

#endif /* FUNCTIONS_CACHE_H_ */
