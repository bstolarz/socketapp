#include "interruption.h"
#include <stdio.h>

void print_interruption(int err){
	switch(err){
/*	-1		No se pudieron reservar recursos para ejecutar el programa
		 * 	-2		El programa intento leer un archivo que no existe
		 * 	-3		El programa intento leer un archivo sin permisos
		 * 	-4		El programa intento escribir un archivo sin permisos
		 * 	-5		Excepcion de memoria
		 * 	-6		Finalizado a travez de desconexion de consola
		 * 	-7		Finalizado a travez del comando finalizar programa de la consola
		 * 	-8		Se intento reservar mas memoria que el tamaño de una pagina
		 * 	-9		No se pueden asignar mas paginas al proceso
		 * 	-10		Se intento borrar un archivo abierto por varios procesos
		 * 	-11		File descriptor inexistente
		 * 	-12		Se intento abrir un archivo inexistente
		 * 	-13		No se pudo borrar un archivo en FS
		 * 	-14		Semaforo inexistente
		 * 	-15		Shared variable inexistente
		 * 	-16		El cpu se desconecto y dejo el programa en un estado inconsistente
		 * 	-20		Error sin definicion
		 * 	*/
	case -2:
		printf("El programa intento leer un archivo que no existe\n");
		break;
	case -3:
		printf("El programa intento leer un archivo sin permisos\n");
		break;
	case -4:
		printf("El programa intento escribir un archivo sin permisos\n");
		break;
	case -5:
		printf("Excepcion de memoria\n");
		break;
	case -6:
		printf("Finalizado a traves de desconexion de consola\n");
		break;
	case -8:
		printf("Se intento reservar mas memoria que el tamaño de una pagina\n");
		break;
	case -9:
		printf("No se pueden asignar mas paginas al proceso\n");
		break;
	case -10:
		printf("Se intento borrar un archivo abierto por varios procesos\n");
		break;
	case -11:
		printf("File descriptor inexistente\n");
		break;
	case -12:
		printf("Se intento abrir un archivo inexistente\n");
		break;
	case -13:
		printf("No se pudo borrar un archivo en FS\n");
		break;
	case -14:
		printf("Semaforo inexistente\n");
		break;
	case -15:
		printf("Variable compartida inexistente\n");
		break;
	case -16:
		printf("La CPU se desconecto y dejo al programa en un estado incosistente\n");
		break;
	case -20:
		printf("Error sin definir\n");
		break;
	}
}
