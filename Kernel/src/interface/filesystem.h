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
void filesystem_escribir(char* path, int offset, int size);
void filesystem_close();
void filesystem_delete();
int filesystem_create(char* flags,char* path);
int filesystem_validar(char* path);

#endif /* INTERFACE_FILESYSTEM_H_ */

