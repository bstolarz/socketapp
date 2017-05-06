#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "../libSockets/client.h"
#include "../libSockets/send.h"
#include "../libSockets/recv.h"
#include "../commons/structures.h"
#include "../commons/declarations.h"

#ifndef FUNCTIONS_INSTRUCTION_CYCLE_H_
#define FUNCTIONS_INSTRUCTION_CYCLE_H_

int cycle_interruption_handler();
int cycle_still_burst();
char * cycle_fetch(t_intructions* currentInstruction);

#endif /* FUNCTIONS_INSTRUCTION_CYCLE_H_ */
