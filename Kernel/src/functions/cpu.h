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

#include "../planner/dispatcher.h"
#include "../functions/serialization.h"
#include "../functions/program.h"

#ifndef THREADS_CPU_FUNCTIONS_H_
#define THREADS_CPU_FUNCTIONS_H_

void cpu_send_pcb(t_cpu* cpu);
t_pcb* cpu_recv_pcb(t_cpu* cpu);
void cpu_destroy_pcb(t_program* program);
t_cpu* cpu_find(int socket);
void cpu_inactive_planner(_Bool lockCPUs);

#endif /* THREADS_CPU_FUNCTIONS_H_ */
