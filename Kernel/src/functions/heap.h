#include "../commons/structures.h"
#include "../commons/declarations.h"

#ifndef FUNCTIONS_HEAP_H_
#define FUNCTIONS_HEAP_H_

int heap_new_page(t_program* program);
int heap_find_space_available(t_program* program, int size, int* page, int* offset);
int heap_alloc(t_program* program, int size, int page, int offset);
int heap_free(t_program* program, int page, int offset);
void heap_defrag(t_program* program, int page);
int heap_max_page_num(t_program* program);

#endif /* FUNCTIONS_HEAP_H_ */
