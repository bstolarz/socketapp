/*
 ============================================================================
 Name        : Console.c
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

void leerArchivoConfiguracion(char* path){

	t_config* config=config_create(path);
	t_console* console=malloc(sizeof(t_console));
	console->ip_kernel=config_get_string_value(config,"IP_KERNEL");
	console->puerto_kernel=config_get_int_value(config,"PUERTO_KERNEL");
	printf("La ip del kernel es: %s\n",console->ip_kernel);
	printf("El puerto del kernel es: %d\n",console->puerto_kernel);
}

int main(int arg, char* argv[]) {
	puts("!!!Hello World!!!"); /* prints !!!Hello World!!! */
	puts("It is working now");
	if(arg!=2){
		printf("Te falta poner el path! %d\n", arg);
		return 1;
	}
	leerArchivoConfiguracion(argv[1]);
	//leerArchivoConfiguracion("/home/utnso/git/tp-2017-1c-SocketApp/console");

	return EXIT_SUCCESS;
}
