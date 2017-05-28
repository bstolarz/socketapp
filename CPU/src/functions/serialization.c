#include <commons/string.h>
#include <commons/collections/dictionary.h>
#include <commons/collections/list.h>
#include <parser/metadata_program.h>
#include <stdbool.h>
#include <assert.h>
#include "serialization.h"
#include "../commons/structures.h"

u_int32_t PUNTERO_SIZE = sizeof(u_int32_t);
u_int32_t SERIALIZED_POSITION_SIZE = sizeof(int32_t) * 3;

// calculo tamanio y despues copio
// stream de bytes regex = elemCount [key\0value]*
t_dataBlob serialize_static_type_dict(t_dictionary* dict, u_int32_t serializedElemSize, SerializerCopier valueSerializer)
{
	t_dataBlob serializedDict;
	u_int32_t elemCount = dictionary_size(dict);

	// calculo tamanio total del dict
	// como los elementos tienen tamano fijo, no tengo que calcular dinamicamente su tamano
	serializedDict.size = sizeof(u_int32_t)/*elemCount*/ + (elemCount * serializedElemSize);

	void key_size_iterator(char* key, void* value)
	{
		serializedDict.size += string_length(key) + 1;
	}

	dictionary_iterator(dict, key_size_iterator);

	serializedDict.data = malloc(serializedDict.size);

	// copio en stream
	char* dataIter = serializedDict.data; // lo uso para ir avanzando por el stream de bytes y copiando key y value

	// copio elemCount
	memcpy(dataIter, &elemCount, sizeof(u_int32_t));
	dataIter += sizeof(u_int32_t);

	// copio key values, uno al lado del otro
	void serialize_iterator(char* key, void* value)
	{
		u_int32_t keyLen = string_length(key) + 1;
		memcpy(dataIter, key, keyLen);
		dataIter += keyLen;

		valueSerializer(value, dataIter);
		dataIter += serializedElemSize;
	}

	dictionary_iterator(dict, serialize_iterator);

	return serializedDict;
}

t_dictionary* deserialize_static_type_dict(	char*  serializedDict,
											u_int32_t elemSize,
											u_int32_t serializedElemSize,
											DeserializerCopier valueDeserializer,
											u_int32_t* byteSize)
{
	t_dictionary* dict = dictionary_create();
	u_int32_t offset = 0;

	// copy size
	u_int32_t i;
	u_int32_t elemCount = *((u_int32_t*) serializedDict);
	offset += sizeof(u_int32_t);


	for (i = 0; i != elemCount; ++i)
	{
		char* key = string_duplicate(serializedDict + offset);
		u_int32_t keyLen = string_length(key) + 1;
		offset += keyLen;

		void* value = malloc(elemSize);
		valueDeserializer(serializedDict + offset, value);
		offset += serializedElemSize;

		dictionary_put(dict, key, value);
	}

	*byteSize = offset;

	return dict;
}

t_dataBlob serialize_static_type_array(	void* array,
										u_int32_t elemCount,
										u_int32_t elemSize,
										u_int32_t serializedElemSize,
										SerializerCopier valueSerializer)
{
	t_dataBlob serializedArray;
	serializedArray.size = sizeof(u_int32_t)/*elemCount*/ + (elemCount * serializedElemSize);
	serializedArray.data = malloc(serializedArray.size);
	u_int32_t serializationOffset = 0;
	u_int32_t arrayOffset = 0;
	u_int32_t i;

	// count
	memcpy(serializedArray.data, &elemCount, sizeof(u_int32_t));
	serializationOffset += sizeof(u_int32_t);


	// content
	for (i = 0; i != elemCount; ++i)
	{
		valueSerializer(array + arrayOffset, serializedArray.data + serializationOffset);
		serializationOffset += serializedElemSize;
		arrayOffset += elemSize;
	}

	assert(serializationOffset == serializedArray.size);

	return serializedArray;
}

