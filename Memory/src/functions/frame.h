#ifndef FUNCTIONS_FRAME_H_
#define FUNCTIONS_FRAME_H_

#include "../commons/structures.h"

_Bool is_frame_free(t_pageTableEntry* entry);
_Bool is_frame_occupied(t_pageTableEntry* entry);
_Bool entry_has_PID_page(t_pageTableEntry* entry, int PID, int page);

#endif /* FUNCTIONS_FRAME_H_ */
