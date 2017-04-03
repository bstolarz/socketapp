/*
 ============================================================================
 Name        : Kernel.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <commons/collections/list.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>
#include "commons/definitions.h"
#include "threads/select.h"
#include "libSockets/send.h"

int main(void) {

	clientes = list_create();
	pthread_create(&selectThread,NULL,selectThreadLauncher, NULL);

	char str[100];

	void _send_message_clients(int* i){
		socket_send_string(*i, str);
	}

	while(1){
		scanf("%s", str);
		list_iterate(clientes, (void*)_send_message_clients);
	}


	pthread_join(selectThread, NULL);
	return EXIT_SUCCESS;
}