void* deserialize_static_type_array(char* serializedArray,
									u_int32_t elemSize,
									u_int32_t serializedElemSize,
									DeserializerCopier valueDeserializer,
									u_int32_t* byteSize)
{
	u_int32_t deserializationOffset = 0;
	u_int32_t arrayOffset = 0;
	int i;
	u_int32_t elemCount = *((u_int32_t*) serializedArray);

	deserializationOffset += sizeof(u_int32_t);
	void * array = malloc(elemSize * elemCount);

	for (i = 0; i != elemCount; ++i)
	{
		valueDeserializer(serializedArray + deserializationOffset, array + arrayOffset);
		deserializationOffset += serializedElemSize;
		arrayOffset += elemSize;
	}

	*byteSize = deserializationOffset;

	return array;
}

void int_value_serializer(void* from, char* toByteStream)
{
	memcpy(toByteStream, from, sizeof(int));
}

void int_value_deserializer(char* fromByteStream, void* to)
{
	memcpy(to, fromByteStream, sizeof(int));
}

void instruction_serializer(void* from, char* to)
{
	size_t uint32Size = sizeof(u_int32_t);

	t_intructions* instructionToSerialize = (t_intructions*)from;

	memcpy(to, &(instructionToSerialize->start), uint32Size);

	u_int32_t fixedSizeOffset = instructionToSerialize->offset;
	memcpy(to + uint32Size, &fixedSizeOffset, uint32Size);
}

void instruction_deserializer(char* from, void* to)
{
	size_t uint32Size = sizeof(u_int32_t);

	t_intructions* instructionToDeserialize = (t_intructions*)to;

	memcpy(&(instructionToDeserialize->start), from, uint32Size);

	u_int32_t fixedSizeOffset;
	memcpy(&fixedSizeOffset, from + uint32Size, uint32Size);

	instructionToDeserialize->offset = fixedSizeOffset;
}

void position_serializer(void* from, char* to)
{
	size_t int32Size = sizeof(int32_t);

	t_position* positionToSerialize = (t_position*)from;

	int32_t page32, off32, size32;

	if (positionToSerialize == NULL)
	{
		page32 = -1;
		off32 = -1;
		size32 = -1;
	}
	else
	{
		page32 = positionToSerialize->page;
		off32 = positionToSerialize->off;
		size32 = positionToSerialize->size;
	}

	memcpy(to, 					&page32, int32Size);
	memcpy(to + int32Size, 		&off32, int32Size);
	memcpy(to + int32Size * 2, 	&size32, int32Size);
}

void position_deserializer(char* from, void* to)
{
	size_t int32Size = sizeof(int32_t);

	t_position* positionToDeserialize = (t_position*)to;

	int32_t page32;
	int32_t off32;
	int32_t size32;

	memcpy(&page32, from, int32Size);
	memcpy(&off32, from + int32Size, int32Size);
	memcpy(&size32, from + int32Size + int32Size, int32Size);

	positionToDeserialize->page = page32;
	positionToDeserialize->off = off32;
	positionToDeserialize->size = size32;
}

t_dataBlob stack_context_serialize(t_indiceDelStack* stackContext)
{
	t_dataBlob serializedVars = serialize_static_type_dict(stackContext->vars, SERIALIZED_POSITION_SIZE, &position_serializer);
	t_dataBlob serializedArgs = serialize_static_type_array(stackContext->args, stackContext->argCount, sizeof(t_position), SERIALIZED_POSITION_SIZE, &position_serializer);
	u_int32_t stackContextSize =	serializedVars.size +
									serializedArgs.size +
									SERIALIZED_POSITION_SIZE  + // retVar
									sizeof(int32_t); // retPos

	t_dataBlob serializedStackContext = { stackContextSize, malloc(stackContextSize) };
	u_int32_t offset = 0;

	// copio cada campo serializado en un stream de con los campos
	memcpy(serializedStackContext.data + offset, serializedVars.data, serializedVars.size);
	offset += serializedVars.size;
	free(serializedVars.data);

	memcpy(serializedStackContext.data + offset, serializedArgs.data, serializedArgs.size);
	offset += serializedArgs.size;
	free(serializedArgs.data);

	position_serializer(stackContext->retVar, serializedStackContext.data + offset);
	offset += SERIALIZED_POSITION_SIZE;

	memcpy(serializedStackContext.data + offset, &stackContext->retPos, sizeof(int32_t));

	return serializedStackContext;
}

