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

t_fd* file_descriptor_get_by_path(t_program* program, char* path){
	bool _findFileDescriptor(t_fd* fd){
		return strcmp(fd->global->path, path)==0;
	}
	return (t_fd*)list_find(program->fileDescriptors, (void*)_findFileDescriptor);
}
t_fd* file_descriptor_get_by_number(t_program* program, t_descriptor_archivo nFD){
	bool _findFileDescriptor(t_fd* fd){
		return fd->value==nFD;
	}
	return (t_fd*)list_find(program->fileDescriptors, (void*)_findFileDescriptor);
}

int file_descriptor_check_permission(t_fd* fd, char* permission){
	return strstr(fd->permissions, permission) != NULL;
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
