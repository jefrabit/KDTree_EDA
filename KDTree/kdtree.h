#ifndef KD_TREE_H
#define KD_TREE_H

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int x, y;
} Punto;

typedef struct Nodo {
    Punto p;
    int eje;
    struct Nodo *izq, *der;
} Nodo;

typedef struct {
    Nodo *raiz;
    int cantidad;
} KDTree;

void Inicializar(KDTree *a);
void Insertar(KDTree *a, Punto p);
void Liberar(Nodo *r);

#endif