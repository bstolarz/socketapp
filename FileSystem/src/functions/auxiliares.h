/*
 * auxiliares.h
 *
 *  Created on: 26/5/2017
 *      Author: utnso
 */

#ifndef FUNCTIONS_AUXILIARES_H_
#define FUNCTIONS_AUXILIARES_H_

void vaciarBloqueFisico(int bloque);
void crearArchivo(char* path, int posBloqueLibre);
void crearBloqueDatos(int posBloqueLibre);
void eliminarMetadataArchivo(char* path);
char* armarPathArchivo(char* pathDelKernel);
char* armarPathBloqueDatos(int numeroBloque);
void actualizarBytesEscritos (int* acum, int bytes);
int is_regular_file(const char *path);
int avanzarBloque(t_metadata_archivo* archivo, int desplazamientoHastaElBloque);

#endif /* FUNCTIONS_AUXILIARES_H_ */
