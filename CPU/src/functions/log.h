#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <commons/log.h>

#ifndef FUNCTIONS_LOG_H_
#define FUNCTIONS_LOG_H_

t_log* log_create_file();
void log_config();

#endif /* FUNCTIONS_LOG_H_ */
