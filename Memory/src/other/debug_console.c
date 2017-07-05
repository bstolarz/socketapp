#include <limits.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include "debug_console.h"
#include "../commons/declarations.h"
#include "../functions/frame.h"
#include "../functions/ram.h"
#include "../functions/cache.h"

_Bool done = false;

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

char* stringify_admin_structs()
{
    char* maxIntStr = string_itoa(INT_MAX);
    int maxIntCharCount = string_length(maxIntStr);
    free(maxIntStr);

    size_t i;

    char* pageTableStr = string_new();

    char* proccessListStr = string_new();
    t_list* activeProccesses = list_create();
    int currentPID;
    bool int_equal(void* voidInt)
    {
    	return *((int*)voidInt) == currentPID;
    };

    string_append(&pageTableStr, "Tabla de Páginas\n");
    string_append(&proccessListStr, "Procesos Activos\n");

    // TODO: lockear?
    for (i = 0; i != proccessPageCount; ++i)
    {
    	// parte tabla de paginas
        t_pageTableEntry* entry = pageTable + i;
        
        char* frameStr = string_itoa(i);
        char* frameStrAligned = align_right(frameStr, maxIntCharCount);
        
        string_append(&pageTableStr, frameStrAligned);
        free(frameStr);
        free(frameStrAligned);

        currentPID = entry->PID; // hay q hacer esto para que la closure lambda funcione; es una captura trucha
        char* proccessStr = string_itoa(currentPID);
        char* proccessStrAligned = align_right(proccessStr, maxIntCharCount);
        
        string_append(&pageTableStr, proccessStrAligned);
        string_append(&pageTableStr, ".\n");


        // procesos activos
        // agrego pid si no estaba en lista de procesos activos
        if (currentPID != -1 && !list_any_satisfy(activeProccesses, &int_equal))
        {
        	int* dynamicInt = (int*) malloc(sizeof(int));
        	*dynamicInt = currentPID;
        	list_add(activeProccesses, dynamicInt);

        	string_append(&proccessListStr, proccessStrAligned);
        	string_append(&proccessListStr, "\n");
        }

        // los detruyo aca porque pueden llegar a usarse para la lista de procesos
        free(proccessStr);
        free(proccessStrAligned);
    }


    string_append(&pageTableStr, "\n\n\n\n");
    string_append(&pageTableStr, proccessListStr);

    free(proccessListStr);
    list_destroy(activeProccesses);

    return pageTableStr;
}


// estos pasan un frame a string
// retorna string, toma como parametro un frame (que es un char*)
typedef char* (*frame_stringifier)(char*);

char* stringify_frame_content_int(char* frame)
{
	size_t i;
	size_t intSize = sizeof(int);

	char* frameContentStr = string_new();
	int* frameAsIntArray = (int*) frame;

	// TODO: lockear?
	for (i = 0; (i * intSize) < (configMemory->frameSize - intSize); ++i)
	{
		int content = frameAsIntArray[i];
		string_append_with_format(&frameContentStr, "%d\t", content);
	}

	int lastInt = frameAsIntArray[i];
	string_append_with_format(&frameContentStr, "%d\n", lastInt);
	string_append_with_format(&frameContentStr, "hay un exceso de %d bytes\n", configMemory->frameSize % intSize);

	return frameContentStr;
}

char* stringify_frame_content_str(char* frame)
{
	// TODO: lockear?
	char* strContent = malloc(sizeof(char) * (configMemory->frameSize + 1));
	memcpy(strContent, frame, configMemory->frameSize);
	strContent[configMemory->frameSize] = '\0';

	return strContent;
}

char* stringify_frame_content_char(char* frame)
{
	size_t i = 0;
	char* charContent = malloc(configMemory->frameSize);

	for (i = 0; i != configMemory->frameSize; ++i)
	{
		if (frame[i] == '\0')
			charContent[i] = '.';
		else
			charContent[i] = frame[i];
	}
	charContent[i] = '\0';

	return charContent;
}


char* stringify_frames_content(_Bool (*framePredicate)(t_pageTableEntry*), frame_stringifier frameStringifier)
{
	size_t i;
	char* contents = string_new();
	
	for (i = 0; i != proccessPageCount; ++i)
	{
		t_pageTableEntry* pageEntry = pageTable + i;

		if (framePredicate(pageEntry))
		{
			pthread_spin_lock(&pageEntry->lock);

			string_append_with_format(&contents, "frame: %zd, PID: %d, page: %d\n", i, pageEntry->PID, pageEntry->page);
			char* frameStr = frameStringifier(get_frame(i));
			string_append(&contents, frameStr);
			free(frameStr);
			string_append(&contents, "\n");

			pthread_spin_unlock(&pageEntry->lock);
		}
	}

	return contents;
}

