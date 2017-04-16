#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <commons/log.h>
#include <commons/string.h>
#include "../commons/structures.h"
#include "../commons/declarations.h"

t_log* log_create_file() {
	remove("logMemory");

	t_log* logs = log_create("logMemory", "Memory", false, LOG_LEVEL_DEBUG);

	if (logs == NULL) {
		printf("No se pudo generar el archivo de logueo.\n");
		return NULL;
	};


	log_info(logs, "Archivo de logueo inicializado.");

	return logs;
}

void log_config(){
	log_info(logMemory,"PUERTO: %s", configMemory->portNumStr);
	log_info(logMemory,"MARCOS: %i", configMemory->frameCount);
	log_info(logMemory,"MARCO_SIZE: %i", configMemory->frameSize);
	log_info(logMemory,"ENTRADAS_CACHE: %i", configMemory->cacheEntryCount);
	log_info(logMemory,"CACHE_X_PROC: %i", configMemory->cachePerProccess);
	log_info(logMemory,"REEMPLAZO_CACHE: %s", configMemory->cacheReplaceStrategy);
	log_info(logMemory,"RETARDO_MEMORIA: %i", configMemory->responseDelay);
}
