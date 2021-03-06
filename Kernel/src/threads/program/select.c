#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdint.h>
#include <pthread.h>

#include <commons/string.h>

#include "../../libSockets/server.h"
#include "../../libSockets/recv.h"
#include "../../libSockets/send.h"

#include "../../commons/declarations.h"
#include "../../commons/error_codes.h"
#include "../../functions/program.h"


void select_program_socket_connection_lost(fd_set* master, int socket, int nbytes){
	programMasterRecord = master;

	log_info(logKernel, "[PROGRAMA] desconexion consola (sock: %d)", socket);
	program_interrup(socket, ERROR_CONSOLE_DISCONNECTED, 0);
	FD_CLR(socket, master);
	log_info(logKernel, "[PROGRAMA] fin desconexion consola (sock: %d)", socket);
}

void select_program_socket_recive_package(fd_set* master, int socket, int nbytes, char* package){
	programMasterRecord = master;

	if(strcmp(package, "NewProgram") == 0){
		log_info(logKernel, "[PROGRAMA] %s (%d)", package, socket);
		program_process_new(master, socket);
		log_info(logKernel, "[PROGRAMA FIN] %s (%d)", package, socket);
	}else if(strcmp(package, "Finished") == 0){
		log_info(logKernel, "[PROGRAMA] %s (%d)", package, socket);
		program_interrup(socket, ERROR_CONSOLE_FINISH_COMMAND, 0);
		// la consola se desconecta despues de mandar finish. mejor no mantenerlo.
		// igaual el program_finish lo saca despues. pero para indicar q pasa lo mismo q con el conosole disconect
		FD_CLR(socket, master);
		log_info(logKernel, "[PROGRAMA FIN] %s (%d)", package, socket);
	}else{
		log_warning(logKernel, "[PROGRAMA error] %s (%d)", package, socket);
	}
}

void* select_program_thread_launcher(void* arg){
	socket_server_select(configKernel->puerto_prog, *select_program_socket_connection_lost, *select_program_socket_recive_package);
	return arg;
}
