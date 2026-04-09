// =============================================================================
// Implementación del KD-Tree
// =============================================================================

#include "kdtree.h"

// Inicializa el árbol vacío
void Inicializar(KDTree *A) {
    A->Raiz = NULL;
    A->cantidad = 0;
}

// Inserta un punto en el árbol
void Ins(Punto PP, Nodo **R, int e) {
    if (*R == NULL) {
        *R = (Nodo *)malloc(sizeof(Nodo));
        if (*R == NULL) return;
        (*R)->P = PP;
        (*R)->eje = e;
        (*R)->Izq = NULL;
        (*R)->Der = NULL;
    } else {
        if ((*R)->eje == 0) {
            if (PP.x <= (*R)->P.x)
                Ins(PP, &(*R)->Izq, 1);
            else
                Ins(PP, &(*R)->Der, 1);
        } else {
            if (PP.y <= (*R)->P.y)
                Ins(PP, &(*R)->Izq, 0);
            else
                Ins(PP, &(*R)->Der, 0);
        }
    }
}

// Wrapper para insertar
void Insertar(KDTree *A, Punto PP) {
    Ins(PP, &A->Raiz, 0);
    A->cantidad++;
}

// Libera memoria del árbol
void LiberarArbol(Nodo *R) {
    if (R == NULL) return;
    LiberarArbol(R->Izq);
    LiberarArbol(R->Der);
    free(R);
}