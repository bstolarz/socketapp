/*
 * filesystem.h
 *
 *  Created on: 20/5/2017
 *      Author: utnso
 */

#ifndef INTERFACE_FILESYSTEM_H_
#define INTERFACE_FILESYSTEM_H_

void filesystem_connect();
int filesystem_read(char* path, size_t offset, int size);
int filesystem_write(char* path, int offset, int size);
int filesystem_close();
int filesystem_delete(char* path);
int filesystem_create(char* path);
int filesystem_validate(char* path);

#endif /* INTERFACE_FILESYSTEM_H_ */

