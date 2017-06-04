#include <string.h>
#include "hash_function.h"
#include "../commons/declarations.h"

const size_t PIDPageCharCount = (sizeof(int) + sizeof(size_t)) / sizeof(char);
unsigned char PIDPageChars[(sizeof(int) + sizeof(size_t)) / sizeof(char)];

unsigned char* PIDPageToChar(int PID, size_t page)
{
	memcpy(PIDPageChars, &PID, sizeof(int));
	memcpy(PIDPageChars + sizeof(int), &page, sizeof(size_t));

	//printf("PID: %d, page: %d, chars: %s\n", PID, page, PIDPageChars);
	return PIDPageChars;
}

size_t xor_hash(int PID, size_t page)
{
	unsigned char *p = PIDPageToChar(PID, page);
	size_t hash = 0;
	size_t i;

	for (i = 0; i < PIDPageCharCount; i++)
	{
		hash ^= p[i];
	}

	return hash;
}

size_t rot_hash(int PID, size_t page)
{
	unsigned char *p = PIDPageToChar(PID, page);
	size_t hash = 0;
	size_t i;

	for (i = 0; i < PIDPageCharCount; i++)
	{
		hash = (hash << 4) ^ (hash >> 28) ^ p[i];
	}

	return hash;
}

size_t bernstein_hash(int PID, size_t page)
{
	unsigned char *p = PIDPageToChar(PID, page);
	size_t hash = 0;
	size_t i;

    for (i = 0; i < PIDPageCharCount; i++)
    {
    	hash = 33 * hash ^ p[i];
    }

    return hash;
}

size_t ayudante_hash(int PID, size_t page)
{
	return ((size_t) PID * proccessPageCount + page * configMemory->frameSize) /
			proccessPageCount;
}
