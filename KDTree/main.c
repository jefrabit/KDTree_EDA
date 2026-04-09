// =============================================================================
// KD-Tree Visualizador - GTK
// =============================================================================

#include <gtk/gtk.h>
#include <time.h>
#include "kdtree.h"
#include "dibujo.h"

KDTree Arbol;
int pxi = 0, pyi = 0, pxf = 400, pyf = 400;

// Dibuja puntos y líneas de división en el panel izquierdo
static gboolean dibujar_puntos(GtkWidget *widget, cairo_t *cr, gpointer data) {
    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_paint(cr);
    
    // Borde
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_set_line_width(cr, 2);
    cairo_rectangle(cr, pxi, pyi, pxf - pxi, pyf - pyi);
    cairo_stroke(cr);
    
    DibujarNodo(cr, pxi, pyi, pxf, pyf, Arbol.Raiz);
    
    return FALSE;
}

// Dibuja estructura del árbol en el panel derecho
static gboolean dibujar_arbol(GtkWidget *widget, cairo_t *cr, gpointer data) {
    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_paint(cr);
    
    if (Arbol.Raiz == NULL) {
        cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
        cairo_set_font_size(cr, 16);
        cairo_move_to(cr, 100, 150);
        cairo_show_text(cr, "Arbol vacio");
        return FALSE;
    }
    
    // Calcular tamaño del lienzo
    int altura = AlturaArbol(Arbol.Raiz);
    int minX = 1000000, maxX = -1000000;
    LimitesArbol(Arbol.Raiz, 0, 0, &minX, &maxX);
    
    if (minX == 1000000) {
        minX = 0;
        maxX = 600;
    }
    
    int ancho = (maxX - minX) + 400;
    if (ancho < 800) ancho = 800;
    
    int alturaDibujo = altura * 60 + 150;
    if (alturaDibujo < 500) alturaDibujo = 500;
    
    int inicioX = 150 - minX;
    
    gtk_widget_set_size_request(widget, ancho, alturaDibujo);
    gtk_widget_size_allocate(widget, &(GtkAllocation){0, 0, ancho, alturaDibujo});
    
    DibujarArbolVisual(cr, Arbol.Raiz, inicioX, 40, 0, altura);
    
    return FALSE;
}

// Click para agregar punto
static gboolean evento_click(GtkWidget *widget, GdkEventButton *event, gpointer data) {
    GtkWidget *da1 = GTK_WIDGET(data);
    GtkWidget *da2 = g_object_get_data(G_OBJECT(data), "da2");
    GtkLabel *labelCoords = g_object_get_data(G_OBJECT(data), "labelCoords");
    
    int x = (int)event->x;
    int y = (int)event->y;
    
    char buffer[64];
    sprintf(buffer, "Coordenadas: (%d, %d)", x, y);
    gtk_label_set_text(labelCoords, buffer);
    
    if (x < pxf && x > pxi && y < pyf && y > pyi) {
        Punto p = {x, y};
        Insertar(&Arbol, p);
        gtk_widget_queue_draw(da1);
        gtk_widget_queue_draw(da2);
    }
    
    return TRUE;
}

// Movimiento del mouse
static gboolean evento_mover(GtkWidget *widget, GdkEventMotion *event, gpointer data) {
    GtkLabel *labelCoords = GTK_LABEL(data);
    char buffer[64];
    sprintf(buffer, "Coordenadas: (%d, %d)", (int)event->x, (int)event->y);
    gtk_label_set_text(labelCoords, buffer);
    return TRUE;
}

// Generar puntos aleatorios
static void generar_aleatorios(GtkWidget *widget, gpointer data) {
    GtkWidget *da1 = g_object_get_data(G_OBJECT(data), "da1");
    GtkWidget *da2 = g_object_get_data(G_OBJECT(data), "da2");
    
    LiberarArbol(Arbol.Raiz);
    Inicializar(&Arbol);
    
    srand(time(NULL));
    for (int i = 0; i < 10; i++) {
        int x = pxi + 10 + rand() % (pxf - pxi - 20);
        int y = pyi + 10 + rand() % (pyf - pyi - 20);
        Punto p = {x, y};
        Insertar(&Arbol, p);
    }
    
    gtk_widget_queue_draw(da1);
    gtk_widget_queue_draw(da2);
}

