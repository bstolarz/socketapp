#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "../libSockets/client.h"
#include "../libSockets/send.h"
#include "../libSockets/recv.h"
#include "../commons/structures.h"
#include "../commons/declarations.h"
#include "memory.h"
#include "pcb.h"

int cycle_interruption_handler(){
	int response=0;
	if(socket_send_string(serverKernel, "interruption")<=0){
		exit(EXIT_FAILURE);
	}

	if(socket_recv_int(serverKernel, &response)<=0){
		exit(EXIT_FAILURE);
	}

	return response;
}

int cycle_still_burst(){
	int response = 0;

	if(socket_send_string(serverKernel, "still_burst")<=0){
		exit(EXIT_FAILURE);
	}

	if(socket_recv_int(serverKernel, &response)<=0){
		exit(EXIT_FAILURE);
	}

	return response;
}

char * cycle_fetch(t_intructions* currentInstruction){
	printf("empieza en %d y tien length %d\n", currentInstruction->start, currentInstruction->offset);

	int codePage = currentInstruction->start / pageSize;
	int codeOffset = currentInstruction->start % pageSize;
	int size = currentInstruction->offset;

	char* instructionStr = memory_read(pcb->pid, codePage, codeOffset, size);

	if (instructionStr == NULL){
		log_error(logCPU, "[fetch instruccion desde memoria] memoria no me mando datos para instruccion page: %d / offset: %d / size : %d", codePage, codeOffset, size);
		pcb->exitCode = -5; //TODO: reemplazar con ERROR_MEMORY o el correspondiente;
		return "";
	}

	if (isblank(instructionStr[size - 1]) || instructionStr[size - 1] == '\n') {
		instructionStr[size - 1] = '\0';
	} else {
		log_debug(logCPU, "realocando porque habia un caracter que no era fin de linea al final de la instruccion (%c)\n", instructionStr[size - 1]);
		instructionStr = realloc(instructionStr, size + 1);
		instructionStr[size] = '\0';
	}

	log_debug(logCPU, "[fetch instruccion desde memoria] instruccion: [%s]\n", instructionStr);

	return instructionStr;
}

void cycle_send_pcv(t_pcb* pcb){
	if(socket_send_string(serverKernel, "end_burst") != -1){
		send_pcb(pcb);
	}
	else
	{
		log_error(logCPU, "[cycle_send_pcb] no pude enviar mensaje end_burst");
	}
}
