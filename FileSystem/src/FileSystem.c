/*
 ============================================================================
 Name        : FileSystem.c
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
	t_kernel* kernel=malloc(sizeof(kernel));
	kernel->ip_kernel=config_get_string_value(config,"IP_KERNEL");
	kernel->puerto_kernel=config_get_int_value(config,"PUERTO_KERNEL");
	printf("La ip del kernel es: %s\n", kernel->ip_kernel);
	printf("El puerto del kernel es: %d\n",kernel->puerto_kernel);
}

int main(int arg, char* argv[]) {
	puts("!!!Hello World!!!");
	puts("It is working now");
	if(arg!=2){
		printf("Te falta poner el path! %d\n", arg);
		return 1;
	}
	leerArchivoConfiguracionKernel(argv[1]);


	return EXIT_SUCCESS;
}
