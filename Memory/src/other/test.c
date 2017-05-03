#include <stdlib.h>
#include "test.h"
#include "../commons/structures.h"
#include "../commons/declarations.h"
#include "../functions/memory.h"

void test_cpu_connection()
{
	//Levanto el archivo  del programa
	char* code = 0;
	FILE *f = fopen("../programas-ejemplo/facil.ansisop", "rb");
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
