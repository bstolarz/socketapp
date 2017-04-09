#ifndef FUNCTIONS_MEMORY_H_
#define FUNCTIONS_MEMORY_H_

// 0 si pudo allocar memoria, -1 si no pudo 
int memory_init();

// retorna array[count] de paginas
int* get_continguous_frames(int count);
int* get_non_continguous_frames(int count);

// utils
int bytes_to_pages(int byteCount);

#endif /* FUNCTIONS_MEMORY_H_ */
