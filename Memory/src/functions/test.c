#include "test.h"
#include "../commons/declarations.h"
#include "memory.h"

void test_program_init_end()
{
	int pageResult = program_init(0, 10);
	log_debug(memoryLog, "pedi 50 pages. result: %d", pageResult);

	pageResult = program_init(1, 25);
	log_debug(memoryLog, "pedi 25 mas. result: %d", pageResult);

	program_end(0);

	pageResult = program_init(2, 30);
	log_debug(memoryLog, "y 25 mas. result: %d", pageResult);
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
	log_debug(memoryLog, "wrote %d. read %d", someInt, readInt);

	char* str = "y si es mas grande que 128?";
	size_t strLength = strlen(str) + 1;
	memory_write(0, 0, /*offset=*/ sizeof(int), strLength, str);
	char* readStr = (char*)memory_read(0, 0, sizeof(int), strLength);
	log_debug(memoryLog, "wrote %s. read %s", str, readStr);

	program_end(0);
}

