#include "dibujo.h"

int AnchoSub(Nodo *n) {
    if (n == NULL) return 0;
    if (n->izq == NULL && n->der == NULL) return 80;
    return AnchoSub(n->izq) + AnchoSub(n->der) + 80;
}

int Altura(Nodo *n) {
    if (n == NULL) return 0;
    int a = Altura(n->izq);
    int b = Altura(n->der);
    return 1 + (a > b ? a : b);
}

void Limites(Nodo *n, int x, int niv, int *minX, int *maxX) {
    if (n == NULL) return;
    if (x < *minX) *minX = x;
    if (x > *maxX) *maxX = x;
    
    int esp = 50;
    int ai = AnchoSub(n->izq);
    int ad = AnchoSub(n->der);
    if (ai > 0 || ad > 0) esp = (ai > ad ? ai : ad) + 20;
    if (esp < 50) esp = 50;
    
    Limites(n->izq, x - esp, niv + 1, minX, maxX);
    Limites(n->der, x + esp, niv + 1, minX, maxX);
}

void Dibujar(Nodo *r, cairo_t *c, int xi, int yi, int xf, int yf) {
    if (r == NULL) return;
    
    int xx = r->p.x;
    int yy = r->p.y;
    
    cairo_set_source_rgb(c, 0.2, 0.4, 1);
    cairo_set_line_width(c, 1);
    if (r->eje == 1) {
        cairo_move_to(c, xi, yy);
        cairo_line_to(c, xf, yy);
    } else {
        cairo_move_to(c, xx, yi);
        cairo_line_to(c, xx, yf);
    }
    cairo_stroke(c);
    
    cairo_set_source_rgb(c, 1, 0, 0);
    cairo_arc(c, xx, yy, 4, 0, 2 * G_PI);
    cairo_fill(c);
    
    if (r->eje == 0) {
        Dibujar(r->izq, c, xi, yi, xx, yf);
        Dibujar(r->der, c, xx, yi, xf, yf);
    } else {
        Dibujar(r->izq, c, xi, yi, xf, yy);
        Dibujar(r->der, c, xi, yy, xf, yf);
    }
}

void DibujarArbol(Nodo *r, cairo_t *c, int x, int y, int niv) {
    if (r == NULL) return;
    
    int ai = AnchoSub(r->izq);
    int ad = AnchoSub(r->der);
    int esp = 50;
    if (ai > 0 || ad > 0) esp = (ai > ad ? ai : ad) + 20;
    if (esp < 50) esp = 50;
    
    cairo_set_source_rgb(c, 0, 0.5, 0.8);
    cairo_set_line_width(c, 1);
    cairo_rectangle(c, x - 15, y - 8, 30, 16);
    cairo_stroke(c);
    
    char txt[32];
    sprintf(txt, "(%d,%d)", r->p.x, r->p.y);
    cairo_set_source_rgb(c, 0, 0, 0);
    cairo_set_font_size(c, 8);
    cairo_move_to(c, x - 18, y + 3);
    cairo_show_text(c, txt);
    
    int dy = 50;
    
    if (r->izq != NULL) {
        int xi = x - esp;
        int yi = y + dy;
        cairo_set_source_rgb(c, 0, 0, 0);
        cairo_set_line_width(c, 1);
        cairo_move_to(c, x, y + 8);
        cairo_line_to(c, xi, yi - 8);
        cairo_stroke(c);
        DibujarArbol(r->izq, c, xi, yi, niv + 1);
    }
    
    if (r->der != NULL) {
        int xd = x + esp;
        int yd = y + dy;
        cairo_set_source_rgb(c, 0, 0, 0);
        cairo_set_line_width(c, 1);
        cairo_move_to(c, x, y + 8);
        cairo_line_to(c, xd, yd - 8);
        cairo_stroke(c);
        DibujarArbol(r->der, c, xd, yd, niv + 1);
    }
}