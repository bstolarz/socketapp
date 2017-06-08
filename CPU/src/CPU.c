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
#include "functions/pcb.h"
#include "others/tests.h"
#include <signal.h>
#include <unistd.h>
void desconectarCPU(){
	printf("CPU desconectada por la senial SIGUSR1\n");
	desconectar=1;
}


int main(int arg, char* argv[]) {
	if(arg!=2){
		printf("Path missing! %d\n", arg);
		return 1;
	}
	printf("Checkout logCPU%d\n",(int)getpid());
	printf("PID of Linux: %d\n",(int)getpid());
	//Inicializo
	configCPU = malloc(sizeof(t_cpu));
	config_read(argv[1]);

	logCPU = log_create_file();
	log_config();
	desconectar=0;
	signal(SIGUSR1,desconectarCPU);
	//Me conecto a memoria
	memory_connect();

	//Inicio las estructuras de
	ansisop_init();


	socket_client_create(&serverKernel, configCPU->ip_kernel, configCPU->puerto_kernel);
	socket_send_string(serverKernel, "NewCPU");

	int interruption=0;
	//Chequeo señal SIGUSR1

	while(1){
		if ((pcb = recv_pcb(serverKernel)) == NULL){
			return EXIT_FAILURE;
		}

		int continuoEjecucion = 1;


		while(continuoEjecucion && pcb->exitCode == 1){
			//Fetch
			char* instruccion = cycle_fetch(pcb->indiceDeCodigo + pcb->pc);

			pcb->pc++;

			//Exec
			analizadorLinea(instruccion, ansisop_funciones, ansisop_funciones_kernel);

			//Checkeo interrupciones
			if((interruption=cycle_interruption_handler())<0){
				continuoEjecucion = 0;
				pcb->exitCode = interruption;
				print_interruption(pcb->exitCode);
			}


			//Consulto al kernel si continuo ejecutando - respondera segun planificacion
			continuoEjecucion = cycle_still_burst();
		}
		cycle_send_pcv(pcb);

		if (desconectar==1){
			printf("Exit OK!\n");
			return EXIT_SUCCESS;
		}

	}

	return EXIT_SUCCESS;
}



