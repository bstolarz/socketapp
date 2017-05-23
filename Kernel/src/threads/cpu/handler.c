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
}
t_gobal_fd* existeArchivoEnTablaGlobalDeArchivos(t_list * l, char* path){
	int tamanio=list_size(l);
	int i;
	t_gobal_fd* pointerToGlobalFile=NULL;
	for (i=0;i!=tamanio;i++){
		t_gobal_fd* globalFD=(t_gobal_fd*)list_get(l,i);
		if(strcmp(globalFD->path,path)==0){
			pointerToGlobalFile=globalFD;
		}
	}
	return pointerToGlobalFile;
}
int filesystem_validar(char* path, t_cpu* cpu){
	//Le pido el file descriptor a FS
	if (socket_send_string(fileSystemServer.socket,"VALIDAR")>0){
		log_info(logKernel, "Le indico a FS que quiero validar una path");
	}else{
		log_info(logKernel, "Error al indicarle a FS que quiero validar una path");
	}
	//Envio el path a FS
	if (socket_send_string(fileSystemServer.socket,path)>0){
		log_info(logKernel, "Le envio a FS la path: %s",path);
	}else{
		log_info(logKernel, "Error al enviarle a FS la path: %s", path);
	}
	int respuesta;
	if (socket_recv_int(cpu->socket,&respuesta)>0){
		log_info(logKernel, "Recibo la validacion del FS");
		if (respuesta>0){
			log_info(logKernel, "Existe el archivo en FS");
		}else{
			log_info(logKernel, "No existe el archivo en FS");
			EXIT_FAILURE;
		}
	}
	return respuesta;
}
int filesystem_create(char* flags,char* path){
	//Le envio al FS la orden de crear
	if(socket_send_string(fileSystemServer.socket,"CREAR")>0){
		log_info(logKernel, "Le informo al FS que cree un archivo");
	}else{
		log_info(logKernel, "Error al informar as FS que cree un archivo");
	}
	//Le envio al FS el path
	if(socket_send_string(fileSystemServer.socket,path)>0){
		log_info(logKernel, "Le envio al FS la path: %s",path);
	}else{
		log_info(logKernel, "Error al enviar al FS el archivo: %s", path);
	}

	//Recibo exito o fail de FS
	int resp;
	if(socket_recv_int(fileSystemServer.socket,&resp)>0){
		log_info(logKernel, "Recibo con exito la respuesta de creacion de un archivo de FS");
	}else{
		log_info(logKernel, "Error al informar as FS que cree un archivo");
	}
	return resp;
}
void handle_cpu_abrir(t_cpu* cpu){
	//Me llega la ruta del archivo
	char* path=string_new();
	char* flags=string_new();
	if(socket_recv_string(cpu->socket,&path)>0){
		log_info(logKernel, "Recibi el path %s", path);
	}else{
		log_info(logKernel, "Error recibiendo path");
	}
	if (socket_recv_string(cpu->socket,&flags)>0){
		log_info(logKernel, "Recibo los flags '%s'",flags);
	}else{
		log_info(logKernel, "Error recibiendo los flags");
	}
	//Chequeo si el archivo existe en la tabla global de archivos
	t_gobal_fd* pointer=existeArchivoEnTablaGlobalDeArchivos(globalFileDescriptors->list,path);
	int descriptorToCPU;
	if (pointer!=NULL){ //Existe
		pointer->open++;
		t_fd* fd=(t_fd*)malloc(sizeof(t_fd));
		strcpy(fd->flags,flags);
		fd->value++;
		descriptorToCPU=fd->value;
		fd->global=pointer;
		list_add(cpu->program->fileDescriptors,fd);
	}else{   //No existe, la agrego
		int respuesta= filesystem_validar(path,cpu);
		if (respuesta==1){
			//Agrego a la tabla global una entrada
			t_gobal_fd* newFD=(t_gobal_fd*)malloc(sizeof(t_gobal_fd));
			newFD->open=1;
			strcpy(newFD->path,path);
			list_add(globalFileDescriptors->list,newFD);

			//Agrego a la tabla del proceso
			t_fd* newFD_to_file=(t_fd*)malloc(sizeof(t_fd));
			strcpy(newFD_to_file->flags,flags);
			newFD_to_file->value++;
			descriptorToCPU=newFD_to_file->value;
			newFD_to_file->global=newFD;
			list_add(cpu->program->fileDescriptors,newFD_to_file);
		}else{
			//Verifico si tengo permiso de creacion
			if(string_contains(flags,string_from_format("%c",'c'))){
				int creation=filesystem_create(flags,path);
				if (creation==1){
					log_info(logKernel, "El archivo fue creado con exito");
				}else{
					log_info(logKernel, "Error creando el archivo");
				}
			}
		}
	}
	socket_send_int(cpu->socket,descriptorToCPU);
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


