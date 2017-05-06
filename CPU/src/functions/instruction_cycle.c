#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../libSockets/client.h"
#include "../libSockets/send.h"
#include "../libSockets/recv.h"
#include "../commons/structures.h"
#include "../commons/declarations.h"

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
