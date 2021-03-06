#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <commons/log.h>

#include "../libSockets/server.h"
#include "../libSockets/recv.h"
#include "../libSockets/send.h"

#include "../commons/structures.h"
#include "../commons/declarations.h"

int bytes_to_pages(int byteCount){
	int fullPages = byteCount / configMemory->frameSize;

	if ((byteCount % configMemory->frameSize) > 0)
		return fullPages + 1;

	return fullPages;
}
