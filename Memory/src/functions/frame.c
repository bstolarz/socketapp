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

int* get_continguous_frames(int count)
{
    int begin, end;

    for (begin = 0; begin != proccessPageCount; ++begin)
    {
        for (end = 0; end != count; ++end)
        {
            if (is_frame_occupied(pageTable + begin + end))
                break;
        }

        if (end == count)
        {
            int* frames = (int*) malloc(sizeof(int) * count);
            int i;

            if (frames != NULL)
            {
                for (i = 0; i != count; ++i)
                    frames[i] = begin + i;
            }

            return frames;
        }
    }

    return NULL;
}

int* get_non_continguous_frames(int count)
{
    int* frames = (int*) malloc(sizeof(int) * count);

    if (frames == NULL)
        return NULL;

    int i, j = 0;

    for (i = 0; i != proccessPageCount; ++i)
    {
        if (is_frame_free(pageTable + i))
        {
            frames[j++] = i;

            if (j == count)
                return frames;
        }
    }

    free(frames);
    return NULL;
}