char* stringify_proccess_content(int PID, frame_stringifier frameStringifier)
{
	_Bool entryHasPID(t_pageTableEntry* entry) { return entry->PID == PID; };
	
	return stringify_frames_content(&entryHasPID, frameStringifier);
}

void dump_proc_int(int PID)
{
	char* dumpazo = stringify_proccess_content(PID, stringify_frame_content_int);
	printf("%s\n", dumpazo);
	free(dumpazo);
}

char* stringify_all_memory_content(frame_stringifier frameStringifier)
{
	_Bool anyFrame(t_pageTableEntry* _) { return true; }

	return stringify_frames_content(&anyFrame, frameStringifier);
}

char* stringify_cache_contents(frame_stringifier frameStringifier)
{
	int i;
	char* contents = string_from_format("Cache: data cacheada\n");

	cache_access_lock();

	for (i = 0; i != configMemory->cacheEntryCount; ++i)
	{
		t_cache_entry* cacheEntry = &cache[i];

		string_append_with_format(&contents, "frame: %d, PID: %d, page: %d, lastAccess: %zu\n", i, cacheEntry->PID, cacheEntry->page, cacheEntry->lastAccess);

		char* frameStr = frameStringifier(cacheEntry->content);
		string_append(&contents, frameStr);
		free(frameStr);

		string_append(&contents, "\n");
	}

	cache_access_unlock();

	return contents;
}




int _occupied_by_proccess_frame_count(int PID)
{
    _Bool is_occupied_by_procceess(t_pageTableEntry* entry)
    {
        return entry->PID == PID;
    };

    return frame_count(&is_occupied_by_procceess);
}

char* memory_size_str()
{
	char* memorySizeStr = string_new();

	string_append(&memorySizeStr, "Memory Size\n");
	string_append_with_format(&memorySizeStr, "frames totales = %d\n", configMemory->frameCount);
	string_append_with_format(&memorySizeStr, "frames para procesos = %zd\n", proccessPageCount);

	pthread_mutex_lock(&freeFrameMutex);
	string_append_with_format(&memorySizeStr, "frames libres = %zd\n", freeFrameCount);
	string_append_with_format(&memorySizeStr, "frames okupas = %zd\n", proccessPageCount - freeFrameCount);
	pthread_mutex_unlock(&freeFrameMutex);

	return memorySizeStr;
}

char* proccess_size_str(int PID)
{
	return string_from_format("El proceso %d ocupa %d frames\n", PID, _occupied_by_proccess_frame_count(PID));
}




void show_options()
{
	printf("Proceso Memoria\n");
	printf("\n");

	printf("- [ret ms] Setear retardo memoria en milisegs(?)\n");
	printf("\n");
	printf("- [dump cache] Muestra qué hay en la memoria caché\n");
	printf("- [dump structs] Muestra la Tabla de páginas y listado de procesos activos\n"); // semi done (ver si hay que locker) (testear)
	printf("- [dump cont (PID|*) type] Datos almacenados en memoria de todos los procesos o de un proceso en particular\n");
	printf("\n");
	printf("- [flu] borra lo que está cacheado\n");
	printf("\n");
	printf("- [size mem] Total frames, cant ocupados y cant libres\n");
	printf("- [size pid] Muestra el tamano de un proceso\n");
	printf("\n");
	printf("- [exit] cierra proceso memoria\n");
}

typedef _Bool (*console_command_handler)(char** tokens, char**);

void free_tokens(char** tokens)
{
	while (*tokens != NULL)
	{
		free(*tokens);
		++tokens;
	}
}

// -1 significa error
int str_to_PID(char* str)
{
	if (str == NULL) return -1;

	int pid = strtol(str, NULL, 10);

	if (pid > 0) // strtol error o es el proceso 0
	{
		return pid;
	}
	else
	{
		if (string_equals_ignore_case(str, "0")) return 0;
		else return -1;
	}
}

_Bool handle_delay(char** tokens, char** info)
{
	if (string_equals_ignore_case(tokens[0], "ret") && tokens[1] != NULL)
	{
		int newDelay = strtol(tokens[1], NULL, 10);

		if (newDelay > -1)
		{
			int prevDelay = configMemory->responseDelay;
			set_response_delay(newDelay);
			*info = string_from_format("[ret] nuevo delay = %d (anterior era %d)", configMemory->responseDelay, prevDelay);
		}
		else
		{
			*info = string_from_format("[ret] argumento %s es invalido tiene q ser un entero positivo", tokens[1]);
		}

		return true;
	}

	return false;
}

