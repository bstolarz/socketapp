#ifndef MEMORY_SERVER_GUARD
#define MEMORY_SERVER_GUARD

typedef void (*request_handler)(int);

void handle_init(int clientSocket);
void handle_end(int clientSocket);
void handle_read(int clientSocket);
void handle_write(int clientSocket);
void handle_frame_size(int clientSocket);

#endif
