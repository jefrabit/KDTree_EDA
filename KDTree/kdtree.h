// =============================================================================
// KD-Tree - Estructura y funciones básicas
// =============================================================================

#ifndef KD_TREE_H
#define KD_TREE_H

#include <stdio.h>
#include <stdlib.h>

// Punto en 2D
typedef struct {
    int x, y;
} Punto;

// Nodo del árbol
typedef struct Nodo {
    Punto P;
    int eje;  // 0 = divide por X, 1 = divide por Y
    struct Nodo *Izq, *Der;
} Nodo;

// ÁrbolKD-Tree
typedef struct {
    Nodo *Raiz;
    int cantidad;
} KDTree;

// Funciones
void Inicializar(KDTree *A);
void Insertar(KDTree *A, Punto PP);
void LiberarArbol(Nodo *R);

#endif