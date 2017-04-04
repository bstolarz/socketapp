/*
 ============================================================================
 Name        : Kernel.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include "functions/config.h"
#include <stdio.h>
#include <stdlib.h>
#include <commons/collections/list.h>
#include <commons/config.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>

#include "commons/structures.h"
//#include "commons/definitions.h"
//#include "threads/select.h"

#include "commons/definitions.h"
#include "threads/select.h"
#include "libSockets/send.h"


int main(int args, char* argv[]) {

	//clientes = list_create();
	//pthread_create(&selectThread,NULL,selectThreadLauncher, NULL);
	//pthread_join(selectThread, NULL);
	if (args!=2){
		printf("Parametros incorrectamente seteados. Recorda poner el path\n");
		return 1;
	}
	leerConfiguracionDeKernel(argv[1]);

	clientes = list_create();
//	pthread_create(&selectThread,NULL,selectThreadLauncher, NULL);


	char str[100];

	void _send_message_clients(int* i){
		socket_send_string(*i, str);
	}

	while(1){
		scanf("%s", str);
		list_iterate(clientes, (void*)_send_message_clients);
	}


//	pthread_join(selectThread, NULL);
	return EXIT_SUCCESS;
}
