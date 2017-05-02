#include <netdb.h>
#include <unistd.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include "commons/structures.h"
#include "commons/declarations.h"
#include "functions/config.h"
#include "libSockets/client.h"
#include "libSockets/send.h"
#include "libSockets/recv.h"
#include "libSockets/server.h"
#include "functions/memory_requests.h"
#include <parser/parser.h>
#include "functions/primitivas.h"
#include "functions/serialization.h"

int serverKernel=0;
int serverMemory=0;

t_log* logCreate()
{
	char pid[10];
	snprintf(pid, 10,"%d",(int)getpid());

	char* path = string_new();
	string_append(&path, "logCPU");
	string_append(&path, pid);

	remove(path);

	t_log* logs = log_create(path, "CPU", false, LOG_LEVEL_DEBUG);

	if (logs == NULL) {
		printf("[SISTEMA] - No se pudo generar el archivo de logueo.\n");
		return NULL;
	};


	log_info(logs, "Archivo de logueo inicializado.");

	return logs;
}

void init(char* configPath)
{
	configCPU = malloc(sizeof(t_cpu));
	config_read(configPath);
	config_print();

	logCPU = logCreate();

	funciones = (AnSISOP_funciones*) malloc(sizeof(AnSISOP_funciones));
	funciones->AnSISOP_asignar=AnSISOP_asignar;
	funciones->AnSISOP_definirVariable=AnSISOP_definirVariable;
	funciones->AnSISOP_dereferenciar=AnSISOP_dereferenciar;
	funciones->AnSISOP_obtenerPosicionVariable=AnSISOP_obtenerPosicionVariable;
	funciones->AnSISOP_finalizar=AnSISOP_finalizar;

	kernel = (AnSISOP_kernel*) malloc(sizeof(AnSISOP_kernel));
}

t_pcb* recv_pcb()
{
	char* message;
	printf("esperando mensaje PCB...");
	socket_recv_string(serverKernel, &message);
	printf("obtuve mensaje PCb");

	if (string_equals_ignore_case("PCB", message))
	{
		t_serialized serializedPcb;
		int recvSize = socket_recv(serverKernel, (void*)&serializedPcb.data, 1);

		if (recvSize == -1)
		{
			log_error(logCPU, "[PCB recv] obtuve size -1");
			return NULL;
		}
		else
		{
			serializedPcb.size = recvSize;
			return pcb_deserialize(serializedPcb);
		}
	}

	return NULL;
}

void connect_to_memory()
{
	socket_client_create(&serverMemory, configCPU->ip_memory, configCPU->puerto_memory);

	// obtener tamanio de pagina
	pageSize = memory_request_frame_size(serverMemory);

	if (pageSize > 0)
	{
		log_info(logCPU,"[page_size] se obtuvo pageSize = %d\n", pageSize);
	}
	else
	{
		log_error(logCPU, "[page_size] no se mando bien. exit program\n");
		exit(EXIT_FAILURE);
	}
}

// ejecutar instrucciones del programa

void instructionCycle(t_intructions* currentInstruction)
{
	printf("empieza en %d y tien length %d\n",
			currentInstruction->start, currentInstruction->offset);

	// fetch
	int codePage = currentInstruction->start / pageSize;
	int codeOffset = currentInstruction->start % pageSize;
	int size = currentInstruction->offset;
	void* data = memory_request_read(serverMemory, pcb->pid, codePage, codeOffset, size);

	if (data == NULL) printf("no pude obtener instruccion\n");
	else printf("instruccion desde memoria: %s\n", (char*)data);

	// exec
	// analizadorLinea(data, funciones, kernel);
}

void programLoop()
{
	int i;

	for (i = 0; i != pcb->indiceDeCodigoCant; ++i)
	{
		printf("instruccion %d:\n", i);
		instructionCycle(pcb->indiceDeCodigo + i);
		++pcb->pc;
		// check interrupts?
	}
}

int main(int arg, char* argv[]) {
	if(arg!=2){
		printf("Path missing! %d\n", arg);
		return 1;
	}

	init(argv[1]);


	connect_to_memory();
	socket_client_create(&serverKernel, configCPU->ip_kernel, configCPU->puerto_kernel);

	if (serverKernel)
	{
		socket_send_string(serverKernel, "NewCPU");
		pcb = recv_pcb(serverKernel);

		if (pcb == NULL) return EXIT_FAILURE;

		programLoop();

	}

	return EXIT_SUCCESS;
}



