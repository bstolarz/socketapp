#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include "commons/structures.h"
#include "commons/declarations.h"
#include "functions/config.h"
#include "libSockets/client.h"
#include "libSockets/send.h"
#include "libSockets/recv.h"
#include <parser/parser.h>
#include "functions/primitivas.h"
#include "functions/serialization.h"
#include "functions/log.h"
#include "functions/memory.h"
#include "functions/ansisop.h"
#include "functions/instruction_cycle.h"
#include "others/tests.h"

t_pcb* recv_pcb()
{
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

// ejecutar instrucciones del programa

void instructionCycle(t_intructions* currentInstruction)
{
	printf("empieza en %d y tien length %d\n",
			currentInstruction->start, currentInstruction->offset);

	// fetch
	int codePage = currentInstruction->start / pageSize;
	int codeOffset = currentInstruction->start % pageSize;
	int size = currentInstruction->offset;
	void* data = memory_read(pcb->pid, codePage, codeOffset, size);

	if (data == NULL)
	{
		log_error(logCPU, "[fetch instruccion desde memoria] memoria no me mando datos para instruccion page: %d / offset: %d / size : %d\n", codePage, codeOffset, size);
		pcb->exitCode = -5;// TODO: reemplazar con ERROR_MEMORY o el correspondiente;
		return;
	}
	// hay que procesar un poquito la instruccion
	// porque si termina en '\n' (no termina en '\0') el string que le pase al parser va a tener fruta al final
	// char* instructionStr = (char*) malloc(size + 1);
	// memcpy(instructionStr, data, size);
	char* instructionStr = (char*) data;

	// pongo algo que lo frene al final para que el analizador no se confunda
	if (isblank(instructionStr[size - 1]) ||
		instructionStr[size - 1] == '\n')
	{
		instructionStr[size - 1] = '\0';
	}
	else
	{
		log_debug(logCPU, "realocando porque habia un caracter que no era fin de linea al final de la instruccion (%c)", instructionStr[size - 1]);
		instructionStr = realloc(instructionStr, size + 1);
		instructionStr[size] = '\0';
	}

	log_debug(logCPU, "[fetch instruccion desde memoria] instruccion: [%s]\n", instructionStr);

	// exec
	analizadorLinea(instructionStr, ansisop_funciones, ansisop_funciones_kernel);

	// liberar recursos
	free(instructionStr);
}

int main(int arg, char* argv[]) {
	if(arg!=2){
		printf("Path missing! %d\n", arg);
		return 1;
	}

	//Inicializo
	configCPU = malloc(sizeof(t_cpu));
	config_read(argv[1]);

	logCPU = log_create_file();
	log_config();

	//Me conecto a memoria
	memory_connect();

	//Inicio las estructuras de
	ansisop_init();

	//Codigo temporal para testear primitivas
	/*
	pcb_to_test_primitives();
	programLoop();
	return 0;
	*/

	socket_client_create(&serverKernel, configCPU->ip_kernel, configCPU->puerto_kernel);
	socket_send_string(serverKernel, "NewCPU");

	int interruption=0;

	while(1){
		if ((pcb = recv_pcb(serverKernel)) == NULL){
			return EXIT_FAILURE;
		}

		int continuoEjecucion = 1;
		while(continuoEjecucion && pcb->exitCode == 0){
			instructionCycle(pcb->indiceDeCodigo + pcb->pc);
			++pcb->pc;

			//Checkeo interrupciones
			if((interruption=cycle_interruption_handler())<0){
				continuoEjecucion = 0;
				pcb->exitCode = interruption;

			}

			//Consulto al kernel si continuo ejecutando - respondera segun planificacion
			continuoEjecucion = cycle_still_burst();
		}

		//TODO devolver el pcb
	}

	return EXIT_SUCCESS;
}