t_indiceDelStack* stack_context_deserialize(char* serializedStackContext, u_int32_t* byteSize)
{
	t_indiceDelStack* stackContext = malloc(sizeof(t_indiceDelStack));


	u_int32_t varsSize;
	stackContext->vars = deserialize_static_type_dict(	serializedStackContext,
														sizeof(t_position),
														SERIALIZED_POSITION_SIZE,
														&position_deserializer,
														&varsSize);

	stackContext->argCount = *((u_int32_t*) (serializedStackContext + varsSize));

	u_int32_t argsSize;
	void* deserializedArgs = deserialize_static_type_array(	serializedStackContext + varsSize,
															sizeof(t_position),
															SERIALIZED_POSITION_SIZE,
															&position_deserializer,
															&argsSize);

	memcpy(stackContext->args, deserializedArgs, stackContext->argCount * sizeof(t_position));

	free(deserializedArgs);

	stackContext->retVar = malloc(sizeof(t_position));
	position_deserializer(serializedStackContext + varsSize + argsSize, stackContext->retVar);
	if (stackContext->retVar->page == -1 &&
		stackContext->retVar->off == -1 &&
		stackContext->retVar->size == -1)
	{
		free(stackContext->retVar);
		stackContext->retVar = NULL;
	}

	stackContext->retPos = *((int32_t*) (serializedStackContext + varsSize + argsSize + SERIALIZED_POSITION_SIZE));

	*byteSize = varsSize + argsSize + SERIALIZED_POSITION_SIZE + PUNTERO_SIZE;

	return stackContext;
}

t_dataBlob stack_serialize(t_list* stack)
{
	u_int32_t stackCount = list_size(stack);
	u_int32_t byteSize = sizeof(u_int32_t);
	char* data = malloc(byteSize);
	memcpy(data, &stackCount, sizeof(u_int32_t)); // guardo cantidad de contextos

	u_int32_t i;

	for (i = 0; i != stackCount; ++i)
	{
		t_dataBlob serializedStackContext = stack_context_serialize(list_get(stack, i));

		// buscar mas memoria para el contexto nuevo,
		data = realloc(data, byteSize + serializedStackContext.size);

		// copiar el contexto en el nuevo espacio
		memcpy(data + byteSize, serializedStackContext.data, serializedStackContext.size);

		// actualizar el tamanio
		byteSize += serializedStackContext.size;

		// liberar la data del contexto, ya esta copiada en la data del stack
		free(serializedStackContext.data);
	}

	t_dataBlob serializedStack = { byteSize, data };
	return serializedStack;
}

t_list* stack_deserialize(void* serializedStack, u_int32_t* byteSize)
{
	t_list* stack = list_create();

	u_int32_t offset = 0;
	u_int32_t stackCount = *((u_int32_t*) serializedStack);
	offset += sizeof(u_int32_t);
	u_int32_t i;

	for (i = 0; i != stackCount; ++i)
	{
		u_int32_t stackContextSize;

		t_indiceDelStack* stackContext = stack_context_deserialize(serializedStack + offset, &stackContextSize);
		list_add(stack, stackContext);

		offset += stackContextSize;
	}

	*byteSize = offset;

	return stack;
}

