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

int get_cursor_of_file(t_cpu* cpu, char* path){
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

void update_cursor_of_file(t_cpu* cpu, t_descriptor_archivo f, int c){
	int tam=list_size(cpu->program->fileDescriptors);
	int i;
	for (i=0;i!=tam;i++){
		t_fd* fd=(t_fd*)list_get(cpu->program->fileDescriptors,i);
		if (fd->value==f){
			fd->cursor=c;
		}
	}
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
