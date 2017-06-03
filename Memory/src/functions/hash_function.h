#ifndef HASH_FUNCTION_H_
#define HASH_FUNCTION_H_

typedef size_t (*hash_function)(int, int);

size_t xor_hash(int PID, int page);
size_t rot_hash(int PID, int page);
size_t bernstein_hash(int PID, int page);

#endif /* HASH_FUNCTION_H_ */
