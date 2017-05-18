/*
 * sadica.h
 *
 *  Created on: 18/5/2017
 *      Author: utnso
 */

#ifndef COMMONS_SADICA_H_
#define COMMONS_SADICA_H_


typedef struct {
	//Este magic number es [7] o [6]?
	unsigned char magic_number[7];
	//uint8_t version;
	uint32_t fs_blocks; 				// cantidad de bloques
	uint32_t bitmap_blocks; 			// bitmap size in blocks
	//uint32_t allocations_table_offset; 	// allocations table's first block number
	//uint32_t data_blocks; // amount of data blocks
	//unsigned char padding[40]; // useless bytes just to complete the block size
} sadica_header;

#endif /* COMMONS_SADICA_H_ */
