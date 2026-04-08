// =============================================================================
// KD-Tree Implementation
// =============================================================================

#include "kdtree.h"

// =============================================================================
// Funciones del KD-Tree
// =============================================================================
void Inicializar(KDTree *A) {
    A->Raiz = NULL;
    A->cantidad = 0;
}

void Ins(Punto PP, Nodo **R, int e) {
    if (*R == NULL) {
        *R = (Nodo *)malloc(sizeof(Nodo));
        (*R)->P = PP;
        (*R)->eje = e;
        (*R)->Izq = NULL;
        (*R)->Der = NULL;
    } else {
        if ((*R)->eje == 0) {
            if (PP.x < (*R)->P.x)
                Ins(PP, &(*R)->Izq, 1);
            else
                Ins(PP, &(*R)->Der, 1);
        } else {
            if (PP.y < (*R)->P.y)
                Ins(PP, &(*R)->Izq, 0);
            else
                Ins(PP, &(*R)->Der, 0);
        }
    }
}

void Insertar(KDTree *A, Punto PP) {
    Ins(PP, &A->Raiz, 0);
    A->cantidad++;
}

void LiberarArbol(Nodo *R) {
    if (R == NULL) return;
    LiberarArbol(R->Izq);
    LiberarArbol(R->Der);
    free(R);
}
