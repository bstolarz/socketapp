#ifndef FUNCTIONS_FRAME_H_
#define FUNCTIONS_FRAME_H_

#include "../commons/structures.h"

_Bool is_frame_free(t_pageTableEntry* entry);
_Bool is_frame_occupied(t_pageTableEntry* entry);
int* get_continguous_frames(int count);
int* get_non_continguous_frames(int count);

#endif /* FUNCTIONS_FRAME_H_ */
