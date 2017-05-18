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

#include "../../libSockets/send.h"
#include "../../libSockets/recv.h"

#include "../../commons/structures.h"
#include "../../commons/declarations.h"

#include "../../functions/dispatcher.h"

#ifndef THREADS_CPU_HANDLER_H_
#define THREADS_CPU_HANDLER_H_

void handle_cpu_get_shared_variable(t_cpu* cpu);
void handle_cpu_set_shared_variable(t_cpu* cpu);
void handle_cpu_wait(t_cpu* cpu);
void handle_cpu_signal(t_cpu* cpu);
void handle_cpu_alocar(t_cpu* cpu);
void handle_cpu_liberar(t_cpu* cpu);
void handle_cpu_abrir(t_cpu* cpu);
void handle_cpu_borrar(t_cpu* cpu);
void handle_cpu_cerrar(t_cpu* cpu);
void handle_cpu_mover_cursor(t_cpu* cpu);
void handle_cpu_escribir(t_cpu* cpu);
void handle_cpu_leer(t_cpu* cpu);
void handle_cpu_imprimir_valor(t_cpu* cpu);
void handle_cpu_imprimir_literal(t_cpu* cpu);

#endif /* THREADS_CPU_HANDLER_H_ */
