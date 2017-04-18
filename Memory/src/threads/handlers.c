#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <commons/log.h>

#include "../libSockets/server.h"
#include "../libSockets/recv.h"
#include "../libSockets/send.h"

#include "../commons/structures.h"
#include "../commons/declarations.h"
#include "../functions/memory.h"

void handle_init(int clientSocket){
	int PID, pageCount, success;

	socket_recv_int(clientSocket, &PID);
	socket_recv_int(clientSocket, &pageCount);

	success = program_init(PID, pageCount);

	socket_send_int(clientSocket, success);
}

void handle_end(int clientSocket){
	int PID;

	socket_recv_int(clientSocket, &PID);

	program_end(PID);
}

void handle_read(int socket){
    int PID, page, offset, size;

    socket_recv_int(socket, &PID);
    //Agrego un send para notificarle a CPU el tamanio de pagina
    socket_send_int(socket,configMemory->frameSize);
    socket_recv_int(socket, &page);
    socket_recv_int(socket, &offset);
    socket_recv_int(socket, &size);

    void* data = memory_read(PID, page, offset, size);

    socket_send(socket, data, size);
}

void handle_write(int socket){
    int PID, page, offset, size;
    
    socket_recv_int(socket, &PID);
    socket_recv_int(socket, &page);
    socket_recv_int(socket, &offset);
    socket_recv_int(socket, &size);

    void* data;

    socket_recv(socket, &data, size);
    memory_write(PID, page, offset, size, data);

    // socket_send_int(socket, 0); // si no mandar error
}

void handle_frame_size(int clientSocket){
	socket_send_int(clientSocket, configMemory->frameSize);
}