t_dataBlob pcb_serialize(t_pcb* pcb)
{
	t_dataBlob serializedCodeIndex = serialize_static_type_array(pcb->indiceDeCodigo,
																 pcb->indiceDeCodigoCant,
																 sizeof(t_intructions),
																 sizeof(u_int32_t) * 2,
																 &instruction_serializer);

	t_dataBlob serializedStack = stack_serialize(pcb->indiceDeStack);

	int32_t pid32 = pcb->pid;
	int32_t exitCode32 = pcb->exitCode;

	u_int32_t pc32 = pcb->pc;
	u_int32_t cantPagsCodigo32 = pcb->cantPagsCodigo;
	u_int32_t indiceDeCodigoCant32 = pcb->indiceDeCodigoCant;
	u_int32_t indiceDeEtiquetasCant32 = pcb->indiceDeEtiquetasCant;
	u_int32_t stackPosition32 = pcb->stackPosition;
	u_int32_t maxStackPosition32 = pcb->maxStackPosition;

	t_dataBlob fieldsToSerialize[] = {
		// primero copio todos los ints, asi es mas facil deserializarlo
		{ sizeof(int32_t), (void*) &pid32 },
		{ sizeof(u_int32_t), (void*) &pc32 },
		{ sizeof(u_int32_t), (void*) &cantPagsCodigo32 },
		{ sizeof(u_int32_t), (void*) &indiceDeCodigoCant32 },
		{ sizeof(u_int32_t), (void*) &indiceDeEtiquetasCant32 },
		{ sizeof(u_int32_t), (void*) &stackPosition32 },
		{ sizeof(u_int32_t), (void*) &maxStackPosition32 },
		{ sizeof(int32_t), (void*) &exitCode32 },
		serializedCodeIndex, // es mas facil deserializar con este aca, despues de lo q siempre es fijo
		{ pcb->indiceDeEtiquetasCant, pcb->indiceDeEtiquetas },
		serializedStack
	};

	u_int32_t serializedFieldsLen = sizeof(fieldsToSerialize) / sizeof(t_dataBlob);


	t_dataBlob serializedPcb;

	// encontrar tamano total
	serializedPcb.size = 0;
	u_int32_t i;

	for (i = 0; i != serializedFieldsLen; ++i)
		serializedPcb.size += fieldsToSerialize[i].size;


	// copiar datos
	serializedPcb.data = malloc(serializedPcb.size);

	u_int32_t offset = 0;
	for (i = 0; i != serializedFieldsLen; ++i)
	{
		memcpy(serializedPcb.data + offset, fieldsToSerialize[i].data, fieldsToSerialize[i].size);
		offset += fieldsToSerialize[i].size;
	}


	// liberar recursos
	free(serializedCodeIndex.data);
	free(serializedStack.data);

	return serializedPcb;
}

t_pcb* pcb_deserialize(t_dataBlob serializedPcb)
{
	t_pcb* pcb = malloc(sizeof(t_pcb));
	int32_t pidFixedSize;
	int32_t exitCodeFixedSize;
	u_int32_t pc32 = pcb->pc;
	u_int32_t cantPagsCodigo32;
	u_int32_t indiceDeCodigoCant32;
	u_int32_t indiceDeEtiquetasCant32;
	u_int32_t stackPosition32;
	u_int32_t maxStackPosition32;

	t_dataBlob serializedFields[] = {
		{ sizeof(int32_t), (void*) &pidFixedSize },
		{ sizeof(u_int32_t), (void*) &pc32 },
		{ sizeof(u_int32_t), (void*) &cantPagsCodigo32 },
		{ sizeof(u_int32_t), (void*) &indiceDeCodigoCant32 },
		{ sizeof(u_int32_t), (void*) &indiceDeEtiquetasCant32 },
		{ sizeof(u_int32_t), (void*) &stackPosition32 },
		{ sizeof(u_int32_t), (void*) &maxStackPosition32 },
		{ sizeof(int32_t), (void*) &exitCodeFixedSize }
	};

	u_int32_t serializedFieldsLen = sizeof(serializedFields) / sizeof(t_dataBlob);

	// pasar datos desde el stream al pcb
	u_int32_t i;
	u_int32_t offset = 0;

	// ints
	for (i = 0; i != serializedFieldsLen; ++i)
	{
		memcpy(serializedFields[i].data, serializedPcb.data + offset, serializedFields[i].size);
		offset += serializedFields[i].size;
	}

	pcb->pid = pidFixedSize;
	pcb->exitCode = exitCodeFixedSize;
	pcb->pc = pc32;
	pcb->cantPagsCodigo = cantPagsCodigo32;
	pcb->indiceDeCodigoCant = indiceDeCodigoCant32;
	pcb->indiceDeEtiquetasCant = indiceDeEtiquetasCant32;
	pcb->stackPosition = stackPosition32;
	pcb->maxStackPosition = maxStackPosition32;


	// indice de codigo
	u_int32_t codeIndexSize;
	pcb->indiceDeCodigo = deserialize_static_type_array(serializedPcb.data + offset,
													sizeof(t_intructions),
													sizeof(u_int32_t) * 2,
													&instruction_deserializer,
													&codeIndexSize);
	offset += codeIndexSize;

	// etiquetas
	pcb->indiceDeEtiquetas = malloc(pcb->indiceDeEtiquetasCant);
	memcpy(pcb->indiceDeEtiquetas, serializedPcb.data + offset, pcb->indiceDeEtiquetasCant);
	offset += pcb->indiceDeEtiquetasCant;

	// stack
	u_int32_t stackSize;
	pcb->indiceDeStack = stack_deserialize(serializedPcb.data + offset, &stackSize);
	offset += stackSize;

	// dict files
	// ...

	assert(serializedPcb.size == offset);

	return pcb;
}




