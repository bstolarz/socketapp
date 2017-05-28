/*
 * auxiliares.h
 *
 *  Created on: 26/5/2017
 *      Author: utnso
 */

#ifndef FUNCTIONS_AUXILIARES_H_
#define FUNCTIONS_AUXILIARES_H_

void crearArchivo(char* path, int posBloqueLibre);
void crearBloqueDatos(int posBloqueLibre);
void eliminarBloqueDatos(int bloque);
void eliminarMetadataArchivo(char* path);

#endif /* FUNCTIONS_AUXILIARES_H_ */
