#include <stddef.h>
#include "memory_requests.h"
#include "../libSockets/send.h"
#include "../libSockets/recv.h"

int memory_request_init(int serverSocket, int PID, int pageCount)
{
    if ((socket_send_string(serverSocket, "init") > 0)  &&
        (socket_send_int(serverSocket, PID) > 0) &&
        (socket_send_int(serverSocket, pageCount) > 0))
    {
        int createProgramResult;
        
        if (socket_recv_int(serverSocket, &createProgramResult) == sizeof(int))
            return createProgramResult;
    }

    return -1;
}

int memory_request_end(int serverSocket, int PID)
{
    if ((socket_send_string(serverSocket, "end") > 0)  &&
        (socket_send_int(serverSocket, PID) > 0))
    {
        int programDestroyResult;
        
        if (socket_recv_int(serverSocket, &programDestroyResult) == sizeof(int))
            return programDestroyResult;
    }
    
    return -1;
}

void* memory_request_read(int serverSocket, int PID, int page, int offset, int size)
{
    if ((socket_send_string(serverSocket, "read") > 0) &&
        (socket_send_int(serverSocket, PID) > 0) &&
        (socket_send_int(serverSocket, page) > 0) &&
        (socket_send_int(serverSocket, offset) > 0) &&
        (socket_send_int(serverSocket, size) > 0))
    {
        void* data;
        
        if (socket_recv(serverSocket, &data, 1) == size)
            return data;
    }

    return NULL;
}

int memory_request_write(int serverSocket, int PID, int page, int offset, int size, void* buffer)
{
    if ((socket_send_string(serverSocket, "write") > 0) &&
        (socket_send_int(serverSocket, PID) > 0) &&
        (socket_send_int(serverSocket, page) > 0) &&
        (socket_send_int(serverSocket, offset) > 0) &&
        (socket_send(serverSocket, buffer, size) == size))
    {
    	int writeResult;
    	int recvResult = socket_recv_int(serverSocket, &writeResult);

    	if (recvResult == -1) return -1;
    	return writeResult;
    }
    
    return -1;
}

int memory_request_frame_size(int serverSocket)
{
	int connectionResult = socket_send_string(serverSocket, "frame_size");
    if (connectionResult == -1) return -1;

    int frameSize = -1;
    connectionResult = socket_recv_int(serverSocket, &frameSize); // -1 si no pudo conectarse; si no
    if (connectionResult == -1) return -1;

    return frameSize;
}
