#ifndef MEMORY_SERVER_GUARD
#define MEMORY_SERVER_GUARD

typedef void (*request_handler)(int);

int handle_init(int clientSocket);
int handle_end(int clientSocket);
int handle_read(int clientSocket);
int handle_write(int clientSocket);
int handle_get_pages(int clientSocket);
int handle_free_page(int clientSocket);
int handle_frame_size(int clientSocket);

#endif
