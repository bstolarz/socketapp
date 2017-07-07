/*
 * config.c
 *
 *  Created on: 3/4/2017
 *      Author: utnso
 */
#include <stdio.h>
#include <stdlib.h>
#include <commons/collections/list.h>
#include <commons/config.h>
#include <commons/string.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>

#include "../commons/declarations.h"

// inotify para observar cambios en config
#include <unistd.h>
#include <sys/types.h>
#include <sys/inotify.h>


void config_read_semaforos(t_config* config){
	char** s=config_get_array_value(config,"SEM_IDS");
	char** vs=config_get_array_value(config,"SEM_INIT");

	configKernel->semaforos=list_create();
	while(*s!=NULL && *vs!= NULL){
		t_semaforo* sem = malloc(sizeof(t_semaforo));
		sem->nombre=string_duplicate(*s);
		sem->value=atoi(*vs);
		pthread_mutex_init(&sem->mutex, NULL);

		list_add(configKernel->semaforos, sem);

		s++;
		vs++;
	}

}
void config_read_shared_vars(t_config* config){
	char** vc=config_get_array_value(config,"SHARED_VARS");

	configKernel->shared_vars=list_create();

	while(*vc!=NULL){
		t_sharedVar* sv=malloc(sizeof(t_sharedVar));
		sv->nombre=string_duplicate(*vc);
		sv->value = 0;
		pthread_mutex_init(&sv->mutex, NULL);

		list_add(configKernel->shared_vars,sv);

		vc++;
	}
}
void config_read(char* path){
	t_config* config=config_create(path);

	configKernel->algoritmo			= string_duplicate(config_get_string_value(config,"ALGORITMO"));
	configKernel->grado_multiprog	= config_get_int_value(config,"GRADO_MULTIPROG");
	configKernel->ip_fs				= string_duplicate(config_get_string_value(config,"IP_FS"));
	configKernel->ip_memoria		= string_duplicate(config_get_string_value(config,"IP_MEMORIA"));
	configKernel->puerto_cpu		= string_duplicate(config_get_string_value(config,"PUERTO_CPU"));
	configKernel->puerto_fs			= string_duplicate(config_get_string_value(config,"PUERTO_FS"));
	configKernel->puerto_memoria	= string_duplicate(config_get_string_value(config,"PUERTO_MEMORIA"));
	configKernel->puerto_prog		= string_duplicate(config_get_string_value(config,"PUERTO_PROG"));
	configKernel->quantum			= config_get_int_value(config,"QUANTUM");
	configKernel->quantum_sleep		= config_get_int_value(config,"QUANTUM_SLEEP");
	configKernel->stack_size		= config_get_int_value(config,"STACK_SIZE");

	config_read_semaforos(config);
	config_read_shared_vars(config);

	config_destroy(config);
}


// copiado de ejemplo del repositoria sisoputnfrba

// El tamaño de un evento es igual al tamaño de la estructura de inotify
// mas el tamaño maximo de nombre de archivo que nosotros soportemos
// en este caso el tamaño de nombre maximo que vamos a manejar es de 24
// caracteres. Esto es porque la estructura inotify_event tiene un array
// sin dimension ( Ver C-Talks I - ANSI C ).
#define EVENT_SIZE  ( sizeof (struct inotify_event) + 24 )

// El tamaño del buffer es igual a la cantidad maxima de eventos simultaneos
// que quiero manejar por el tamaño de cada uno de los eventos. En este caso
// Puedo manejar hasta 1024 eventos simultaneos.
#define BUF_LEN     ( 1024 * EVENT_SIZE )

void* config_observe_changes(void* configPathAsVoid)
{
	char buffer[BUF_LEN];
	char* configPath = (char*) configPathAsVoid;

	// Al inicializar inotify este nos devuelve un descriptor de archivo
	int file_descriptor = inotify_init();
	if (file_descriptor < 0) {
		log_error(logKernel, "[config_observe_changes] error en init del fd observador");
		return NULL;
	}

	// Creamos un monitor sobre un path indicando que eventos queremos escuchar
	// El file descriptor creado por inotify,
	// es el que recibe la información sobre los eventos ocurridos
	// para leer esta información
	// el descriptor se lee como si fuera un archivo comun y corriente pero
	// la diferencia esta en que lo que leemos no es el contenido de un archivo sino
	// la información referente a los eventos ocurridos
	int watch_descriptor = inotify_add_watch(file_descriptor, configPath, IN_MODIFY | IN_DELETE);

	_Bool observeConfig = 1;

	while (observeConfig)
	{
		// aca se queda esperando
		int length = read(file_descriptor, buffer, BUF_LEN);

		if (length < 0) {
			log_error(logKernel, "[config_observe_changes] error al leer cambios en config");
			inotify_rm_watch(file_descriptor, watch_descriptor);
			close(file_descriptor);
			return NULL;
		}

		int offset = 0;

		// Luego del read buffer es un array de n posiciones donde cada posición contiene
		// un eventos ( inotify_event ) junto con el nombre de este.
		while (offset < length) {

			// El buffer es de tipo array de char, o array de bytes. Esto es porque como los
			// nombres pueden tener nombres mas cortos que 24 caracteres el tamaño va a ser menor
			// a sizeof( struct inotify_event ) + 24.
			struct inotify_event *event = (struct inotify_event *) &buffer[offset];

			// El campo "len" nos indica la longitud del tamaño del nombre
			if (event->mask & IN_MODIFY) {

				t_config* config=config_create(configPath);
				if (config_has_property(config, "QUANTUM_SLEEP"))
				{
					int prevValue = configKernel->quantum_sleep;
					int newValue = config_get_int_value(config,"QUANTUM_SLEEP");

					if (prevValue != newValue)
					{
						configKernel->quantum_sleep	= newValue;
						printf("QUANTUM_SLEEP actualizado a %d\n", configKernel->quantum_sleep);
					}
				}
				config_destroy(config);
			}
			else if (event->mask & IN_DELETE) {
				observeConfig = 0;
			}

			offset += sizeof (struct inotify_event) + event->len;
		}
	}

	inotify_rm_watch(file_descriptor, watch_descriptor);
	close(file_descriptor);

	return NULL;
}
