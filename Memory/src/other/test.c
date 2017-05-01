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

void* _init_proccess(void* PIDptr)
{
	program_init(*(int*) PIDptr, 10);
	return NULL;
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

void test_cpu_connection()
{
	//Levanto el archivo  del programa
	char* code = 0;
	FILE *f = fopen("../../programas-ejemplo/facil.ansisop", "rb");
	int fileSize;

	if (f)
	{
	  fseek (f, 0, SEEK_END);
	  fileSize = ftell (f);

	  fseek (f, 0, SEEK_SET);
	  code = malloc(fileSize);

	  if (code){
		fread (code, 1, fileSize, f);
	  }
	  fclose (f);

	  printf("codigo leido: %s\n", code);
	}else{
		printf("lei mal el archivo!");
		exit(EXIT_FAILURE);
	}

	int codePageCount = (fileSize / configMemory->frameSize) + 1;
	int stackPageCount = 2;

	int pageResult = program_init(0, codePageCount + stackPageCount);

	memory_write(0, 0, 0, fileSize, code);

	free(code);

}
