/*
 ============================================================================
 Name        : CPU.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include "commons/structures.h"

void leerArchivoConfiguracionKernel(char* path){

	t_config* config=config_create(path);
	t_kernel* kernel=malloc(sizeof(t_kernel));
	kernel->ip_kernel=config_get_string_value(config,"IP_KERNEL");
	kernel->puerto_kernel=config_get_int_value(config,"PUERTO_KERNEL");
	printf("La ip del Kernel es: %s\n",kernel->ip_kernel);
	printf("El puerto del Kernel es: %d\n", kernel->puerto_kernel);
}

void leerArchivoConfiguracionMemoria(char* path){

	t_config* config=config_create(path);
	t_memory* memory=malloc(sizeof(t_memory));
	memory->ip_memory=config_get_string_value(config,"IP_MEMORY");
	memory->puerto_memory=config_get_int_value(config,"PUERTO_MEMORY");
	printf("La ip de la Memoria es: %s\n",memory->ip_memory);
	printf("El puerto de la Memoria es: %d\n",memory->puerto_memory);
}

int main(int arg, char* argv[]) {
	puts("!!!Hello World!!!");
	puts("It is working now");
	if(arg!=3){
		printf("Te falta poner algun path! %d\n", arg);
		return 1;
	}

	leerArchivoConfiguracionKernel(argv[1]);
	leerArchivoConfiguracionMemoria(argv[2]);


	return EXIT_SUCCESS;
}

