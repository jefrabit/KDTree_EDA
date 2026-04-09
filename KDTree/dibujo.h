// =============================================================================
// Funciones de dibujo
// =============================================================================

#ifndef DIBUJO_H
#define DIBUJO_H

#include <gtk/gtk.h>
#include "kdtree.h"

// Dimensiones del árbol
int AnchoSubtree(Nodo *n);
int AlturaArbol(Nodo *n);
void LimitesArbol(Nodo *n, int x, int nivel, int *minX, int *maxX);

// Dibujo
void DibujarNodo(cairo_t *cr, int xi, int yi, int xf, int yf, Nodo *R);
void DibujarArbolVisual(cairo_t *cr, Nodo *R, int x, int y, int nivel, int maxNivel);

#endif