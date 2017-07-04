#include "../commons/structures.h"
#include "../commons/declarations.h"

#ifndef FUNCTIONS_MEMORY_H_
#define FUNCTIONS_MEMORY_H_

void memory_connect();
int memory_frame_size();
int memory_init(t_program* program, int cantPaginas);
int memory_get_pages(t_program* program, int cantPaginas);
int memory_free_page(t_program* program, int page);
int memory_read(t_program* program, int page, int offset, int size, void** buffer);
int memory_write(t_program* program, int page, int offset, void* buffer, int size);
int memory_heap_alloc(t_program* program, int size);
void memory_heap_free(t_program* program, int page, int offset);
void memory_end_program(t_program* program); // libera paginas en Memoria

#endif /* FUNCTIONS_MEMORY_H_ */
