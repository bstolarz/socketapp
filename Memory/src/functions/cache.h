#ifndef FUNCTIONS_CACHE_H_
#define FUNCTIONS_CACHE_H_

#include "../commons/structures.h"

int cache_init();
t_cache_entry* cache_search(int PID, int page);
void cache_cache_contents(int PID, int page, char* content); // por ahora fifo trucho, despues lru
void cache_program_end(int PID);

#endif /* FUNCTIONS_CACHE_H_ */
