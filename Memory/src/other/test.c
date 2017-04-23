#include <stdlib.h>
#include <commons/string.h>
#include "test.h"
#include "../commons/structures.h"
#include "../commons/declarations.h"
#include "../functions/memory.h"
#include "debug_console.h"
#include "pthread.h"


void test_program_init_end()
{
	int pageResult = program_init(0, 10);
	log_debug(logMemory, "pedi 50 pages. result: %d", pageResult);

	pageResult = program_init(1, 25);
	log_debug(logMemory, "pedi 25 mas. result: %d", pageResult);

	program_end(0);

	pageResult = program_init(2, 30);
	log_debug(logMemory, "y 25 mas. result: %d", pageResult);
	program_end(1);
	program_end(2);
}

void test_read_write()
{
	program_init(0, 10);

	int someInt = 1234;
	memory_write(0, 0, 0, sizeof(int), &someInt);
	int readInt;
	void* readBits = memory_read(0, 0, 0, sizeof(int));
	readInt = *((int*)(readBits));
	log_debug(logMemory, "wrote %d. read %d", someInt, readInt);

	char* str = "y si es mas grande que 128?";
	size_t strLength = string_length(str) + 1;
	memory_write(0, 0, /*offset=*/ sizeof(int), strLength, str);
	char* readStr = (char*)memory_read(0, 0, sizeof(int), strLength);
	log_debug(logMemory, "wrote %s. read %s", str, readStr);

	program_end(0);
}

void* _init_proccess(void* PIDptr)
{
	program_init(*(int*) PIDptr, 10);
	return NULL;
}

void test_threads_init()
{
	int i, threadCount = 10;

	pthread_t initThreads[threadCount];
	int pids[threadCount];
	pids[0] = 1;
	pids[1] = 2;
	pids[2] = 3;
	pids[3] = 4;
	pids[4] = 5;
	pids[5] = 6;
	pids[6] = 7;
	pids[7] = 8;
	pids[8] = 9;
	pids[9] = 10;

	for (i = 0; i != threadCount; ++i)
	{
		pthread_create(initThreads + i, NULL, _init_proccess, (void*)(pids + i));
	}

	for (i = 0; i != threadCount; ++i)
	{
		pthread_join(initThreads[i], NULL);
	}

	for (i = 0; i != threadCount; ++i)
	{
		char* proccessSizeStr = proccess_size_str(pids[i]);
		printf("%s", proccessSizeStr);
		free(proccessSizeStr);
	}
}

void init_some_programs()
{
	int pageResult = program_init(0, 10);

	pageResult = program_init(1, 10);
	int pageCount = 10;
	int i, j, varVal;

	for (i = 0; i != pageCount; ++i)
	{
		varVal = i * 1000;

		for (j = 0; j < configMemory->frameSize; j += sizeof(int))
		{
			++varVal;
			memory_write(1, i, j, sizeof(int), &varVal);
		}
	}

	program_end(0);

	pageResult = program_init(2, 20);

	for (i = 0; i != pageCount; ++i)
	{
		varVal = i * 1000;

		for (j = 0; j < configMemory->frameSize; j += sizeof(int))
		{
			++varVal;
			memory_write(2, i, j, sizeof(int), &varVal);
		}
	}
}
