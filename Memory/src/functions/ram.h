#ifndef FUNCTIONS_MEMORY_H_
#define FUNCTIONS_MEMORY_H_

#include "../commons/structures.h"

// 0 si pudo allocar memoria, -1 si no pudo 
int memory_init();

// Operaciones de Memoria (pag 26)
int program_init(int PID, int pageCount);
void program_end(int PID);
void* memory_read(int PID, int page, int offset, int size);
int memory_write(int PID, int page, int offset, int size, void* buffer);

char* get_frame(int i); // busca el frame por indice
int frame_count(_Bool (*framePredicate)(t_pageTableEntry*));

#endif /* FUNCTIONS_MEMORY_H_ */
