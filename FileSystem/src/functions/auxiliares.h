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
void eliminarMetadataArchivo(char* path);
char* armarPathArchivo(char* pathDelKernel);
char* armarPathBloqueDatos(int numeroBloque);
void actualizarBytesEscritos (int* acum, int bytes);


#endif /* FUNCTIONS_AUXILIARES_H_ */
