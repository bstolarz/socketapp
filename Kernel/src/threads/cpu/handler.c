#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>
#include <sys/socket.h>

#include <commons/collections/list.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/log.h>

#include "../../libSockets/send.h"
#include "../../libSockets/recv.h"

#include "../../commons/structures.h"
#include "../../commons/declarations.h"

#include "../../planner/dispatcher.h"

#include "../../functions/cpu.h"

void handle_new_cpu(int socket){
	t_cpu* cpu = malloc(sizeof(t_cpu));
	cpu->socket = socket;
	list_add(queueCPUs->list,cpu);
	log_info(logKernel,"New CPU added to list\n");
	if(list_size(queueReadyPrograms->list)>0){
		cpu->program = planificar();
		if(cpu->program != NULL){
			cpu_send_pcb(cpu);
		}
	}else{
		cpu->program=NULL;
	}
	//TODO send quantum
}

void handle_interruption(t_cpu * cpu){
	if(socket_send_int(cpu->socket, cpu->program->interruptionCode)<=0){
		exit(EXIT_FAILURE);
	}
}

void handle_still_burst(t_cpu* cpu){
	int burst = 1;
	if(cpu->program->waiting == 1){
		burst = 0;
		cpu->program->quantum = 0;
	}else{
		if(cpu->program->quantum != -1){
			cpu->program->quantum = cpu->program->quantum - 1;
			if(cpu->program->quantum == 0){
				burst = 0;
			}
		}
	}

	if(socket_send_int(cpu->socket, burst)<=0){
		exit(EXIT_FAILURE);
	}
}

void handle_end_burst(t_cpu* cpu){
	t_program* program = cpu->program;
	program->pcb = cpu_recv_pcb(cpu);
	program->quantum = 0;

	int termino = 0;
	if(socket_recv_int(cpu->socket, &termino)<=0){
		//TODO Eliminar cpu de la lista de cpus
		exit(EXIT_FAILURE);
	}

	if(termino == 1){
		program_finish(program);
	}else{
		if(program->waiting == 1){
			pthread_mutex_lock(&queueBlockedPrograms->mutex);
			list_add(queueBlockedPrograms->list, program);
			pthread_mutex_unlock(&queueBlockedPrograms->mutex);
		}else{
			pthread_mutex_lock(&queueReadyPrograms->mutex);
			list_add(queueReadyPrograms->list, program);
			pthread_mutex_unlock(&queueReadyPrograms->mutex);
		}

	}

	cpu->program = planificar();
	if(cpu->program != NULL){
		cpu_send_pcb(cpu);
	}
}

void handle_cpu_get_shared_variable(t_cpu* cpu){
	//Obtengo el nombre de la shared variable
	char* sharedVariable=string_new();
	if (socket_recv_string(cpu->socket,&sharedVariable)<=0){
		log_info(logKernel,"No se obtuvo el nombre de la shared variable de %d\n", cpu->socket);
		return;
	}

	//Busco la shared variable
	int _is_the_variable(t_sharedVar* var){
		return strcmp(sharedVariable,var->nombre);
	}
	t_sharedVar* sv = list_find(configKernel->shared_vars,(void*)_is_the_variable);

	//Verifico que exista
	if(sv == NULL){
		log_info(logKernel,"La shared variable '%s' que solicito %d no existe.\n", sv->nombre, cpu->socket);
		if(socket_send_string(cpu->socket, "Failure")<=0){
			log_info(logKernel,"No se pudo informar el estado a %d\n", sv->nombre, cpu->socket);
		}
		return;
	}

	if(socket_send_string(cpu->socket, "Success")<=0){
		log_info(logKernel,"No se pudo informar el estado a %d\n", sv->nombre, cpu->socket);
		return;
	}

	//Envio el valor de la shared variable
	pthread_mutex_lock(&sv->mutex);
	if(socket_send_int(cpu->socket, sv->value)<=0){
		log_info(logKernel,"Ocurrio un error al enviarle el valor de la shared variable '%s' a %d\n", sv->nombre, cpu->socket);
	}
	pthread_mutex_unlock(&sv->mutex);

}

void handle_cpu_set_shared_variable(t_cpu* cpu){
	//Obtengo el nombre de la shared variable
	char* sharedVariable=string_new();
	if (socket_recv_string(cpu->socket,&sharedVariable)<=0){
		log_info(logKernel,"No se obtuvo el nombre de la shared variable de %d\n", cpu->socket);
		return;
	}

	//Obtengo el valor de la shared variable
	int value = 0;
	if (socket_recv_int(cpu->socket,&value)<=0){
		log_info(logKernel,"No se obtuvo el valor de la shared variable de %d\n", cpu->socket);
		return;
	}

	//Busco la shared variable
	int _is_the_variable(t_sharedVar* var){
		return strcmp(sharedVariable,var->nombre);
	}
	t_sharedVar* sv = list_find(configKernel->shared_vars,(void*)_is_the_variable);

	//Verifico que exista
	if(sv == NULL){
		log_info(logKernel,"La shared variable '%s' que solicito %d no existe.\n", sv->nombre, cpu->socket);
		if(socket_send_string(cpu->socket, "Failure")<=0){
			log_info(logKernel,"No se pudo informar el estado a %d\n", sv->nombre, cpu->socket);
		}
		return;
	}

	//Envio el valor de la shared variable
	pthread_mutex_lock(&sv->mutex);
	sv->value = value;
	pthread_mutex_unlock(&sv->mutex);

	if(socket_send_string(cpu->socket, "Success")<=0){
		log_info(logKernel,"No se pudo informar el estado a %d\n", sv->nombre, cpu->socket);
	}
}

