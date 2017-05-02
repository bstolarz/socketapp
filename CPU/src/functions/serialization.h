#ifndef SERIALIZATION_H_
#define SERIALIZATION_H_

#include <commons/collections/dictionary.h>
#include <parser/metadata_program.h>
#include "../commons/structures.h"

typedef struct {
	u_int32_t size;
	char* data;
} t_serialized;

t_serialized intPtrSerializer(void* intPtrVoid);

t_serialized serialize_dict(t_dictionary* dict, t_serialized (*valueSerializer)(void*));
t_dictionary* deserialize_dict(t_serialized serializedDict, t_serialized (*valueSerializer)(void*));

t_serialized serialize_value_array(void* array, u_int32_t elemCount, u_int32_t elemSize, void valueSerializer(void*, void*));
void* deserialize_value_array(t_serialized serializedArray, u_int32_t elemSize, void valueDeserializer(void*, void*));

void int_value_serializer(void* intPtr, void* byteStream);
void int_value_deserializer(void* serializedPtr, void* byteStream);
void instruction_serializer(void* toSerialize, void* byteStream);
void instruction_deserializer(void* toDeserialize, void* byteStream);

t_serialized pcb_serialize(t_pcb*);
t_pcb* pcb_deserialize(t_serialized);

#endif /* SERIALIZATION_H_ */
