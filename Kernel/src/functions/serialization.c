#include <commons/string.h>
#include <commons/collections/dictionary.h>

#include "serialization.h"


t_serialized intPtrSerializer(void* intPtrVoid)
{
	t_serialized intPtrSerialized = { sizeof(int), intPtrVoid };
	return intPtrSerialized;
}

// implementacion simple: calculo tamanio y despues copio
t_serialized serialize_dict(t_dictionary* dict, t_serialized (*valueSerializer)(void*))
{
	t_serialized serializedDict;

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

		t_serialized serializedValue = valueSerializer(value);
		memcpy(serializedDict.data, serializedValue.data, serializedValue.size);
		serializedDict.data += serializedValue.size;
	}


	dictionary_iterator(dict, serialize_iterator);

	serializedDict.data -= serializedDict.size;

	return serializedDict;
}

t_dictionary* deserialize_dict(t_serialized serializedDict, t_serialized (*valueSerializer)(void*))
{
	t_dictionary* dict = dictionary_create();
	u_int32_t offset = 0;

	while (serializedDict.size != 0)
	{
		char* key = string_duplicate(serializedDict.data + offset);
		u_int32_t keyLen = string_length(key) + 1;
		serializedDict.data += keyLen;
		serializedDict.size -= keyLen;

		t_serialized serializedValue = valueSerializer(serializedDict.data);
		void* value = malloc(serializedValue.size);
		memcpy(value, serializedValue.data, serializedValue.size);
		serializedDict.data += serializedValue.size;
		serializedDict.size -= serializedValue.size;

		dictionary_put(dict, key, value);
	}

	return dict;
}

t_serialized serialize_value_array(void* array, u_int32_t elemCount, u_int32_t elemSize, void (*valueSerializer)(void* intPtr, void* byteStream))
{
	t_serialized serializedArray;
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

void* deserialize_value_array(t_serialized serializedArray, u_int32_t elemSize, void (*valueDeserializer)(void* intPtr, void* byteStream))
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


t_serialized pcb_serialize(t_pcb* pcb)
{
	t_serialized serializedCodeIndex = serialize_value_array((void*) pcb->indiceDeCodigo, pcb->indiceDeCodigoCant, sizeof(u_int32_t) * 2, &instruction_serializer);

	t_serialized fieldsToSerialize[] = {
		{ sizeof(int), (void*) &pcb->pid },
		{ sizeof(int), (void*) &pcb->pc },
		{ sizeof(int), (void*) &pcb->cantPagsCodigo },
		{ sizeof(int), (void*) &pcb->indiceDeCodigoCant },
		serializedCodeIndex,
		{ sizeof(int), (void*) &pcb->stackPosition },
		{ sizeof(int), (void*) &pcb->maxStackPosition },
		{ sizeof(int), (void*) &pcb->exitCode },
	};

	u_int32_t serializedFieldsLen = sizeof(fieldsToSerialize) / sizeof(t_serialized);


	t_serialized serializedPcb;

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

t_pcb* pcb_deserialize(t_serialized serializedPcb)
{
	t_pcb* pcb = malloc(sizeof(t_pcb));

	t_serialized serializedFields[] = {
		{ sizeof(int), (void*) &pcb->pid },
		{ sizeof(int), (void*) &pcb->pc },
		{ sizeof(int), (void*) &pcb->cantPagsCodigo },
		{ sizeof(int), (void*) &pcb->indiceDeCodigoCant }
	};

	u_int32_t serializedFieldsLen = sizeof(serializedFields) / sizeof(t_serialized);

	// pasar datos desde el stream al pcb
	u_int32_t i;
	u_int32_t offset = 0;

	// primeros ints
	for (i = 0; i != serializedFieldsLen; ++i)
	{
		memcpy(serializedFields[i].data, serializedPcb.data + offset, serializedFields[i].size);
		offset += serializedFields[i].size;
	}

	// indice de codigo
	u_int32_t codeIndexSize = pcb->indiceDeCodigoCant * sizeof(u_int32_t) * 2;
	t_serialized serializedCodeIndex = { codeIndexSize, serializedPcb.data + offset };
	pcb->indiceDeCodigo = deserialize_value_array( serializedCodeIndex,
													sizeof(u_int32_t) * 2,
													instruction_deserializer);
	offset += codeIndexSize;


	// otros ints
	t_serialized moreSerializedFields[] = {
		{ sizeof(int), (void*) &pcb->stackPosition },
		{ sizeof(int), (void*) &pcb->maxStackPosition },
		{ sizeof(int), (void*) &pcb->exitCode }
	};

	serializedFieldsLen = sizeof(moreSerializedFields) / sizeof(t_serialized);

	for (i = 0; i != serializedFieldsLen; ++i)
	{
		memcpy(moreSerializedFields[i].data, serializedPcb.data + offset, moreSerializedFields[i].size);
		offset += moreSerializedFields[i].size;
	}

	return pcb;
}

