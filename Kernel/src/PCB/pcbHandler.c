/*
 * pcbHandler.c
 *
 *  Created on: 6/4/2017
 *      Author: utnso
 */
#include <stdio.h>
#include <stdlib.h>
#include <parser/parser.h>
#include "../PCB/pcbHandler.h"
#include <parser/metadata_program.h>
#include <dirent.h>
#include <commons/log.h>

int PID=0;
void incrementarPID(){
	PID++;
}
void createPCB(t_pcb* PCB){
	incrementarPID();
	PCB->PID=PID;
	PCB->PC=0;
	t_dictionary* processFileTable=dictionary_create();
	PCB->processFileTable=processFileTable;
	PCB->stackPosition=0;
	PCB->cantPagsCodigo=0;
}
void printPCB(t_pcb* PCB){

	printf("PID: %d\n",PCB->PID);
	printf("PC: %d\n",PCB->PC);
	printf("Stack Position: %d\n",PCB->stackPosition);
	printf("Pages Counter: %d\n",PCB->cantPagsCodigo);
	printf("Process File Table size: %d\n",dictionary_size(PCB->processFileTable));
}
