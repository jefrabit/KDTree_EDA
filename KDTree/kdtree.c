#include "kdtree.h"

void Inicializar(KDTree *a) {
    a->raiz = NULL;
    a->cantidad = 0;
}

void Ins(Punto p, Nodo **r, int e) {
    if (*r == NULL) {
        *r = (Nodo *)malloc(sizeof(Nodo));
        if (*r == NULL) return;
        (*r)->p = p;
        (*r)->eje = e;
        (*r)->izq = NULL;
        (*r)->der = NULL;
    } else {
        if ((*r)->eje == 0) {
            if (p.x <= (*r)->p.x)
                Ins(p, &(*r)->izq, 1);
            else
                Ins(p, &(*r)->der, 1);
        } else {
            if (p.y <= (*r)->p.y)
                Ins(p, &(*r)->izq, 0);
            else
                Ins(p, &(*r)->der, 0);
        }
    }
}

void Insertar(KDTree *a, Punto p) {
    Ins(p, &a->raiz, 0);
    a->cantidad++;
}

void Liberar(Nodo *r) {
    if (r == NULL) return;
    Liberar(r->izq);
    Liberar(r->der);
    free(r);
}