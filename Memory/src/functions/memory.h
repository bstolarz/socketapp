#ifndef FUNCTIONS_MEMORY_H_
#define FUNCTIONS_MEMORY_H_

void memory_init();
void memory_destroy();

// Operaciones de Memoria
void program_end(int PID);
void* memory_read(int PID, int page, int offset, int size);
int memory_write(int PID, int page, int offset, int size, void* buffer);

#endif /* FUNCTIONS_MEMORY_H_ */
