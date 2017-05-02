#include <commons/string.h>
#include <commons/collections/dictionary.h>
#include <parser/metadata_program.h>
#include <stdbool.h>
#include "serialization.h"


t_dataBlob intPtrSerializer(void* intPtrVoid)
{
	t_dataBlob intPtrSerialized = { sizeof(int), intPtrVoid };
	return intPtrSerialized;
}

// implementacion simple: calculo tamanio y despues copio
t_dataBlob serialize_dict(t_dictionary* dict, t_dataBlob (*valueSerializer)(void*))
{
	t_dataBlob serializedDict;

	// calculo tamanio total del dict
	serializedDict.size = 0;

	void size_iterator(char* key, void* value)
	{
		serializedDict.size += string_length(key) + 1 + valueSerializer(value).size;
	}

	dictionary_iterator(dict, size_iterator);


	// copio en stream
	serializedDict.data = malloc(serializedDict.size);

	void serialize_iterator(char* key, void* value)
	{
		u_int32_t keyLen = string_length(key) + 1;
		memcpy(serializedDict.data, key, keyLen);
		serializedDict.data += keyLen;

		t_dataBlob serializedValue = valueSerializer(value);
		memcpy(serializedDict.data, serializedValue.data, serializedValue.size);
		serializedDict.data += serializedValue.size;
	}


	dictionary_iterator(dict, serialize_iterator);

	serializedDict.data -= serializedDict.size;

	return serializedDict;
}

t_dictionary* deserialize_dict(t_dataBlob serializedDict, t_dataBlob (*valueSerializer)(void*))
{
	t_dictionary* dict = dictionary_create();
	u_int32_t offset = 0;

	while (serializedDict.size != 0)
	{
		char* key = string_duplicate(serializedDict.data + offset);
		u_int32_t keyLen = string_length(key) + 1;
		serializedDict.data += keyLen;
		serializedDict.size -= keyLen;

		t_dataBlob serializedValue = valueSerializer(serializedDict.data);
		void* value = malloc(serializedValue.size);
		memcpy(value, serializedValue.data, serializedValue.size);
		serializedDict.data += serializedValue.size;
		serializedDict.size -= serializedValue.size;

		dictionary_put(dict, key, value);
	}

	return dict;
}

t_dataBlob serialize_static_type_array(void* array, u_int32_t elemCount, u_int32_t elemSize, void (*valueSerializer)(void* intPtr, void* byteStream))
{
	t_dataBlob serializedArray;
	serializedArray.size = elemCount * elemSize;
	serializedArray.data = malloc(serializedArray.size);

	u_int32_t i;

	for (i = 0; i != elemCount; ++i)
	{
		u_int32_t offset = i * elemSize;
		valueSerializer(array + offset, serializedArray.data + offset);
	}

	return serializedArray;
}

void* deserialize_static_type_array(t_dataBlob serializedArray, u_int32_t elemSize, void (*valueDeserializer)(void* intPtr, void* byteStream))
{
	u_int32_t elemCount = serializedArray.size / elemSize;
	void * array = malloc(serializedArray.size);
	int i;

	for (i = 0; i != elemCount; ++i)
	{
		u_int32_t offset = i * elemSize;
		valueDeserializer(array + offset, serializedArray.data + offset);
	}

	return array;
}

void int_value_serializer(void* intPtr, void* byteStream)
{
	memcpy(byteStream, intPtr, sizeof(int));
}

void int_value_deserializer(void* serializedPtr, void* byteStream)
{
	memcpy(byteStream, serializedPtr, sizeof(int));
}


void instruction_serializer(void* toSerialize, void* byteStream)
{
	size_t uint32Size = sizeof(u_int32_t);

	t_intructions* instructionToSerialize = (t_intructions*)toSerialize;

	memcpy(byteStream, &(instructionToSerialize->start), uint32Size);

	u_int32_t fixedSizeOffset = instructionToSerialize->offset;
	memcpy(byteStream + uint32Size, &fixedSizeOffset, uint32Size);
}

void instruction_deserializer(void* toDeserialize, void* byteStream)
{
	size_t uint32Size = sizeof(u_int32_t);

	t_intructions* instructionToDeserialize = (t_intructions*)toDeserialize;

	memcpy(&(instructionToDeserialize->start), byteStream, uint32Size);

	u_int32_t fixedSizeOffset;
	memcpy(&fixedSizeOffset, byteStream + uint32Size, uint32Size);

	instructionToDeserialize->offset = fixedSizeOffset;
}


t_dataBlob pcb_serialize(t_pcb* pcb)
{
	t_dataBlob serializedCodeIndex = serialize_static_type_array((void*) pcb->indiceDeCodigo, pcb->indiceDeCodigoCant, sizeof(u_int32_t) * 2, &instruction_serializer);

	t_dataBlob fieldsToSerialize[] = {
		{ sizeof(int), (void*) &pcb->pid },
		{ sizeof(int), (void*) &pcb->pc },
		{ sizeof(int), (void*) &pcb->cantPagsCodigo },
		{ sizeof(int), (void*) &pcb->indiceDeCodigoCant },
		{ sizeof(int), (void*) &pcb->indiceDeEtiquetasCant },
		{ sizeof(int), (void*) &pcb->stackPosition },
		{ sizeof(int), (void*) &pcb->maxStackPosition },
		{ sizeof(int), (void*) &pcb->exitCode },
		serializedCodeIndex, // es mas facil deserializar con este aca, despues de lo q siempre es fijo
		{ pcb->indiceDeEtiquetasCant, pcb->indiceDeEtiquetas }
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

	return serializedPcb;
}

t_pcb* pcb_deserialize(t_dataBlob serializedPcb)
{
	t_pcb* pcb = malloc(sizeof(t_pcb));

	t_dataBlob serializedFields[] = {
		{ sizeof(int), (void*) &pcb->pid },
		{ sizeof(int), (void*) &pcb->pc },
		{ sizeof(int), (void*) &pcb->cantPagsCodigo },
		{ sizeof(int), (void*) &pcb->indiceDeCodigoCant },
		{ sizeof(int), (void*) &pcb->indiceDeEtiquetasCant },
		{ sizeof(int), (void*) &pcb->stackPosition },
		{ sizeof(int), (void*) &pcb->maxStackPosition },
		{ sizeof(int), (void*) &pcb->exitCode }
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

	// indice de codigo
	u_int32_t codeIndexSize = pcb->indiceDeCodigoCant * sizeof(u_int32_t) * 2;
	t_dataBlob serializedCodeIndex = { codeIndexSize, serializedPcb.data + offset };
	pcb->indiceDeCodigo = deserialize_static_type_array( serializedCodeIndex,
													sizeof(u_int32_t) * 2,
													instruction_deserializer);
	offset += codeIndexSize;

	// etiquetas
	memcpy(pcb->indiceDeEtiquetas, serializedPcb.data + offset, pcb->indiceDeEtiquetasCant);
	offset += pcb->indiceDeEtiquetasCant;

	// stack ...

	return pcb;
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

	return true;
}

