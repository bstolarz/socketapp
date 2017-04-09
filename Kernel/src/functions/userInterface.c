/*
 * userInterface.c
 *
 *  Created on: 9/4/2017
 *      Author: utnso
 */
#include <stdio.h>
#include <stdlib.h>
#include "userInterface.h"
#include "../PCB/pcbHandler.h"

void initNewAnsisopProgram(char* command){
	printf("About to Start Ansisop Program\n");
	printf("The program is located in: %s\n",command);
	t_pcb* PCB=(t_pcb*)malloc(sizeof(t_pcb));
	createPCB(PCB);
	printPCB(PCB);
}
void finishAnsisopProgram(char* command){

}
void disconnectAllConsoleThreads(){

}
void cleanMessagesOnScreen(){

}