_Bool handle_flush(char** tokens, char** info)
{
	if (string_equals_ignore_case(tokens[0], "flu"))
	{
		cache_flush();
		*info = string_from_format("[flu] se limpio la cache");

		return true;
	}

	return false;
}

_Bool handle_dump(char** tokens, char** info)
{
	frame_stringifier frame_stringifier_from_tokens(char** tokens)
	{
		int i;

		// dump cont | dump cont *
		for (i = 1; i != 4; ++i)
		{
			if (tokens[i] == NULL)
				return stringify_frame_content_int;

			if (string_equals_ignore_case(tokens[i], "str"))
				return stringify_frame_content_str;

			if (string_equals_ignore_case(tokens[i], "char"))
				return stringify_frame_content_char;
		}
		return stringify_frame_content_int;
	};

	if (string_equals_ignore_case(tokens[0], "dump") && (tokens + 1) != NULL)
	{
		if (string_equals_ignore_case(tokens[1], "cache"))
		{
			*info = stringify_cache_contents(frame_stringifier_from_tokens(tokens));
			return true;
		}
		else if (string_equals_ignore_case(tokens[1], "structs"))
		{
			// [dump structs] Muestra la Tabla de páginas y listado de procesos activos\n");
			*info = stringify_admin_structs();
			return true;
		}
		else if (string_equals_ignore_case(tokens[1], "cont"))
		{


			if ((tokens + 2) == NULL || string_equals_ignore_case(tokens[2], "*") || string_equals_ignore_case(tokens[2], "str"))
			{
				*info = stringify_all_memory_content(frame_stringifier_from_tokens(tokens));
			}
			else
			{
				int PID = str_to_PID(tokens[2]);

				if (PID > -1)
				{
					*info = stringify_proccess_content(PID, frame_stringifier_from_tokens(tokens));
				}
				else
				{
					*info = string_from_format("[dump cont] no pude parsear bien el PID \"%s %s %s\"", tokens[0], tokens[1], tokens[2]);
				}
			}

			return true;
		}
	}

	return false;
}

_Bool handle_size(char** tokens, char** info)
{

	if (string_equals_ignore_case(tokens[0], "size") &&
		(tokens + 1) != NULL)
	{
		if (string_equals_ignore_case(tokens[1], "mem"))
		{
			*info = memory_size_str();
		}
		else
		{
			int pid = str_to_PID(tokens[1]);

			if (pid > -1)
				*info = proccess_size_str(pid);
			else
				*info = string_from_format("[size] no se argumento qué hacer con argument %s", tokens[1]);
		}

		return true;
	}

	return false;
}

_Bool handle_exit(char** tokens, char** info)
{
	if (string_equals_ignore_case(tokens[0], "exit"))
	{
		done = true;
		*info = string_from_format("Memoria se cierra");

		return true;
	}

	return false;
}

char* get_input()
{
	size_t inputSize = 128;
	char* inputContainer;
	inputContainer = (char*) (malloc(sizeof(char) * inputSize));

	size_t inputLength = getline(&inputContainer, &inputSize, stdin);
	inputContainer[inputLength - 1] = '\0';

	string_trim(&inputContainer);
	string_to_lower(inputContainer);

	return inputContainer;
}

_Bool proccess_command(char** commandTokens, char** resultInfo)
{
	console_command_handler commandHandlers[] = {
		&handle_delay,
		&handle_flush,
		&handle_dump,
		&handle_size,
		&handle_exit
	};
	int commandCount = sizeof(commandHandlers) / sizeof(console_command_handler);
	int i;

	_Bool foundCommand = false;

	for (i = 0; !foundCommand && i != commandCount; ++i)
		foundCommand = commandHandlers[i](commandTokens, resultInfo);

	if (!foundCommand)
		*resultInfo = string_from_format("[%s] no es un comando válido.", commandTokens);

	return foundCommand;
}

void debug_console()
{
	while(!done)
	{
		show_options();
		printf("Ingrese alguno de esos comandos: ");
		char* command = get_input();
		char** tokens = string_split(command, " ");
		printf("\n");

		if (tokens && tokens[0])
		{
			char* resultInfo = NULL;
			proccess_command(tokens, &resultInfo);

			printf("%s\n", resultInfo);

			if (resultInfo != NULL) free(resultInfo);
		}

		free(command);
		free_tokens(tokens);
	}
}
