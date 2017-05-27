/*
 * filesystem.h
 *
 *  Created on: 20/5/2017
 *      Author: utnso
 */

#ifndef INTERFACE_FILESYSTEM_H_
#define INTERFACE_FILESYSTEM_H_

void filesystem_connect();
void filesystem_leer(char* path, size_t offset, int size);
#endif /* INTERFACE_FILESYSTEM_H_ */
