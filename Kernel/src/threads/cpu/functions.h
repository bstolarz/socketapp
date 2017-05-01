/*
 * functions.h
 *
 *  Created on: 16/4/2017
 *      Author: utnso
 */

#ifndef THREADS_CPU_FUNCTIONS_H_
#define THREADS_CPU_FUNCTIONS_H_

void cpu_send_pcb(t_cpu* cpu);
void cpu_recv_pcb(t_cpu* cpu);
t_cpu* cpu_find(int socket);
void cpu_process_new(int socket);
void cpu_interruption(t_cpu * cpu);
void cpu_burst(t_cpu* cpu);

#endif /* THREADS_CPU_FUNCTIONS_H_ */
