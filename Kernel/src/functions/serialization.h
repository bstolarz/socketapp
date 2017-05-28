#ifndef SERIALIZATION_H_
#define SERIALIZATION_H_

#include <commons/collections/dictionary.h>
#include <stdbool.h>
#include "../commons/structures.h"

typedef void (*SerializerCopier)(void* /*from*/, char* /*to byteStream*/);
typedef void (*DeserializerCopier)(char*/*from byteStream*/, void*/*to*/);

typedef struct {
	u_int32_t size;
	char* data;
} t_dataBlob;

t_dataBlob serialize_static_type_dict(t_dictionary* dict, u_int32_t serializedElemSizelemSize, SerializerCopier valueSerializer);
t_dictionary* deserialize_static_type_dict(	char* serializedDict,
											u_int32_t elemSize,
											u_int32_t serializedElemSize,
											DeserializerCopier valueDeserializer,
											u_int32_t* byteSize);

t_dataBlob serialize_static_type_array(	void* array,
										u_int32_t elemCount,
										u_int32_t elemSize,
										u_int32_t serializedElemSize,
										SerializerCopier valueSerializer);
void* deserialize_static_type_array(char* serializedArray, u_int32_t elemSize, u_int32_t serializedElemSize, DeserializerCopier valueDeserializer, u_int32_t* byteSize);

void int_value_serializer(void* from, char* toByteStream);
void int_value_deserializer(char* fromByteStream, void* to);
void instruction_serializer(void* from, char* toByteStream);
void instruction_deserializer(char* fromByteStream, void* to);
void position_serializer(void* from, char* to);
void position_deserializer(char* from, void* to);

t_dataBlob stack_context_serialize(t_indiceDelStack* stackContext);
t_indiceDelStack* stack_context_deserialize(char* serializedStackContext, u_int32_t* byteSize);

t_dataBlob pcb_serialize(t_pcb*);
t_pcb* pcb_deserialize(t_dataBlob);

bool stack_context_compare(t_indiceDelStack* original, t_indiceDelStack* copy);
bool stack_compare(t_list* original, t_list* copy);
bool pcb_compare(t_pcb* original, t_pcb* copy);
bool dictionary_compare(t_dictionary* original, t_dictionary* copy, bool (*valueCompare)(void*, void*));
#endif /* SERIALIZATION_H_ */
