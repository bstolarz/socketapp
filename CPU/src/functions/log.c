#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <commons/log.h>
#include <commons/string.h>
#include "../commons/structures.h"
#include "../commons/declarations.h"

t_log* log_create_file() {
	char pid[10];
	snprintf(pid, 10,"%d",(int)getpid());

	char* path = string_new();
	string_append(&path, "logCPU");
	string_append(&path, pid);

	remove(path);

	t_log* logs = log_create(path, "CPU", false, LOG_LEVEL_DEBUG);

	if (logs == NULL) {
		printf("No se pudo generar el archivo de logueo.\n");
		return NULL;
	};


	log_info(logs, "Archivo de logueo inicializado.");

	return logs;
}

void log_config(){
	log_info(logCPU,"IP KERNEL: %s",configCPU->ip_kernel);
	log_info(logCPU,"PUERTO KERNEL: %s",configCPU->puerto_kernel);
	log_info(logCPU,"IP MEMORY: %s",configCPU->ip_memory);
	log_info(logCPU,"PUERTO MEMORY: %s",configCPU->puerto_memory);
}
