#ifndef MEMORY_CONFIG_H_
#define MEMORY_CONFIG_H_

typedef struct {

	int portNum;

	int frameCount;
	int frameSize;
	int responseDelay;

	// cache config
	int cacheEntryCount;
	int cachePerProccess;
	char* cacheReplaceStrategy;

} memory_config;

#endif /* MEMORY_CONFIG_H_ */
