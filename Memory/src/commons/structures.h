#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <commons/collections/list.h>

#ifndef COMMONS_STRUCTURES_H_
#define COMMONS_STRUCTURES_H_

typedef struct {

	int PID;
	int page;
	pthread_spinlock_t lock;

} t_pageTableEntry;

typedef struct {

	char* portNumStr;
	int frameCount;
	int frameSize;
	int cacheEntryCount;
	int cachePerProccess;
	useconds_t responseDelay; // en milisegs

} t_memory;

typedef struct{
	int socket;
	char* command;
	int nbytes;
	pthread_t threadId;
}t_socket_thread_arg;

typedef struct{
	pthread_mutex_t mutex;
	t_list * list;
}t_queue;

typedef struct
{
	int PID;
	int page;
	char* content; // frameSize byte block
	clock_t lastAccess;
} t_cache_entry;


#endif /* COMMONS_STRUCTURES_H_ */
