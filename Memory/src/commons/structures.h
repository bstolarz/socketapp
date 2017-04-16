/*
 * Structures.h
 *
 *  Created on: 1/4/2017
 *      Author: utnso
 */

#ifndef COMMONS_STRUCTURES_H_
#define COMMONS_STRUCTURES_H_

typedef struct {

	int PID;
	int page;

} t_pageTableEntry;

typedef struct {

	char* portNumStr;
	int frameCount;
	int frameSize;
	int cacheEntryCount;
	int cachePerProccess;
	char* cacheReplaceStrategy;
	int responseDelay;

} t_memory;


#endif /* COMMONS_STRUCTURES_H_ */
