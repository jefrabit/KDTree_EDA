// =============================================================================
// KD-Tree Header - Estructuras y funciones del KD-Tree
// =============================================================================

#ifndef KD_TREE_H
#define KD_TREE_H

#include <stdio.h>
#include <stdlib.h>

// =============================================================================
// Estructuras del KD-Tree
// =============================================================================
typedef struct {
    int x, y;
} Punto;

typedef struct Nodo {
    Punto P;
    int eje;
    struct Nodo *Izq, *Der;
} Nodo;

typedef struct {
    Nodo *Raiz;
    int cantidad;
} KDTree;

// =============================================================================
// Funciones del KD-Tree
// =============================================================================
void Inicializar(KDTree *A);
void Ins(Punto PP, Nodo **R, int e);
void Insertar(KDTree *A, Punto PP);
void LiberarArbol(Nodo *R);

#endif // KD_TREE_H
