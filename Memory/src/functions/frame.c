#include "frame.h"
#include "../commons/structures.h"

_Bool is_frame_free(t_pageTableEntry* entry) { return entry->PID == -1; }
_Bool is_frame_occupied(t_pageTableEntry* entry) { return entry->PID != -1; }
