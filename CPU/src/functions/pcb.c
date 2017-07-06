#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include "../commons/structures.h"
#include "../commons/declarations.h"
#include "config.h"
#include "../libSockets/client.h"
#include "../libSockets/send.h"
#include "../libSockets/recv.h"
#include <parser/parser.h>
#include "primitivas.h"
#include "serialization.h"
#include "log.h"
#include "memory.h"
#include "ansisop.h"
#include "instruction_cycle.h"


t_pcb* recv_pcb(){
	char* message;

	log_debug(logCPU, "[recv PCB] esperando ...");
	socket_recv_string(serverKernel, &message);

	if (string_equals_ignore_case("PCB", message))
	{
		log_debug(logCPU, "[recv PCB] llego mensaje PCB ...");

		t_dataBlob serializedPcb;
		int recvSize = socket_recv(serverKernel, (void*)&serializedPcb.data, 1);

		if (recvSize == -1)
		{
			log_error(logCPU, "[PCB recv] obtuve size del pcb = -1");
			return NULL;
		}
		else
		{
			log_debug(logCPU, "[recv PCB] llego bien el PCB");
			serializedPcb.size = recvSize;

			return pcb_deserialize(serializedPcb);
		}
	}

	log_error(logCPU, "[PCB recv] nunca llego el mensaje PCB");
	return NULL;
}

void send_pcb(t_pcb* pcb){
	log_info(logCPU,"socket is: %d\n",serverKernel);
	socket_send_string(serverKernel,"PCB");
	log_info(logCPU,"Envio la PCB del programa con PID: %d\n",pcb->pid);

	t_dataBlob serializedPcb = pcb_serialize(pcb);
	int sentSize = socket_send(serverKernel, serializedPcb.data, serializedPcb.size);

	if (sentSize == -1){
		log_error(logCPU, "[send_pcb] no pude mandar el pcb del proceso: %d\n",pcb->pid);
	}
	else {
		log_info(logCPU, "[send_pcb] mande %d bytes", sentSize);
	}

	free(serializedPcb.data);
}
