// =============================================================================
// Drawing Header - Funciones de dibujo del KD-Tree
// =============================================================================

#ifndef DRAWING_H
#define DRAWING_H

#include <gtk/gtk.h>
#include "kdtree.h"

// =============================================================================
// Funciones aux para calcular dimensiones del árbol
// =============================================================================
int AnchoSubtree(Nodo *n);
int AlturaArbol(Nodo *n);

// =============================================================================
// Funciones de dibujo
// =============================================================================
void DibujarNodo(cairo_t *cr, int xi, int yi, int xf, int yf, Nodo *R);
void DibujarArbolVisual(cairo_t *cr, Nodo *R, int x, int y, int nivel, int maxNivel);

#endif // DRAWING_H
