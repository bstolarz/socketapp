/*
 * bitmap.h
 *
 *  Created on: 26/5/2017
 *      Author: utnso
 */

#ifndef FUNCTIONS_BITMAP_H_
#define FUNCTIONS_BITMAP_H_

int encontrarUnBloqueLibre();
int hayNBloquesLibres(int n);
void ocuparBloqueLibre(int posBloqueLibre);
void liberarBloqueDelBitmap(int bloque);

#endif /* FUNCTIONS_BITMAP_H_ */
