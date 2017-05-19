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

#include "../libSockets/send.h"
#include "../libSockets/recv.h"

#include "../commons/structures.h"
#include "../commons/declarations.h"

#include "../planner/dispatcher.h"
#include "../functions/serialization.h"
#include "../functions/program.h"


void cpu_send_pcb(t_cpu* cpu){
	log_info(logKernel,"socket is: %d\n",cpu->socket);
	socket_send_string(cpu->socket,"PCB");
	log_info(logKernel,"Envio la PCB del programa con PID: %d\n",cpu->program->pcb->pid);

	t_dataBlob serializedPcb = pcb_serialize(cpu->program->pcb);
	int sentSize = socket_send(cpu->socket, serializedPcb.data, serializedPcb.size);

	if (sentSize == -1){
		log_error(logKernel, "no pude mandar el pcb del proceso: %d\n",cpu->program->pcb->pid);
	}

	free(serializedPcb.data);
}

t_pcb* cpu_recv_pcb(t_cpu* cpu){
	char* message;

	log_debug(logKernel, "[recv PCB] esperando ...");
	socket_recv_string(cpu->socket, &message);

	if (string_equals_ignore_case("PCB", message))
	{
		log_debug(logKernel, "[recv PCB] llego mensaje PCB ...");

		t_dataBlob serializedPcb;
		int recvSize = socket_recv(cpu->socket, (void*)&serializedPcb.data, 1);

		if (recvSize == -1)
		{
			log_error(logKernel, "[PCB recv] obtuve size del pcb = -1");
			return NULL;
		}
		else
		{
			log_debug(logKernel, "[recv PCB] llego bien el PCB");
			serializedPcb.size = recvSize;
			return pcb_deserialize(serializedPcb);
		}
	}

	log_error(logKernel, "[PCB recv] nunca llego el mensaje PCB");
	return NULL;
}

void cpu_destroy_pcb(t_cpu* cpu){
	//TODO
}

t_cpu* cpu_find(int socket){
	bool _buscarProgramaSocketInCPUs(t_cpu* cpu){
		return cpu->socket==socket;
	}

	return list_find(queueCPUs->list, (void*)_buscarProgramaSocketInCPUs);
}

