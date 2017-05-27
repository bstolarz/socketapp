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
#include "../../interface/memory.h"
void get_filename_with_filedescriptor(t_cpu* cpu, t_descriptor_archivo _fd, char* path){
	int tam=list_size(cpu->program->fileDescriptors);
	int i;
	for(i=0;i!=tam;i++){
		t_fd* fd=(t_fd*)list_get(cpu->program->fileDescriptors,i);
		if(fd->value==_fd){
			strcpy(path,fd->global->path);
		}
	}
}
int get_cursor_of_file(t_descriptor_archivo d, t_cpu* cpu, char* path){
	int tamanio=list_size(cpu->program->fileDescriptors);
	int i;
	int cursor=0;
	for (i=0;i!=tamanio;i++){
		t_fd* fd=(t_fd*)list_get(cpu->program->fileDescriptors,i);
		if (fd->global->path==path){
			if(string_contains(fd->flags,string_from_format("%c",'r'))){
				cursor=fd->cursor;
			}
		}
	}
	return cursor;
}
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
		return (strcmp(sharedVariable,var->nombre)==0);
	}
	t_sharedVar* sv = (t_sharedVar*)list_find(configKernel->shared_vars,(void*)_is_the_variable);

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
	}else{
		log_info(logKernel, "Nombre de variable compartida: %s", sharedVariable);
	}

	//Obtengo el valor de la shared variable
	int value = 0;
	if (socket_recv_int(cpu->socket,&value)<=0){
		log_info(logKernel,"No se obtuvo el valor de la shared variable de %d\n", cpu->socket);
		return;
	}else{
		log_info(logKernel, "Valor a setear a %s: %d", sharedVariable, value);
	}

	//Busco la shared variable
	int _is_the_variable(t_sharedVar* var){
		return (strcmp(sharedVariable,var->nombre)==0);
	}
	t_sharedVar* sv = (t_sharedVar*)list_find(configKernel->shared_vars,(void*)_is_the_variable);

	//Verifico que exista
	if(sv == NULL){
		log_info(logKernel,"La shared variable '%s' que solicito %d no existe.\n", sv->nombre, cpu->socket);
		if(socket_send_string(cpu->socket, "Failure")<=0){
			log_info(logKernel,"No se pudo informar el estado a %d\n", sv->nombre, cpu->socket);
		}else{
			log_info(logKernel, "Notifique correctamente que no existe la variable compartida");
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
	t_program* program = cpu->program;

	//Obtengo el tamaño a alocar
	int size = 0;
	if (socket_recv_int(cpu->socket,&size)<=0){
		log_info(logKernel,"No se obtuvo el size a alocar de %d\n", cpu->socket);
		return;
	}

	int puntero = memory_dynamic_alloc(program, size);
	if(puntero > 0 ){
		if (socket_recv_int(cpu->socket,&puntero)<=0){
			log_info(logKernel,"No se pudo enviar el puntero de %d\n", cpu->socket);
			return;
		}
	}else{
		//TODO error handler
	}
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
		log_info(logKernel, "Error al informar al FS que cree un archivo");
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
			newFD_to_file->cursor=0;
			log_info(logKernel, "Entrada en tabla de archivos del proceso:");
			log_info(logKernel, "FD: %d|Flags: %s|Cursor: %d",newFD_to_file->value, newFD_to_file->flags,newFD_to_file->cursor);
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
	free(path);
	free(flags);
	socket_send_int(cpu->socket,descriptorToCPU);
}
int delete_file_from_global_file_table(t_descriptor_archivo d, t_cpu* cpu){
	int tam=list_size(cpu->program->fileDescriptors);
	int i;
	int result=0;
	for(i=0;i!=tam;i++){
		t_fd* fd=(t_fd*)list_get(cpu->program->fileDescriptors,i);
		if(fd->global==NULL){
			result=1;
		}
	}
	return result;
}
int program_has_permission_to_write(t_cpu* cpu,t_descriptor_archivo d){
	int tam=list_size(cpu->program->fileDescriptors);
	int i;
	int permission=0;
	for(i=0;i!=tam;i++){
		t_fd* fd=(t_fd*)list_get(cpu->program->fileDescriptors,i);
		if (string_contains(fd->flags,string_from_format("%c",'w'))){
			permission=1;
		}
	}
	return permission;
}
int program_has_permission_to_delete(t_cpu* cpu,t_descriptor_archivo d){
	int tam=list_size(cpu->program->fileDescriptors);
	int i;
	int result=0;
	for(i=0;i!=tam;i++){
		t_fd* fd=(t_fd*)list_get(cpu->program->fileDescriptors,i);
		if(fd->value==d){
			if(string_contains(fd->flags,string_from_format("%c",'w'))){
				result=1;
			}
		}
	}
	return result;
}
void filesystem_delete(){
	if(socket_send_string(fileSystemServer.socket,"BORRAR")>0){
		log_info(logKernel, "Envio correctamente a FS que quiero borrar");
	}else{
		log_info(logKernel, "Error al enviar a FS que quiero borrar");
	}
}
void handle_cpu_borrar(t_cpu* cpu){
	//Recibo el file descriptor del archivo que CPU quiere borrar
	int dAux;
	t_descriptor_archivo d;
	if (socket_recv_int(cpu->socket,&dAux)>0){
		d=(t_descriptor_archivo)dAux;
		log_info(logKernel, "CPU quiere borrar el archivo con file descriptor %d",d);
		if(program_has_permission_to_delete(cpu,d)){
			int resultado=delete_file_from_global_file_table(d, cpu);
			if(resultado==1){
				//Comunico a FS que borre el archivo
				filesystem_delete();
				//Recibo de FS la respuesta al indicarle que borre el archivo
				int answer;
				if(socket_recv_int(fileSystemServer.socket,&answer)>0){
					if (answer==1){
						//Le digo a CPU que el archivo fue borrado con exito
						if(socket_send_int(cpu->socket,1)>0){
							log_info(logKernel, "Notifico a CPU que el programa %d que pedia borrar el archivo con file descriptor %d,  pudo hacerlo  con exito",cpu->program->pcb->pid,d);
						}else{
							log_info(logKernel, "Errro al notificar a CPU que el programa %d que pedia borrar el archivo con file descriptor %d,  pudo hacerlo  con exito",cpu->program->pcb->pid,d);
						}
					}else{
						//No se pudo borrar el archivo fisico
						if(socket_send_int(cpu->socket,0)>0){
							log_info(logKernel, "Notifico a CPU que el programa %d que pedia borrar el archivo con file descriptor %d, no pudo hacerlo",cpu->program->pcb->pid,d);
						}else{
							log_info(logKernel, "Errro al notificar a CPU que el programa %d que pedia borrar el archivo con file descriptor %d,  no puedo hacerlo",cpu->program->pcb->pid,d);
						}
					}
				}
			}
		}
	}else{
		log_info(logKernel, "Error recibiendo el file descriptor del archivo");
	}

}
int process_had_opened_file(t_cpu* cpu,t_descriptor_archivo d){
	int size=(int)list_size(cpu->program->fileDescriptors);
	int i;
	int exists=-1;
	for(i=0;i!=size;i++){
		t_fd* fd=(t_fd*)list_get(cpu->program->fileDescriptors,i);
		if (fd->value==d){
			exists=i;
			//Decremento la cantidad de veces abierto el archivo en la tabla global de archivos
			fd->global->open--;
		}
	}
	return exists;
}
void filesystem_close(){
	if(socket_send_string(fileSystemServer.socket,"CERRAR")>0){
		log_info(logKernel, "Envio correctamente a FS que quiero borrar");
	}else{
		log_info(logKernel, "Error al enviar a FS que quiero borrar");
	}
}
void handle_cpu_cerrar(t_cpu* cpu){
	//Recibo el file descriptor
	int dAux;
	t_descriptor_archivo d;
	if(socket_recv_int(cpu->socket,&dAux)>0){
		d=(t_descriptor_archivo)dAux;
		int position=process_had_opened_file(cpu,d);
		if(position!=-1){
			//Borro de la tabla de archivos por proceso la entrada correspondiente al file descriptor que recibi de CPU
			list_remove(cpu->program->fileDescriptors,position);
			log_info(logKernel, "Se borro de la tabla de archivos por proceso el indice %d que tenía al file descriptor %d",position, d);
			//Le digo a FS que cierre el archivo
			filesystem_close();
			if(socket_send_int(cpu->socket,1)>0){
				log_info(logKernel, "Notifico al programa con pid: %d que se cerro el archivo con exito", cpu->program->pcb->pid);
			}else{
				log_info(logKernel, "Error al notificar al programa con pid: %d que se cerro el archivo con exito", cpu->program->pcb->pid);
			}
		}else{
			if(socket_send_int(cpu->socket,-ENOENT)>0){
				log_info(logKernel, "Notifico al programa con pid: %d que se cerro el archivo con exito", cpu->program->pcb->pid);
			}else{
				log_info(logKernel, "Error al notificar al programa con pid: %d que se cerro el archivo con exito", cpu->program->pcb->pid);
			}
		}
	}
}

void handle_cpu_mover_cursor(t_cpu* cpu){
	//Recibo el descriptor de archivo
	int FD;
	if (socket_recv_int(cpu->socket,&FD)<=0){
			log_info(logKernel, "No se pudo obtener el FD de: %i\n", cpu->socket);
			return;
	}
	//Recibo la cantidad de bytes a moverme
	int bytesToMove;
	if (socket_recv_int(cpu->socket,&bytesToMove)<=0){
		log_info(logKernel, "No se pudo obtener el offset de: %i\n", cpu->socket);
		return;
	}

}
void filesystem_escribir(char* path, int offset, int size){
	if(socket_send_string(fileSystemServer.socket,"GUARDARDATOS")>0){
		log_info(logKernel, "Envio correctamente a FS que quiero escribir el archivo '%s'",path);
	}else{
		log_info(logKernel, "Error al enviar a FS que quiero escribir el archivo '%s'",path);
	}
	if(socket_send_string(fileSystemServer.socket,path)>0){
		log_info(logKernel, "Envio correctamente a FS la path '%s' que quiero escribir",path);
	}else{
		log_info(logKernel, "Error al enviar a FS la path '%s' que quiero escribir",path);
	}
	if(socket_send_int(fileSystemServer.socket,offset)>0){
		log_info(logKernel, "Envio correctamente a FS el offset: %d",offset);
	}else{
		log_info(logKernel, "Error al enviar a FS el offset: %d", offset);
	}
	if(socket_send_int(fileSystemServer.socket,size)>0){
		log_info(logKernel, "Envio correctamente a FS el tamanio a escribir: %d",size);
	}else{
		log_info(logKernel, "Error al enviar a FS el tamanio a escribir: %d", size);
	}


}
void handle_cpu_escribir(t_cpu* cpu){
	printf("entramos a escribir\n");
	int FD = 0;
	char* path=string_new();
	//Recibo de CPU el descriptor de archivo
	if (socket_recv_int(cpu->socket,&FD)<=0){
		log_info(logKernel, "No se pudo obtener el FD de: %i\n", cpu->socket);
		return;
	}

	//Recibo de CPU la informacion con su tamanio
	char* buffer = string_new();
	int nbytes=0;
	if ((nbytes = socket_recv(cpu->socket, (void**)&buffer, 1))<=0){
		log_info(logKernel, "No se pudo obtener el buffer de: %i\n", cpu->socket);
		return;
	}

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
	}//SE PIDE ESCRIBIR EN UN ARCHIVO

	else{
		//Verifico que tenga los permisos
		if(program_has_permission_to_write(cpu,FD)){
			log_info(logKernel,"El programa con pid: %d tiene permisos para escribir en el archivo con file descriptor: %d", cpu->program->pcb->pid, FD);
			//Informo a FS que quiero escribir
			get_filename_with_filedescriptor(cpu,FD,path);
			int cursorToFS=get_cursor_of_file(FD,cpu,path);
			filesystem_escribir(path, cursorToFS, nbytes);
			int respuestaFromFS;
			if(socket_recv_int(fileSystemServer.socket,&respuestaFromFS)>0){
				if(respuestaFromFS>0){
					log_info(logKernel, "Se pudo escribir con exito");
				}else{
					log_info(logKernel, "Error al escribir en FS");
				}
			}else{
				log_info(logKernel, "Error recibiendo respuesta del FS al escribir archivo");
			}
		}else{
			log_info(logKernel,"El programa con pid: %d NO tiene permisos para escribir en el archivo con file descriptor: %d", cpu->program->pcb->pid, FD);
			//Le nofitico a CPU el error de escritura por permisos
			if(socket_send_int(cpu->socket,-4)>0){
				log_info(logKernel, "Informo correctamente a CPU que el programa con pid %d NO puede escribir archivos", cpu->program->pcb->pid);
			}else{
				log_info(logKernel, "Error informando a CPU que el programa con pid %d NO puede escribir archivos", cpu->program->pcb->pid);
			}
		}

	}
	free(path);
}
int get_permission_on_file(t_descriptor_archivo d, t_cpu* cpu, char* path){
	int tamanio=list_size(cpu->program->fileDescriptors);
	int i;
	int permiso=0;
	for (i=0;i!=tamanio;i++){
		t_fd* fd=(t_fd*)list_get(cpu->program->fileDescriptors,i);
		if (fd->value==d){
			strcpy(path,fd->global->path);
			if(string_contains(fd->flags,string_from_format("%c",'r'))){
				permiso=1;
			}
		}
	}
	return permiso;
}



