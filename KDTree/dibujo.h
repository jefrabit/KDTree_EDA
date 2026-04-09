#ifndef DIBUJO_H
#define DIBUJO_H

#include <gtk/gtk.h>
#include "kdtree.h"

int AnchoSub(Nodo *n);
int Altura(Nodo *n);
void Limites(Nodo *n, int x, int niv, int *minX, int *maxX);
void Dibujar(Nodo *r, cairo_t *c, int xi, int yi, int xf, int yf);
void DibujarArbol(Nodo *r, cairo_t *c, int x, int y, int niv);

#endif