// Limpiar árbol
static void limpiar_arbol(GtkWidget *widget, gpointer data) {
    GtkWidget *da1 = g_object_get_data(G_OBJECT(data), "da1");
    GtkWidget *da2 = g_object_get_data(G_OBJECT(data), "da2");
    
    LiberarArbol(Arbol.Raiz);
    Inicializar(&Arbol);
    
    gtk_widget_queue_draw(da1);
    gtk_widget_queue_draw(da2);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    
    Inicializar(&Arbol);
    
    // Ventana principal
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "KD-Tree Visualizador");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 500);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    
    // Contenedor horizontal
    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_container_add(GTK_CONTAINER(window), hbox);
    
    // Panel izquierdo
    GtkWidget *vboxIzq = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(hbox), vboxIzq, TRUE, TRUE, 10);
    
    GtkWidget *labelCoords = gtk_label_new("Coordenadas: (0, 0)");
    gtk_widget_set_halign(labelCoords, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(vboxIzq), labelCoords, FALSE, FALSE, 0);
    
    GtkWidget *drawingarea1 = gtk_drawing_area_new();
    gtk_widget_set_size_request(drawingarea1, 400, 400);
    gtk_box_pack_start(GTK_BOX(vboxIzq), drawingarea1, TRUE, TRUE, 0);
    
    // Panel derecho
    GtkWidget *vboxDer = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(hbox), vboxDer, TRUE, TRUE, 10);
    
    gtk_box_pack_start(GTK_BOX(vboxDer), gtk_label_new("Estructura del Arbol"), FALSE, FALSE, 0);
    
    GtkWidget *btnRandom = gtk_button_new_with_label("Generar 10 puntos");
    gtk_box_pack_start(GTK_BOX(vboxDer), btnRandom, FALSE, FALSE, 0);
    
    GtkWidget *btnLimpiar = gtk_button_new_with_label("Limpiar");
    gtk_box_pack_start(GTK_BOX(vboxDer), btnLimpiar, FALSE, FALSE, 0);
    
    // ScrolledWindow para el árbol
    GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled), GTK_POLICY_ALWAYS, GTK_POLICY_ALWAYS);
    gtk_box_pack_start(GTK_BOX(vboxDer), scrolled, TRUE, TRUE, 0);
    
    GtkWidget *drawingarea2 = gtk_drawing_area_new();
    gtk_widget_set_size_request(drawingarea2, 800, 500);
    gtk_container_add(GTK_CONTAINER(scrolled), drawingarea2);
    
    // Datos para callbacks
    g_object_set_data(G_OBJECT(drawingarea1), "da2", drawingarea2);
    g_object_set_data(G_OBJECT(drawingarea1), "labelCoords", labelCoords);
    g_object_set_data(G_OBJECT(btnRandom), "da1", drawingarea1);
    g_object_set_data(G_OBJECT(btnRandom), "da2", drawingarea2);
    g_object_set_data(G_OBJECT(btnLimpiar), "da1", drawingarea1);
    g_object_set_data(G_OBJECT(btnLimpiar), "da2", drawingarea2);
    
    // Señales
    g_signal_connect(drawingarea1, "draw", G_CALLBACK(dibujar_puntos), NULL);
    g_signal_connect(drawingarea2, "draw", G_CALLBACK(dibujar_arbol), NULL);
    g_signal_connect(drawingarea1, "button-press-event", G_CALLBACK(evento_click), drawingarea1);
    g_signal_connect(drawingarea1, "motion-notify-event", G_CALLBACK(evento_mover), labelCoords);
    g_signal_connect(btnRandom, "clicked", G_CALLBACK(generar_aleatorios), btnRandom);
    g_signal_connect(btnLimpiar, "clicked", G_CALLBACK(limpiar_arbol), btnLimpiar);
    
    gtk_widget_set_events(drawingarea1, GDK_BUTTON_PRESS_MASK | GDK_POINTER_MOTION_MASK);
    
    gtk_widget_show_all(window);
    gtk_main();
    
    return 0;
}