// comparar para testear
bool dictionary_compare(t_dictionary* original, t_dictionary* copy, bool (*valueCompare)(void*, void*))
{
	if (dictionary_size(original) != dictionary_size(copy))
		return false;

	// chequear que la copia tenga mismos valores
	bool areEqual = true;

	void copyHasKeys(char* key, void* value)
	{
		if (!areEqual) return;

		void* copyValue = dictionary_get(copy, key);

		areEqual = copyValue != NULL && valueCompare(value, copyValue);
	};

	dictionary_iterator(original, copyHasKeys);

	return areEqual;
}

bool stack_context_compare(t_indiceDelStack* original, t_indiceDelStack* copy)
{
	// vars
	bool position_compare(void* original, void* copy)
	{
		t_position* originalPos = (t_position*) original;
		t_position* copyPos = (t_position*) copy;

		if (originalPos == NULL)
			return copyPos == NULL;

		return	originalPos->page == copyPos->page &&
				originalPos->off == copyPos->off &&
				originalPos->size == copyPos->size;
	};

	if (!dictionary_compare(original->vars, copy->vars, &position_compare))
		return false;


	if (original->argCount != copy->argCount)
		return false;

	// args
	int i;
	for (i = 0; i != original->argCount; ++i)
		if (!position_compare(&original->args[i], &copy->args[i]))
			return false;

	if (original->retPos != copy->retPos)
		return false;

	if (!position_compare(original->retVar, copy->retVar))
		return false;

	return true;
}

bool stack_compare(t_list* original, t_list* copy)
{
	int originalCount = list_size(original);
	int copyCount = list_size(copy);

	if (originalCount != copyCount) return false;

	int i;

	for (i = 0; i != originalCount; ++i)
		if (!stack_context_compare(list_get(original, i), list_get(copy, i)))
			return false;

	return true;
}

bool pcb_compare(t_pcb* original, t_pcb* copy)
{
	if (original->pid != copy->pid) return false;
	if (original->pc != copy->pc) return false;
	if (original->cantPagsCodigo != copy->cantPagsCodigo) return false;
	if (original->indiceDeCodigoCant != copy->indiceDeCodigoCant) return false;
	if (original->indiceDeEtiquetasCant != copy->indiceDeEtiquetasCant) return false;
	if (original->stackPosition != copy->stackPosition) return false;
	if (original->maxStackPosition != copy->maxStackPosition) return false;
	if (original->exitCode != copy->exitCode) return false;

	int i;

	for (i = 0; i != original->indiceDeCodigoCant; ++i)
		if (original->indiceDeCodigo[i].start != copy->indiceDeCodigo[i].start ||
			original->indiceDeCodigo[i].offset != copy->indiceDeCodigo[i].offset)
			return false;


	for (i = 0; i != original->indiceDeEtiquetasCant; ++i)
		if (original->indiceDeEtiquetas[i] != copy->indiceDeEtiquetas[i])
			return false;

	if (!stack_compare(original->indiceDeStack, copy->indiceDeStack))
		return false;

	return true;
}

