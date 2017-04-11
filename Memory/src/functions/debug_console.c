#include "debug_console.h"
#include "../commons/declarations.h"
#include "frame.h"
#include <limits.h>
#include <stdlib.h>
#include <commons/string.h>

void set_response_delay(int newResponseDelay)
{
    configMemory->responseDelay = newResponseDelay;
}




// Dump
char* align_left(char* content, int length)
{
    int contentLength = string_length(content);

    char* pad = string_repeat(' ', length - contentLength);
    char* aligned = string_from_format("%s%s", content, pad);
    
    free(pad);

    return aligned;
}


char* align_right(char* content, int length)
{
    int contentLength = string_length(content);

    char* pad = string_repeat(' ', length - contentLength);
    char* aligned = string_from_format("%s%s", pad, content);
    
    free(pad);

    return aligned;
}

char* page_table_str()
{
    char* pageTableStr = string_new();
    int i;

    char* maxIntStr = string_itoa(INT_MAX);
    int maxIntCharCount = string_length(maxIntStr);
    free(maxIntStr);

    for (i = 0; i != proccessPageCount; ++i)
    {
        t_pageTableEntry* entry = pageTable + i;
        
        char* frameStr = string_itoa(i);
        char* frameStrAligned = align_right(frameStr, maxIntCharCount);
        
        string_append(&pageTableStr, frameStrAligned);
        free(frameStr);
        free(frameStrAligned);


        char* proccessStr = string_itoa(entry->PID);
        char* proccessStrAligned = align_right(proccessStr, maxIntCharCount);
        
        string_append(&pageTableStr, proccessStrAligned);
        free(proccessStr);
        free(proccessStrAligned);

        string_append(&pageTableStr, ".\n");
    }

    return pageTableStr;
}




// Size
int _frame_count(_Bool (*framePredicate)(t_pageTableEntry*))
{
    int i;
    int count = 0;

    for (i = 0; i != proccessPageCount; ++i)
        if ((*framePredicate)(pageTable + i))
            ++count;

    return count;
}

int _free_frame_count() { return _frame_count(&is_frame_free); }
int _occupied_frame_count() { return _frame_count(&is_frame_occupied); }
int _occupied_by_proccess_frame_count(int PID)
{
    _Bool is_occupied_by_procceess(t_pageTableEntry* entry)
    {
        return entry->PID == PID;
    };

    return _frame_count(&is_occupied_by_procceess);
}

char* memory_size_str()
{
	char* memorySizeStr = string_new();

	string_append(&memorySizeStr, "Memory Size\n");
	string_append_with_format(&memorySizeStr, "frames totales = %d\n", configMemory->frameCount);
	string_append_with_format(&memorySizeStr, "frames para procesos = %d\n", proccessPageCount);
	string_append_with_format(&memorySizeStr, "frames libres = %d\n", _free_frame_count());
	string_append_with_format(&memorySizeStr, "frames okupas = %d\n", _occupied_frame_count());

	return memorySizeStr;
}
;
char* proccess_size_str(int PID)
{
	return string_from_format("El proceso %d ocupa %d frames\n", PID, _occupied_by_proccess_frame_count(PID));
}

