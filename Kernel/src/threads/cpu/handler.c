#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
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
#include "../../functions/file_descriptor.h"

#include "../../interface/memory.h"
#include "../../interface/filesystem.h"

#include <parser/parser.h>


#include "../../commons/error_codes.h"



void handle_new_cpu(int socket){
	t_cpu* cpu = malloc(sizeof(t_cpu));
	cpu->socket = socket;
	cpu->disconnected = 0;
	list_add(queueCPUs->list,cpu);
	cpu->program = planificar();
	if(cpu->program != NULL){
		cpu_send_pcb(cpu);
	}
}

void handle_interruption(t_cpu * cpu){
	if(socket_send_int(cpu->socket, cpu->program->interruptionCode)<=0){
		log_warning(logKernel, "[handle_interruption/interruption] CPU desconectado");
		cpu->disconnected = 1;
		return;
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

	// TODO: testear
	usleep(configKernel->quantum_sleep /* in ms */ * 1000 /* in microsecs */);

	if(socket_send_int(cpu->socket, burst)<=0){
		log_warning(logKernel, "[handle_still_burst/burst] CPU desconectado");
		cpu->disconnected = 1;
		return;
	}
}

void handle_end_burst(t_cpu* cpu){
	t_program* program = cpu->program;

	cpu->program = NULL;

	program->pcb = cpu_recv_pcb(cpu);
	program->quantum = 0;
	program->stats.rafagas++;
	int termino = 0;
	if(socket_recv_int(cpu->socket, &termino)<=0){
		log_warning(logKernel, "[handle_end_burst/termino] CPU desconectado");
		cpu->disconnected = 1;
		return;
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
	cpu->program->stats.syscallEjecutadas++;
	cpu->program->stats.syscallPrivilegiadas++;
	//Obtengo el nombre de la shared variable
	char* sharedVariable;
	if (socket_recv_string(cpu->socket,&sharedVariable)<=0){
		log_warning(logKernel, "[handle_cpu_get_shared_variable/shared_variable] CPU desconectado");
		cpu->disconnected = 1;
		return;
	}

	//Busco la shared variable
	int _is_the_variable(t_sharedVar* var){
		return (strcmp(sharedVariable,var->nombre)==0);
	}
	t_sharedVar* sv = (t_sharedVar*)list_find(configKernel->shared_vars,(void*)_is_the_variable);

	//Verifico que exista
	if(sv == NULL){
		cpu->program->interruptionCode = ERROR_SHARED_VAR_DOES_NOT_EXIST;
		if(socket_send_string(cpu->socket, "Failure")<=0){
			log_warning(logKernel, "[handle_cpu_get_shared_variable/Failure] CPU desconectado");
			cpu->disconnected = 1;
			return;
		}
		return;
	}

	if(socket_send_string(cpu->socket, "Success")<=0){
		log_warning(logKernel, "[handle_cpu_get_shared_variable/Success] CPU desconectado");
		cpu->disconnected = 1;
		return;
	}

	//Envio el valor de la shared variable
	pthread_mutex_lock(&sv->mutex);
	if(socket_send_int(cpu->socket, sv->value)<=0){
		log_warning(logKernel, "[handle_cpu_get_shared_variable/Resultado] CPU desconectado");
		cpu->disconnected = 1;
		return;
	}
	pthread_mutex_unlock(&sv->mutex);

}

void handle_cpu_set_shared_variable(t_cpu* cpu){
	cpu->program->stats.syscallEjecutadas++;
	cpu->program->stats.syscallPrivilegiadas++;
	//Obtengo el nombre de la shared variable

	char* sharedVariable;
	if (socket_recv_string(cpu->socket,&sharedVariable)<=0){
		log_warning(logKernel, "[handle_cpu_set_shared_variable/shared_variable] CPU desconectado");
		cpu->disconnected = 1;
		return;
	}

	//Obtengo el valor de la shared variable
	int value = 0;
	if (socket_recv_int(cpu->socket,&value)<=0){
		log_warning(logKernel, "[handle_cpu_set_shared_variable/valor] CPU desconectado");
		cpu->disconnected = 1;
		return;
	}

	//Busco la shared variable
	int _is_the_variable(t_sharedVar* var){
		return (strcmp(sharedVariable,var->nombre)==0);
	}
	t_sharedVar* sv = (t_sharedVar*)list_find(configKernel->shared_vars,(void*)_is_the_variable);

	//Verifico que exista
	if(sv == NULL){
		cpu->program->interruptionCode = ERROR_SHARED_VAR_DOES_NOT_EXIST;
		if(socket_send_string(cpu->socket, "Failure")<=0){
			log_warning(logKernel, "[handle_cpu_set_shared_variable/Failure] CPU desconectado");
			cpu->disconnected = 1;
			return;
		}
		return;
	}

	//Envio el valor de la shared variable
	pthread_mutex_lock(&sv->mutex);
	sv->value = value;
	pthread_mutex_unlock(&sv->mutex);

	if(socket_send_string(cpu->socket, "Success")<=0){
		log_warning(logKernel, "[handle_cpu_set_shared_variable/Success] CPU desconectado");
		cpu->disconnected = 1;
		return;
	}
}

void handle_cpu_wait(t_cpu* cpu){
	cpu->program->stats.syscallEjecutadas++;
	cpu->program->stats.syscallPrivilegiadas++;
	//Obtengo el nombre de la shared variable
	char* semaforo;

	if (socket_recv_string(cpu->socket,&semaforo)<=0){
		log_warning(logKernel, "[handle_cpu_wait/semaforo] CPU desconectado");
		cpu->disconnected = 1;
		return;
	}

	//Busco sem
	int _es_el_semaforo(t_semaforo* var){
		return strcmp(semaforo,var->nombre)==0;
	}
	t_semaforo* sem = list_find(configKernel->semaforos,(void*)_es_el_semaforo);

	//Verifico que exista
	if(sem == NULL){
		cpu->program->interruptionCode = ERROR_SEM_DOES_NOT_EXIST;

		if(socket_send_string(cpu->socket, "Failure")<=0){
			log_warning(logKernel, "[handle_cpu_wait/Failure] CPU desconectado");
			cpu->disconnected = 1;
			return;
		}
		return;
	}

	if(socket_send_string(cpu->socket, "Success")<=0){
		log_warning(logKernel, "[handle_cpu_wait/Success] CPU desconectado");
		cpu->disconnected = 1;
		return;
	}

	pthread_mutex_lock(&sem->mutex);

	int resp;
	sem->value -= 1;

	if(sem->value >= 0){
		resp = 1;
	}else{ // val neg -> bloquear programa
		resp = 0;
		cpu->program->waiting = 1;
		cpu->program->waitingReason = string_duplicate(semaforo);
	}

	pthread_mutex_unlock(&sem->mutex);

	if(socket_send_int(cpu->socket, resp)<=0){
		log_warning(logKernel, "[handle_cpu_wait/Resultado] CPU desconectado");
		cpu->disconnected = 1;
		return;
	}

}

void handle_cpu_signal(t_cpu* cpu){
	cpu->program->stats.syscallEjecutadas++;
	cpu->program->stats.syscallPrivilegiadas++;
	//Obtengo el nombre de la shared variable
	char* semaforo=string_new();
	if (socket_recv_string(cpu->socket,&semaforo)<=0){
		log_warning(logKernel, "[handle_cpu_signal/semaforo] CPU desconectado");
		cpu->disconnected = 1;
		return;
	}

	//Busco la shared variable
	int _es_el_semaforo(t_semaforo* var){
		return strcmp(semaforo,var->nombre) == 0;
	}
	t_semaforo* sem = list_find(configKernel->semaforos,(void*)_es_el_semaforo);

	//Verifico que exista
	if(sem == NULL){
		cpu->program->interruptionCode = ERROR_SEM_DOES_NOT_EXIST;

		if(socket_send_string(cpu->socket, "Failure")<=0){
			log_warning(logKernel, "[handle_cpu_signal/Failure] CPU desconectado");
			cpu->disconnected = 1;
			return;
		}
		return;
	}

	if(socket_send_string(cpu->socket, "Success")<=0){
		log_warning(logKernel, "[handle_cpu_signal/Success] CPU desconectado");
		cpu->disconnected = 1;
		return;
	}

	//Envio el valor de la shared variable
	pthread_mutex_lock(&sem->mutex);
	sem->value = sem->value + 1;

	program_unblock(sem);
	pthread_mutex_unlock(&sem->mutex);
}

void handle_cpu_alocar(t_cpu* cpu){
	cpu->program->stats.syscallEjecutadas++;
	cpu->program->stats.syscallPrivilegiadas++;
	//Obtengo el tamaño a alocar
	int size = 0;
	if (socket_recv_int(cpu->socket,&size)<=0){
		log_warning(logKernel, "[handle_cpu_alocar/size] CPU desconectado");
		cpu->disconnected = 1;
		return;
	}

	int puntero = memory_heap_alloc(cpu->program, size);

	if (socket_send_int(cpu->socket, puntero)<=0){
		log_warning(logKernel, "[handle_cpu_alocar/respuesta] CPU desconectado");
		cpu->disconnected = 1;
		return;
	}
	cpu->program->stats.cantCallAlloc++;
	cpu->program->stats.bytesAlloc+=size;
}

void handle_cpu_liberar(t_cpu* cpu){
	cpu->program->stats.syscallEjecutadas++;
	cpu->program->stats.syscallPrivilegiadas++;
	//Obtengo el tamaño a alocar
	int posicion = 0;
	if (socket_recv_int(cpu->socket,&posicion)<=0){
		log_warning(logKernel, "[handle_cpu_liberar] CPU desconectado");
		cpu->disconnected = 1;
		return;
	}
	div_t pos = div(posicion, pageSize);
	memory_heap_free(cpu->program, pos.quot, pos.rem);
	cpu->program->stats.cantCallFree++;
}

void handle_cpu_abrir(t_cpu* cpu){
	cpu->program->stats.syscallEjecutadas++;
	cpu->program->stats.syscallPrivilegiadas++;

	//Recibo el path
	char* path;
	if(socket_recv_string(cpu->socket,&path)<=0){
		log_warning(logKernel, "[handle_cpu_abrir/path] CPU desconectado");
		cpu->disconnected = 1;
		return;
	}

	//Recibo los permisos
	char* flags;
	if (socket_recv_string(cpu->socket,&flags)<=0){
		log_warning(logKernel, "[handle_cpu_abrir/flags] CPU desconectado");
		cpu->disconnected = 1;
		return;
	}

	// que otro proceso no entre a tratar de crear el mismo gfd
	// o se meta a buscar para remover un gfd que voy a incrementar
	pthread_mutex_lock(&globalFileDescriptors->mutex);

	t_global_fd* gFD = file_descriptor_global_get_by_path(path);
	t_fd* fd = NULL;

	if (gFD!=NULL){ //Existe
		fd = file_descriptor_create(cpu->program, gFD, flags);
	}else{//No existe
		if (filesystem_validate(path)==1) // ya existe en fs
		{
			// Creo gFd y fd
			gFD = file_descriptor_global_create(path);
			fd = file_descriptor_create(cpu->program, gFD, flags);
		}
		else
		{ //No existe el archivo. Si tengo permiso, lo creo
			if(strstr(flags, FILE_DESCRIPTOR_PERMISSION_CREATE) != NULL)
			{
				if (filesystem_create(path)==1)
				{
					gFD = file_descriptor_global_create(path);
					fd = file_descriptor_create(cpu->program, gFD, flags);
				}
				else // fs tira error
				{
					cpu->program->interruptionCode = ERROR_FS_OPERATION; // o ERROR_NO_RESOURCES
				}
			}
			else // sin permisos para crear
			{
				cpu->program->interruptionCode = ERROR_FILE_CREATE_PERMISION;
			}
		}
	}

	pthread_mutex_unlock(&globalFileDescriptors->mutex);

	free(path);
	free(flags);

	if (socket_send_int(cpu->socket, fd != NULL ? fd->value : 0)<=0){
		log_warning(logKernel, "[handle_cpu_abrir/respuesta=FD] CPU desconectado");
		cpu->disconnected = 1;
		return;
	}
}

void handle_cpu_borrar(t_cpu* cpu){
	cpu->program->stats.syscallEjecutadas++;
	cpu->program->stats.syscallPrivilegiadas++;

	//Recibo el file descriptor del archivo que CPU quiere borrar
	int nFD;
	if (socket_recv_int(cpu->socket,&nFD)<=0){
		log_warning(logKernel, "[handle_cpu_abrir/respuesta=0] CPU desconectado");
		cpu->disconnected = 1;
		return;
	}

	void onDeleteError(int interruptionCode)
	{
		cpu->program->interruptionCode = interruptionCode;
		log_error(logKernel, "[handle_cpu_borrar] %d", interruptionCode);

		if(socket_send_int(cpu->socket,0)<=0){
			log_error(logKernel, "[handle_cpu_borrar] CPU desconectado");
			cpu->disconnected = 1;
		}
	};

	//Obtengo el file descriptor
	t_fd* filedescriptor = file_descriptor_get_by_number(cpu->program, nFD);

	//Verifico que existe el FD
	if(filedescriptor == NULL){
		onDeleteError(ERROR_FD_DOES_NOT_EXIST);
		return;
	}

	// que nadie trate de buscar este gfd
	pthread_mutex_lock(&globalFileDescriptors->mutex);

	//Verifico que sea el unico que abrio el file descriptor
	if(filedescriptor->global->open>1){
		pthread_mutex_unlock(&globalFileDescriptors->mutex);
		onDeleteError(ERROR_FILE_BAD_DELETE);
		return;
	}

	if(filesystem_delete(filedescriptor->global->path) == 1){
		//Borro el global file descriptor de la lista
		bool _findGlobalFD(t_global_fd* gFD){
			return strcmp(gFD->path, filedescriptor->global->path)==0;
		}
		list_remove_by_condition(globalFileDescriptors->list, (void*)_findGlobalFD);

		// ya pueden buscarlo y ver q no existe
		pthread_mutex_unlock(&globalFileDescriptors->mutex);

		//Borro el file descriptor de la lista
		bool _findFD(t_fd* fd){
			return fd->value == filedescriptor->value;
		}
		list_remove_by_condition(cpu->program->fileDescriptors, (void*)_findFD);

		free(filedescriptor->global->path);
		free(filedescriptor->global);
		free(filedescriptor->permissions);
		free(filedescriptor);

		if(socket_send_int(cpu->socket,1)<=0){
			log_warning(logKernel, "[handle_cpu_borrar/respuesta=1] CPU desconectado");
			cpu->disconnected = 1;
		}
	}else{
		pthread_mutex_unlock(&globalFileDescriptors->mutex);
		onDeleteError(ERROR_FS_OPERATION);
	}
}

void handle_cpu_cerrar(t_cpu* cpu){
	cpu->program->stats.syscallEjecutadas++;
	cpu->program->stats.syscallPrivilegiadas++;

	// recibir datos
	// fd
	int nFD;
	if (socket_recv_int(cpu->socket,&nFD)<=0){
		log_warning(logKernel, "[handle_cpu_cerrar/nFD] CPU desconectado");
		cpu->disconnected = 1;
		return;
	}

	// buscar fd del proceso
	t_fd* filedescriptor = file_descriptor_get_by_number(cpu->program, nFD);

	//Verifico que existe el FD
	if(filedescriptor == NULL){
		cpu->program->interruptionCode = ERROR_FD_DOES_NOT_EXIST;

		if(socket_send_int(cpu->socket,0)<=0){
			log_warning(logKernel, "[handle_cpu_cerrar/resultado=0/FD=NULL] CPU desconectado");
			cpu->disconnected = 1;
		}

		return;
	}

	// bajo referencia del gfd. si no hay mas, saco de la lista
	pthread_mutex_lock(&globalFileDescriptors->mutex);
	filedescriptor->global->open -= 1;

	if(filedescriptor->global->open == 0)
	{
		bool _findGlobalFD(t_global_fd* gFD){
			return strcmp(gFD->path, filedescriptor->global->path)==0;
		}
		list_remove_and_destroy_by_condition(globalFileDescriptors->list, (void*)_findGlobalFD, file_descriptor_global_destroy);
	}

	pthread_mutex_unlock(&globalFileDescriptors->mutex);


	//Borro el fd del proceso
	bool _findFD(t_fd* fd){
		return fd->value == filedescriptor->value;
	}
	list_remove_by_condition(cpu->program->fileDescriptors, (void*)_findFD);

	free(filedescriptor->permissions);
	free(filedescriptor);

	if(socket_send_int(cpu->socket,1)<=0){
		log_warning(logKernel, "[handle_cpu_cerrar/resultado=1] CPU desconectado");
		cpu->disconnected = 1;
		return;
	}
}

void handle_cpu_mover_cursor(t_cpu* cpu){
	cpu->program->stats.syscallPrivilegiadas++;
	cpu->program->stats.syscallEjecutadas++;
	//Recibo el descriptor de archivo
	int FD;
	if (socket_recv_int(cpu->socket,&FD)<=0){
		log_warning(logKernel, "[handle_cpu_mover_cursor/FD] CPU desconectado");
		cpu->disconnected = 1;
		return;
	}

	//Recibo la cantidad de bytes a moverme
	int bytesToMove;
	if (socket_recv_int(cpu->socket,&bytesToMove)<=0){
		log_warning(logKernel, "[handle_cpu_mover_cursor/Bytes] CPU desconectado");
		cpu->disconnected = 1;
		return;
	}

	t_fd* filedescriptor = file_descriptor_get_by_number(cpu->program, FD);
	if(filedescriptor == NULL){
		cpu->program->interruptionCode = ERROR_FD_DOES_NOT_EXIST;
		return;
	}

	filedescriptor->cursor=bytesToMove;
}

void handle_cpu_escribir(t_cpu* cpu){
	cpu->program->stats.syscallEjecutadas++;
	cpu->program->stats.syscallPrivilegiadas++;

	int FD = 0;
	//Recibo de CPU el descriptor de archivo
	if (socket_recv_int(cpu->socket,&FD)<=0){
		log_info(logKernel, "No se pudo obtener el FD de: %i\n", cpu->socket);
		return;
	}

	//Recibo de CPU la informacion con su tamanio
	char* buffer;
	int nbytes=0;
	if ((nbytes = socket_recv(cpu->socket, (void**)&buffer, 1))<=0){
		log_info(logKernel, "No se pudo obtener el buffer de: %i\n", cpu->socket);
		return;
	}

	log_info(logKernel, "[escribir] fd = %i", FD);
	if(FD == DESCRIPTOR_SALIDA){
		if(buffer[nbytes] != '\0'){
			buffer = realloc(buffer, nbytes+1);
			buffer[nbytes] = '\0';
			nbytes = nbytes + 1;
		}

		// fixucho para no mandar a consola a printear si se desconecto
		if (cpu->program->interruptionCode != ERROR_CONSOLE_DISCONNECTED &&
			cpu->program->interruptionCode != ERROR_CONSOLE_FINISH_COMMAND)
		{
			if(socket_send_string(cpu->program->socket, "imprimir")<=0){
				log_info(logKernel,"No se pudo imprimir en: %i\n", cpu->program->socket);
			}

			if(socket_send_string(cpu->program->socket, buffer)<=0){
				log_info(logKernel,"No se pudo imprimir el mensaje en: %i\n", cpu->program->socket);
			}
		}
	}else{ //SE PIDE ESCRIBIR EN UN ARCHIVO
		//Verifico que tenga los permisos
		t_fd* filedescriptor = file_descriptor_get_by_number(cpu->program, FD);

		if (filedescriptor == NULL)
		{
			cpu->program->interruptionCode = ERROR_FD_DOES_NOT_EXIST;
		}
		else if (!file_descriptor_check_permission(filedescriptor, FILE_DESCRIPTOR_PERMISSION_WRITE))
		{
			cpu->program->interruptionCode = ERROR_FILE_NO_WRITE_PERMISION;
			log_info(logKernel,"El programa con pid: %d NO tiene permisos para escribir en el archivo con file descriptor: %d", cpu->program->pcb->pid, FD);
		}
		else
		{
			log_info(logKernel,"El programa con pid: %d tiene permisos para escribir en el archivo con file descriptor: %d", cpu->program->pcb->pid, FD);

			//Informo a FS que quiero escribir
			int respuestaFromFS = filesystem_write(filedescriptor->global->path, filedescriptor->cursor, buffer, nbytes);

			if(respuestaFromFS>0){
				log_info(logKernel, "Se pudo escribir con exito");
			}else{
				log_info(logKernel, "Error al escribir en FS");

				if (respuestaFromFS == -ENOSPC)
					cpu->program->interruptionCode = -ENOSPC;
				else
					cpu->program->interruptionCode = ERROR_FS_OPERATION;
			}
		}

	}
}

void handle_cpu_leer(t_cpu* cpu){
	t_descriptor_archivo descriptor;
	int d;
	int dondeGuardarLoLeido;
	int tamanioALeer;
	cpu->program->stats.syscallEjecutadas++;
	cpu->program->stats.syscallPrivilegiadas++;
	int readResult = -20;
	void* buffer = NULL;

	// file descriptor
	if(socket_recv_int(cpu->socket,&d) == -1) {
		log_error(logKernel, "Error recibiendo el file descriptor del programa %d que esta en la CPU [socket: %d]", cpu->program->pcb->pid,cpu->socket);
		cpu->disconnected = 1;
		return;
	}

	descriptor = (t_descriptor_archivo) d;

	// pos en memoria
	if(socket_recv_int(cpu->socket,&dondeGuardarLoLeido) == -1){
		log_error(logKernel, "Error al recibir el puntero donde se quiere guardar lo leido");
		cpu->disconnected = 1;
		return;
	}

	// tamanio a leer
	if(socket_recv_int(cpu->socket,&tamanioALeer) == -1){
		log_error(logKernel, "Error al recibir el tamanio de lo que se quiere leer");
		cpu->disconnected = 1;
		return;
	}

	log_debug(logKernel, "[leer] cpu me paso bien los datos (%d, %d, %d)", d, dondeGuardarLoLeido, tamanioALeer);


	t_fd* filedescriptor = file_descriptor_get_by_number(cpu->program, descriptor);

	if(filedescriptor == NULL)
	{
		cpu->program->interruptionCode = ERROR_FD_DOES_NOT_EXIST;
		readResult = ERROR_FD_DOES_NOT_EXIST;
	}
	else
	{
		//Verifico si se puede leer y en ese caso le pido a FS leer el archivo
		//Reviso los permisos de lectura
		int puedeLeer = file_descriptor_check_permission(filedescriptor, FILE_DESCRIPTOR_PERMISSION_READ);

		if(puedeLeer)
		{
			readResult = filesystem_read(filedescriptor->global->path, filedescriptor->cursor, tamanioALeer, &buffer);
		}
		else
		{
			cpu->program->interruptionCode = ERROR_FILE_NO_READ_PERMISION;
			readResult = ERROR_FILE_NO_READ_PERMISION;
		}
	}

	// guardar en memoria
	if(buffer != NULL)
	{
		int writeMemoryResult = memory_write(cpu->program, dondeGuardarLoLeido / pageSize, dondeGuardarLoLeido % pageSize, buffer, readResult);

		if (writeMemoryResult > -1)
		{
			readResult = 1; // es lo q espera cpu
		}
		else
		{
			cpu->program->interruptionCode = writeMemoryResult;
			readResult = writeMemoryResult;
		}
	}

	// mando resultado a cpu, no se si es necesario,
	// solo seria necesario si usase el error code.
	// preguntar a jonatan
	if (socket_send_int(cpu->socket, readResult) == -1)
	{
		log_error(logKernel, "Error al recibir el tamanio de lo que se quiere leer");
		cpu->disconnected = 1;
		return;
	}
}


