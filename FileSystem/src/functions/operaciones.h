/*
 * operaciones.h
 *
 *  Created on: 9/5/2017
 *      Author: utnso
 */

#ifndef FUNCTIONS_OPERACIONES_H_
#define FUNCTIONS_OPERACIONES_H_

int validar(char* path);
int crear(char* path);
int borrar(char* path);
size_t obtenerDatos(char* path, off_t offset, size_t size);
int guardarDatos(char* path, off_t offset, size_t size, void* buffer);

#endif /* FUNCTIONS_OPERACIONES_H_ */
