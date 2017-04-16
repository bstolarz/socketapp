#ifndef MEMORY_SERVER_GUARD
#define MEMORY_SERVER_GUARD

void handle_init(int clientSocket);
void handle_end(int clientSocket);
void handle_read(int socket);
void handle_write(int socket);
void handle_frame_size(int clientSocket);

#endif
