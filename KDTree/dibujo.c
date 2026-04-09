// =============================================================================
// Funciones de dibujo del KD-Tree
// =============================================================================

#include "dibujo.h"

// Ancho del subárbol
int AnchoSubtree(Nodo *n) {
    if (n == NULL) return 0;
    if (n->Izq == NULL && n->Der == NULL) return 80;
    return AnchoSubtree(n->Izq) + AnchoSubtree(n->Der) + 80;
}

// Altura del árbol
int AlturaArbol(Nodo *n) {
    if (n == NULL) return 0;
    int izq = AlturaArbol(n->Izq);
    int der = AlturaArbol(n->Der);
    return 1 + (izq > der ? izq : der);
}

// Límites izquierdo y derecho
void LimitesArbol(Nodo *n, int x, int nivel, int *minX, int *maxX) {
    if (n == NULL) return;
    if (x < *minX) *minX = x;
    if (x > *maxX) *maxX = x;
    
    int esp = 50;
    int aIzq = AnchoSubtree(n->Izq);
    int aDer = AnchoSubtree(n->Der);
    if (aIzq > 0 || aDer > 0) esp = (aIzq > aDer ? aIzq : aDer) + 20;
    if (esp < 50) esp = 50;
    
    LimitesArbol(n->Izq, x - esp, nivel + 1, minX, maxX);
    LimitesArbol(n->Der, x + esp, nivel + 1, minX, maxX);
}

// Dibuja líneas de división en el plano
void DibujarNodo(cairo_t *cr, int xi, int yi, int xf, int yf, Nodo *R) {
    if (R == NULL) return;
    
    int xx = R->P.x;
    int yy = R->P.y;
    
    // Línea divisora (azul)
    cairo_set_source_rgb(cr, 0.2, 0.4, 1);
    cairo_set_line_width(cr, 1);
    if (R->eje == 1) {
        cairo_move_to(cr, xi, yy);
        cairo_line_to(cr, xf, yy);
    } else {
        cairo_move_to(cr, xx, yi);
        cairo_line_to(cr, xx, yf);
    }
    cairo_stroke(cr);
    
    // Punto (rojo)
    cairo_set_source_rgb(cr, 1, 0, 0);
    cairo_arc(cr, xx, yy, 4, 0, 2 * G_PI);
    cairo_fill(cr);
    
    // Continuar con hijos
    if (R->eje == 0) {
        DibujarNodo(cr, xi, yi, xx, yf, R->Izq);
        DibujarNodo(cr, xx, yi, xf, yf, R->Der);
    } else {
        DibujarNodo(cr, xi, yi, xf, yy, R->Izq);
        DibujarNodo(cr, xi, yy, xf, yf, R->Der);
    }
}

// Dibuja estructura jerárquica del árbol
void DibujarArbolVisual(cairo_t *cr, Nodo *R, int x, int y, int nivel, int maxNivel) {
    if (R == NULL) return;
    
    int aIzq = AnchoSubtree(R->Izq);
    int aDer = AnchoSubtree(R->Der);
    int esp = 50;
    if (aIzq > 0 || aDer > 0) esp = (aIzq > aDer ? aIzq : aDer) + 20;
    if (esp < 50) esp = 50;
    
    // Nodo
    cairo_set_source_rgb(cr, 0, 0.5, 0.8);
    cairo_set_line_width(cr, 1);
    cairo_rectangle(cr, x - 15, y - 8, 30, 16);
    cairo_stroke(cr);
    
    // Texto
    char txt[32];
    sprintf(txt, "(%d,%d)", R->P.x, R->P.y);
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_set_font_size(cr, 8);
    cairo_move_to(cr, x - 18, y + 3);
    cairo_show_text(cr, txt);
    
    int dy = 50;
    
    // Hijo izquierdo
    if (R->Izq != NULL) {
        int xIzq = x - esp;
        int yIzq = y + dy;
        cairo_set_source_rgb(cr, 0, 0, 0);
        cairo_set_line_width(cr, 1);
        cairo_move_to(cr, x, y + 8);
        cairo_line_to(cr, xIzq, yIzq - 8);
        cairo_stroke(cr);
        DibujarArbolVisual(cr, R->Izq, xIzq, yIzq, nivel + 1, maxNivel);
    }
    
    // Hijo derecho
    if (R->Der != NULL) {
        int xDer = x + esp;
        int yDer = y + dy;
        cairo_set_source_rgb(cr, 0, 0, 0);
        cairo_set_line_width(cr, 1);
        cairo_move_to(cr, x, y + 8);
        cairo_line_to(cr, xDer, yDer - 8);
        cairo_stroke(cr);
        DibujarArbolVisual(cr, R->Der, xDer, yDer, nivel + 1, maxNivel);
    }
}