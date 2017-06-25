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

#include "../libSockets/send.h"
#include "../libSockets/recv.h"

#include "../commons/structures.h"
#include "../commons/declarations.h"

//Operaciones sobre file descriptors
t_fd* file_descriptor_get_by_number(t_program* program, t_descriptor_archivo nFD){
	bool _findFileDescriptor(t_fd* fd){
		return fd->value==nFD;
	}
	return (t_fd*)list_find(program->fileDescriptors, (void*)_findFileDescriptor);
}

int file_descriptor_check_permission(t_fd* fd, char* permission){
	return strstr(fd->permissions, permission) != NULL;
}

t_fd* file_descriptor_create(t_program* program, t_global_fd* gFD, char* permissions){
	t_fd* fd=(t_fd*)malloc(sizeof(t_fd));
	fd->permissions = string_duplicate(permissions);
	fd->value = program->incrementalFD;
	fd->global=gFD;
	fd->cursor = 0;

	gFD->open++;
	program->incrementalFD++;

	list_add(program->fileDescriptors,fd);

	return fd;
}


// global file descriptors
t_global_fd* file_descriptor_global_get_by_path(char* path){
	bool _findGlobalFileDescriptor(t_global_fd* fd){
		return strcmp(fd->path, path)==0;
	}

	t_global_fd* ret = (t_global_fd*)list_find(globalFileDescriptors->list, (void*)_findGlobalFileDescriptor);

	return ret;
}

t_global_fd* file_descriptor_global_create(char* path){
	t_global_fd* gFD = malloc(sizeof(t_global_fd));
	gFD->path = string_duplicate(path);
	gFD->open=0;
	list_add(globalFileDescriptors->list,gFD);

	return gFD;
}

void file_descriptor_global_destroy(void* elem){
	t_global_fd* gFD = (t_global_fd*) elem;

	free(gFD->path);
	free(gFD);
}


//Otros
t_global_fd* existeArchivoEnTablaGlobalDeArchivos(t_list * l, char* path){
	int tamanio=list_size(l);
	int i;
	t_global_fd* pointerToGlobalFile=NULL;
	for (i=0;i!=tamanio;i++){
		t_global_fd* globalFD=(t_global_fd*)list_get(l,i);
		if(strcmp(globalFD->path,path)==0){
			pointerToGlobalFile=globalFD;
		}
	}
	return pointerToGlobalFile;
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
