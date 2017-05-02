#ifndef SERIALIZATION_H_
#define SERIALIZATION_H_

#include <commons/collections/dictionary.h>
#include <stdbool.h>
#include "../commons/structures.h"

typedef struct {
	u_int32_t size;
	char* data;
} t_dataBlob;

t_dataBlob intPtrSerializer(void* intPtrVoid);

t_dataBlob serialize_dict(t_dictionary* dict, t_dataBlob (*valueSerializer)(void*));
t_dictionary* deserialize_dict(t_dataBlob serializedDict, t_dataBlob (*valueSerializer)(void*));

t_dataBlob serialize_static_type_array(void* array, u_int32_t elemCount, u_int32_t elemSize, void valueSerializer(void*, void*));
void* deserialize_static_type_array(t_dataBlob serializedArray, u_int32_t elemSize, void valueDeserializer(void*, void*));

void int_value_serializer(void* intPtr, void* byteStream);
void int_value_deserializer(void* serializedPtr, void* byteStream);
void instruction_serializer(void* toSerialize, void* byteStream);
void instruction_deserializer(void* toDeserialize, void* byteStream);

t_dataBlob pcb_serialize(t_pcb*);
t_pcb* pcb_deserialize(t_dataBlob);

bool pcb_compare(t_pcb* original, t_pcb* copy);

#endif /* SERIALIZATION_H_ */
