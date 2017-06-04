#ifndef HASH_FUNCTION_H_
#define HASH_FUNCTION_H_

typedef size_t (*hash_function)(int, size_t);

size_t xor_hash(int PID, size_t page);
size_t rot_hash(int PID, size_t page);
size_t bernstein_hash(int PID, size_t page);
size_t ayudante_hash(int PID, size_t page);

#endif /* HASH_FUNCTION_H_ */
