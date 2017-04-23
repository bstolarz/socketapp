#ifndef FUNCTIONS_DEBUG_CONSOLE_H_
#define FUNCTIONS_DEBUG_CONSOLE_H_

void set_response_delay(int newResponseDelay);
char* memory_size_str();
char* proccess_size_str(int PID);
char* stringify_admin_structs();

void debug_console();

#endif /* FUNCTIONS_DEBUG_CONSOLE_H_ */
