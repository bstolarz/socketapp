#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include "../commons/structures.h"
#include "../commons/declarations.h"
#include "config.h"
#include "../libSockets/client.h"
#include "../libSockets/send.h"
#include "../libSockets/recv.h"
#include <parser/parser.h>
#include "primitivas.h"
#include "serialization.h"
#include "log.h"
#include "memory.h"
#include "ansisop.h"
#include "instruction_cycle.h"

#ifndef FUNCTIONS_PCB_H_
#define FUNCTIONS_PCB_H_

t_pcb* recv_pcb();
void send_pcb(t_pcb* pcb);

#endif /* FUNCTIONS_PCB_H_ */
