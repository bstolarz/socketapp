#ifndef MEMORY_SERVER_GUARD
#define MEMORY_SERVER_GUARD

// request types
typedef struct t_init_program_request
{
	int clientSocket;
	int32_t PID;
	int32_t pageCount;
}
t_init_program_request;

typedef struct t_end_program_request
{
	int clientSocket;
	int32_t PID;
}
t_end_program_request;

typedef struct t_read_request
{
	int clientSocket;
	int32_t PID;
	int32_t page;
	int32_t offset;
	int32_t size;
}
t_read_request;

typedef struct t_write_request
{
	int clientSocket;
	int32_t PID;
	int32_t page;
	int32_t offset;
	int32_t size;
	void* buffer;
}
t_write_request;


void* handle_init(void* clientSocketPtr);
void* handle_end(void* clientSocketPtr);
void* handle_read(void* socketPtr);
void* handle_write(void* socketPtr);
void* handle_frame_size(void* clientSocketPtr);

#endif
