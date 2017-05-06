#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>
#include <sys/socket.h>

#include <commons/collections/list.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/log.h>

#include "../../libSockets/send.h"
#include "../../libSockets/recv.h"

#include "../../commons/structures.h"
#include "../../commons/declarations.h"

#include "../../functions/dispatcher.h"

void handle_cpu_get_shared_variable(t_cpu* cpu){
	char* sharedVariable=string_new();
	if (socket_recv_string(cpu->socket,&sharedVariable)>0){
		int _is_the_variable(t_sharedVar* var){
			return strcmp(sharedVariable,var->nombre);
		}
		int value = (int)list_find(configKernel->shared_vars,(void*)_is_the_variable);

		socket_send_int(cpu->socket, value);
	}
}

void handle_cpu_set_shared_variable(t_cpu* cpu){
	//TODO
}

void handle_cpu_finish(t_cpu* cpu){
	//TODO
}

void handle_cpu_wait(t_cpu* cpu){
	//TODO
}

void handle_cpu_signal(t_cpu* cpu){
	//TODO
}

void handle_cpu_reservar(t_cpu* cpu){
	//TODO
}

void handle_cpu_liberar(t_cpu* cpu){
	//TODO
}

void handle_cpu_abrir(t_cpu* cpu){
	//TODO
}

void handle_cpu_borrar(t_cpu* cpu){
	//TODO
}

void handle_cpu_cerrar(t_cpu* cpu){
	//TODO
}

void handle_cpu_mover_cursor(t_cpu* cpu){
	//TODO
}

void handle_cpu_escribir(t_cpu* cpu){
	//TODO
}

void handle_cpu_leer(t_cpu* cpu){
	//TODO
}


