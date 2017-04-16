#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <commons/log.h>
#include <commons/string.h>
#include "../commons/structures.h"
#include "../commons/declarations.h"

t_log* log_create_file() {
	remove("logKernel");

	t_log* logs = log_create("logKernel", "Console", false, LOG_LEVEL_DEBUG);

	if (logs == NULL) {
		printf("No se pudo generar el archivo de logueo.\n");
		return NULL;
	};


	log_info(logs, "Archivo de logueo inicializado.");

	return logs;
}

void log_config(){
	log_info(logKernel,"Algoritmo: %s", configKernel->algoritmo);
	log_info(logKernel,"Grado Multiprog: %d", "Grado multip: %d\n",configKernel->grado_multiprog);
	log_info(logKernel,"IP FS: %s", configKernel->ip_fs);
	log_info(logKernel,"IP MEMORIA: %s", configKernel->ip_memoria);
	log_info(logKernel,"PUERTO CPU: %s", configKernel->puerto_cpu);
	log_info(logKernel,"PUERTO FS: %s", configKernel->puerto_fs);
	log_info(logKernel,"PUERTO MEMORIA: %s", configKernel->puerto_memoria);
	log_info(logKernel,"PUERTO PROG: %s", configKernel->puerto_prog);
	log_info(logKernel,"QUANTUM: %d", configKernel->quantum);
	log_info(logKernel,"QUANTUM SLEEP: %d", configKernel->quantum_sleep);
	log_info(logKernel,"STACK SIZE: %d", configKernel->stack_size);

	void _printSemaforo(t_semaforo* semaforo){
		log_info(logKernel, "Semaforo: %s, Valor: %i", semaforo->id, semaforo->initialValue);
	}
	list_iterate(configKernel->semaforos, (void*)_printSemaforo);

	void _printShareVar(t_sharedVar* sharevar){
		log_info(logKernel, "Share var: %s", sharevar->nombre);
	}
	list_iterate(configKernel->shared_vars, (void*)_printShareVar);
}
