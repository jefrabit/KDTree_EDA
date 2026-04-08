// =============================================================================
// Drawing Implementation - Funciones de dibujo del KD-Tree
// =============================================================================

#include "drawing.h"

// =============================================================================
// Funciones aux para calcular dimensiones del árbol
// =============================================================================
int AnchoSubtree(Nodo *n) {
    if (n == NULL) return 0;
    if (n->Izq == NULL && n->Der == NULL) return 60;
    
    int izq = AnchoSubtree(n->Izq);
    int der = AnchoSubtree(n->Der);
    
    return izq + der + 60;
}

int AlturaArbol(Nodo *n) {
    if (n == NULL) return 0;
    int izq = AlturaArbol(n->Izq);
    int der = AlturaArbol(n->Der);
    return 1 + (izq > der ? izq : der);
}

// =============================================================================
// Funciones de dibujo
// =============================================================================
void DibujarNodo(cairo_t *cr, int xi, int yi, int xf, int yf, Nodo *R) {
    if (R == NULL) return;
    
    int xx = R->P.x;
    int yy = R->P.y;
    
    // Línea divisora
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
    
    // Punto
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

void DibujarArbolVisual(cairo_t *cr, Nodo *R, int x, int y, int nivel, int maxNivel) {
    if (R == NULL) return;
    
    // El espaciado se reduce en cada nivel (factor de 2)
    int espaciado = 180 >> nivel;
    if (espaciado < 30) espaciado = 30;
    
    // Rectángulo del nodo
    cairo_set_source_rgb(cr, 0, 0.5, 0.8);
    cairo_set_line_width(cr, 1);
    cairo_rectangle(cr, x - 25, y - 12, 50, 24);
    cairo_stroke(cr);
    
    // Texto del punto
    char texto[32];
    sprintf(texto, "(%d,%d)", R->P.x, R->P.y);
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_set_font_size(cr, 10);
    cairo_move_to(cr, x - 30, y + 4);
    cairo_show_text(cr, texto);
    
    int dy = 55;
    
    // Hijo izquierdo
    if (R->Izq != NULL) {
        int xIzq = x - espaciado;
        int yIzq = y + dy;
        cairo_set_source_rgb(cr, 0, 0, 0);
        cairo_set_line_width(cr, 1);
        cairo_move_to(cr, x, y + 12);
        cairo_line_to(cr, xIzq, yIzq - 12);
        cairo_stroke(cr);
        DibujarArbolVisual(cr, R->Izq, xIzq, yIzq, nivel + 1, maxNivel);
    }
    
    // Hijo derecho
    if (R->Der != NULL) {
        int xDer = x + espaciado;
        int yDer = y + dy;
        cairo_set_source_rgb(cr, 0, 0, 0);
        cairo_set_line_width(cr, 1);
        cairo_move_to(cr, x, y + 12);
        cairo_line_to(cr, xDer, yDer - 12);
        cairo_stroke(cr);
        DibujarArbolVisual(cr, R->Der, xDer, yDer, nivel + 1, maxNivel);
    }
}
