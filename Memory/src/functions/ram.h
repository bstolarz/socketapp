#ifndef FUNCTIONS_RAM_H_
#define FUNCTIONS_RAM_H_

#include "../commons/structures.h"

// 0 si pudo allocar memoria, -1 si no pudo 
int ram_init();

// Operaciones de Memoria (pag 26)
int ram_program_init(int PID, size_t pageCount);
void ram_program_end(int PID);
char* ram_frame_lookup(int PID, size_t page);
int ram_get_pages(int PID, size_t pageCount);
int ram_free_page(int PID, size_t page);

char* get_frame(size_t i); // busca el frame por indice
size_t frame_count(_Bool (*framePredicate)(t_pageTableEntry*));

#endif /* FUNCTIONS_RAM_H_ */
