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
int avanzarBloquesParaEscribir (int bloqueInicial,int desplazamientoLimite);
int is_regular_file(const char *path);

#endif /* FUNCTIONS_AUXILIARES_H_ */
