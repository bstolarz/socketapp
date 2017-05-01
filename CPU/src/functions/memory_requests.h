#ifndef MEMORY_REQUEST_GUARD
#define MEMORY_REQUEST_GUARD

// 0 ok,
// -1 no pudo conectarse
// ERROR_NO_RESOURCES_FOR_PROCCESS si no habia espacio en memoria
int memory_request_init(int serverSocket, int PID, int pageCount);

// 0 ok
// -1 no pudo conectarse
int memory_request_end(int serverSocket, int PID);

// retorna una lo que leyo. si hubo error, retorna NULL
void* memory_request_read(int serverSocket, int PID, int page, int offset, int size);

// retorn size si pudo;
// si no -1 si hubo problema mandando o recviendo; ERROR_MEMORY si memoria no pudo escribir
int memory_request_write(int serverSocket, int PID, int page, int offset, int size, void* buffer);

// retorna tamano del frame. si hubo error retorna -1
int memory_request_frame_size(int serverSocket);

#endif
