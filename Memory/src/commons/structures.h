#include <pthread.h>
#include <commons/collections/list.h>

#ifndef COMMONS_STRUCTURES_H_
#define COMMONS_STRUCTURES_H_

typedef struct {

	int PID;
	int page;

} t_pageTableEntry;

typedef struct {

	char* portNumStr;
	int frameCount;
	int frameSize;
	int cacheEntryCount;
	int cachePerProccess;
	char* cacheReplaceStrategy;
	int responseDelay;

} t_memory;

typedef struct{
	int socket;
	char* command;
	int nbytes;
}t_socket_thread_arg;

typedef struct{
	pthread_mutex_t mutex;
	t_list * list;
}t_queue;


#endif /* COMMONS_STRUCTURES_H_ */
