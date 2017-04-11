#ifndef FUNCTIONS_MEMORY_H_
#define FUNCTIONS_MEMORY_H_

// 0 si pudo allocar memoria, -1 si no pudo 
int memory_init();

// Operaciones de Memoria (pag 26)
int program_init(int PID, int pageCount);
void program_end(int PID);
void* memory_read(int PID, int page, int offset, int size);
int memory_write(int PID, int page, int offset, int size, void* buffer);

// retorna array[count] de paginas
int* get_continguous_frames(int count);
int* get_non_continguous_frames(int count);

// utils
int bytes_to_pages(int byteCount);


#endif /* FUNCTIONS_MEMORY_H_ */