void handle_cpu_wait(t_cpu* cpu){
	//Obtengo el nombre de la shared variable
	char* semaforo=string_new();
	if (socket_recv_string(cpu->socket,&semaforo)<=0){
		log_info(logKernel,"No se obtuvo el nombre del semaforo de %d\n", cpu->socket);
		return;
	}

	//Busco la shared variable
	int _es_el_semaforo(t_semaforo* var){
		return strcmp(semaforo,var->nombre)==0;
	}
	t_semaforo* sem = list_find(configKernel->semaforos,(void*)_es_el_semaforo);

	printf("sem %s %d\n", sem->nombre, sem->value);

	//Verifico que exista
	if(sem == NULL){
		log_info(logKernel,"El semaforo '%s' que solicito %d no existe.\n", sem->nombre, cpu->socket);
		if(socket_send_string(cpu->socket, "Failure")<=0){
			log_info(logKernel,"No se pudo informar el estado a %d\n", sem->nombre, cpu->socket);
		}
		return;
	}

	if(socket_send_string(cpu->socket, "Success")<=0){
		log_info(logKernel,"No se pudo informar el estado a %d\n", sem->nombre, cpu->socket);
	}

	//Envio el valor de la shared variable
	pthread_mutex_lock(&sem->mutex);

	int resp;
	if(sem->value > 0){
		sem->value = sem->value -1;
		resp = 1;
	}else{
		resp = 0;
		cpu->program->waiting = 1;
		cpu->program->waitingReason = string_duplicate(semaforo);
	}

	if(socket_send_int(cpu->socket, resp)<=0){
		log_info(logKernel,"Ocurrio un error al enviarle el valor del semaforo '%s' a %d\n", sem->nombre, cpu->socket);
	}
	pthread_mutex_unlock(&sem->mutex);
}

void handle_cpu_signal(t_cpu* cpu){
	//Obtengo el nombre de la shared variable
	char* semaforo=string_new();
	if (socket_recv_string(cpu->socket,&semaforo)<=0){
		log_info(logKernel,"No se obtuvo el nombre del semaforo de %d\n", cpu->socket);
		return;
	}

	//Busco la shared variable
	int _es_el_semaforo(t_semaforo* var){
		return strcmp(semaforo,var->nombre);
	}
	t_semaforo* sem = list_find(configKernel->semaforos,(void*)_es_el_semaforo);

	//Verifico que exista
	if(sem == NULL){
		log_info(logKernel,"El semaforo '%s' que solicito %d no existe.\n", sem->nombre, cpu->socket);
		if(socket_send_string(cpu->socket, "Failure")<=0){
			log_info(logKernel,"No se pudo informar el estado a %d\n", sem->nombre, cpu->socket);
		}
		return;
	}

	if(socket_send_string(cpu->socket, "Success")<=0){
		log_info(logKernel,"No se pudo informar el estado a %d\n", sem->nombre, cpu->socket);
	}

	//Envio el valor de la shared variable
	pthread_mutex_lock(&sem->mutex);
	sem->value = sem->value + 1;
	pthread_mutex_unlock(&sem->mutex);

	//TODO avisarle a los bloqueados que se levanto este semaforo
}
void handle_cpu_alocar(t_cpu* cpu){
	//TODO
}

void handle_cpu_liberar(t_cpu* cpu){
	//TODO
}

void handle_cpu_abrir(t_cpu* cpu){
	//TODO
}

void handle_cpu_borrar(t_cpu* cpu){
	//TODO
}

void handle_cpu_cerrar(t_cpu* cpu){
	//TODO
}

void handle_cpu_mover_cursor(t_cpu* cpu){
	//TODO
}

void handle_cpu_escribir(t_cpu* cpu){
	printf("entramos a escribir\n");
	int FD = 0;
	//Envio al kernel el descriptor de archivo
	if (socket_recv_int(cpu->socket,&FD)<=0){
		log_info(logKernel, "No se pudo obtener el FD de: %i\n", cpu->socket);
		return;
	}

	//Envio al kernel la informacion con su tamanio
	char* buffer = string_new();
	int nbytes=0;
	if ((nbytes = socket_recv(cpu->socket, (void**)&buffer, 1))<=0){
		log_info(logKernel, "No se pudo obtener el buffer de: %i\n", cpu->socket);
		return;
	}

//	printf("%i %s\n", FD, buffer);
	//printf("%i\n", DESCRIPTOR_SALIDA);
	//if(FD == DESCRIPTOR_SALIDA){
	if(FD == 0){ //Por algun motivo cuando es imprimir me llama con 0
		if(buffer[nbytes] != '\0'){
			buffer = realloc(buffer, nbytes+1);
			buffer[nbytes] = '\0';
			nbytes = nbytes + 1;
		}

		printf("%i %s\n", FD, buffer);

	//	printf("todo listo para mandar\n");
		if(socket_send_string(cpu->program->socket, "imprimir")<=0){
			log_info(logKernel,"No se pudo imprimir en: %i\n", cpu->program->socket);
		}

		if(socket_send_string(cpu->program->socket, buffer)<=0){
			log_info(logKernel,"No se pudo imprimir el mensaje en: %i\n", cpu->program->socket);
		}
	}

}

void handle_cpu_leer(t_cpu* cpu){
	//TODO
}