void handle_cpu_leer(t_cpu* cpu){
	t_descriptor_archivo descriptor;
	int d;
	int dondeGuardarLoLeido;
	int tamanioALeer;

	//Recibo el file descriptor
	if(socket_recv_int(cpu->socket,&d)>0){
		log_info(logKernel, "Recibi correctamente el file descriptor: %d",d);
	}else{
		log_info(logKernel, "Error recibiendo el file descriptor del programa %d que esta en la CPU [socket: %d]", cpu->program->pcb->pid,cpu->socket);
	}
	descriptor=(t_descriptor_archivo)d;
	char* path=string_new();
	//Busco el path
	get_filename_with_filedescriptor(cpu,descriptor,path);
	//Recibo el tamanio a leer
	if(socket_recv_int(cpu->socket,&tamanioALeer)>0){
		log_info(logKernel, "CPU necesita leer %d bytes",tamanioALeer);
	}else{
		log_info(logKernel, "Error al recibir el tamanio de lo que se quiere leer");
	}

	//Reviso los permisos de lectura
	int puedeLeer=get_permission_on_file(descriptor, cpu,path);

	//Verifico si se puede leer y en ese caso le pido a FS leer el archivo
	if(puedeLeer){
		//PUEDE LEER
		if(socket_recv_int(cpu->socket,&dondeGuardarLoLeido)>0){
			log_info(logKernel, "CPU requiere que se guarde la info en el puntero %d",dondeGuardarLoLeido);
			//Le pido a FS leer el archivo
			int cursorToFS=get_cursor_of_file(descriptor,cpu,path);
			filesystem_leer(path,cursorToFS,tamanioALeer);
			//Recibo la respuesta de FS de la lectura efectuada
			int resp;
			if (socket_recv_int(fileSystemServer.socket,&resp)>0){
				log_info(logKernel, "Recibo la respuesta del FS de lectura de '%s': %d", path, resp);
				if(resp==1){
					//Informo a CPU que se leyo con exito
					if (socket_send_int(cpu->socket,resp)>0){
						log_info(logKernel, "Se informo correctamente a la CPU [socket: %d] de la lectura EXITOSA del archivo '%s' con file descriptor: %d", cpu->socket,path, d);
					}else{
						log_info(logKernel, "Error al informar a la CPU [socket: %d] de la lectura  EXITOSA del archivo '%s' con file descriptor: %d", cpu->socket,path, d);
					}
				}
			}
		}else{
			log_info(logKernel, "Error al recibir el puntero donde se quiere guardar lo leido");
		}
	}// NO PUEDE LEER
	else
	{
		if(socket_send_int(cpu->socket,-3)>0){
			log_info(logKernel, "Le informo al CPU que el programa con pid %d no tiene permisos de lectura", cpu->program->pcb->pid);
		}else{
			log_info(logKernel, "Error al informar al CPU que el programa con pid %d no tiene permisos de lectura", cpu->program->pcb->pid);
		}
	}
}


