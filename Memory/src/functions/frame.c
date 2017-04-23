#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../commons/structures.h"
#include "../commons/declarations.h"

_Bool is_frame_free(t_pageTableEntry* entry) {
	return entry->PID == -1;
}

_Bool is_frame_occupied(t_pageTableEntry* entry) {
	return entry->PID != -1;
}
