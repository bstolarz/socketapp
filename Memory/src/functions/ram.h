#ifndef FUNCTIONS_RAM_H_
#define FUNCTIONS_RAM_H_

#include "../commons/structures.h"

// 0 si pudo allocar memoria, -1 si no pudo 
int ram_init();

// Operaciones de Memoria (pag 26)
int ram_program_init(int PID, int pageCount);
void ram_program_end(int PID);
char* ram_frame_lookup(int PID, int page);

char* get_frame(int i); // busca el frame por indice
int frame_count(_Bool (*framePredicate)(t_pageTableEntry*));

#endif /* FUNCTIONS_RAM_H_ */
