#ifndef MEMORY_REQUEST_GUARD
#define MEMORY_REQUEST_GUARD
extern int ERROR_MEMORY;

void* memory_read(int PID, int page, int offset, int size);
int memory_write(int PID, int page, int offset, int size, void* buffer);
int memory_frame_size();
void memory_connect();

#endif
