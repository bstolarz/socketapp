#include "../commons/structures.h"
#include "../commons/declarations.h"

#ifndef FUNCTIONS_MEMORY_H_
#define FUNCTIONS_MEMORY_H_

int memory_frame_size();
int memory_init(t_program* program, int cantPaginas);
int memory_write(t_program* program, int page, int offset, void* buffer, int size);

#endif /* FUNCTIONS_MEMORY_H_ */
