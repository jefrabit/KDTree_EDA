// =============================================================================
// KD-Tree Visualizador - GTK C (Versión modular)
// =============================================================================

#include <gtk/gtk.h>
#include <time.h>
#include "kdtree.h"
#include "drawing.h"

// =============================================================================
// Variables globales
// =============================================================================
KDTree Arbol;
int pxi = 0, pyi = 0, pxf = 400, pyf = 400;

// =============================================================================
// Callbacks
// =============================================================================
static gboolean dibujar_puntos(GtkWidget *widget, cairo_t *cr, gpointer data) {
    // Fondo blanco
    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_paint(cr);
    
    // Borde
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_set_line_width(cr, 2);
    cairo_rectangle(cr, pxi, pyi, pxf - pxi, pyf - pyi);
    cairo_stroke(cr);
    
    // Dibujar árbol
    DibujarNodo(cr, pxi, pyi, pxf, pyf, Arbol.Raiz);
    
    return FALSE;
}

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
    
    // Calcular tamaño necesario del árbol según altura
    int altura = AlturaArbol(Arbol.Raiz);
    int ancho = 200; // Base
    for (int i = 0; i < altura; i++) {
        int espaciado = 180 >> i;
        if (espaciado < 30) espaciado = 30;
        ancho += espaciado * 2;
    }
    if (ancho < 600) ancho = 600;
    
    int alturaDrawing = altura * 70 + 100;
    if (alturaDrawing < 400) alturaDrawing = 400;
    
    // Ajustar tamaño del drawing area
    gtk_widget_set_size_request(widget, ancho, alturaDrawing);
    
    // Forzar actualización del widget
    gtk_widget_size_allocate(widget, &(GtkAllocation){0, 0, ancho, alturaDrawing});
    
    // Obtener el scrolled window padre
    GtkWidget *parent = gtk_widget_get_parent(widget);
    if (parent && GTK_IS_SCROLLED_WINDOW(parent)) {
        // Obtener adjustments
        GtkAdjustment *hadj = gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(parent));
        GtkAdjustment *vadj = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(parent));
        
        // Calcular posición para centrar
        gdouble max_h = gtk_adjustment_get_upper(hadj) - gtk_adjustment_get_page_size(hadj);
        gdouble max_v = gtk_adjustment_get_upper(vadj) - gtk_adjustment_get_page_size(vadj);
        
        // Posicionar al centro
        gtk_adjustment_set_value(hadj, max_h / 2);
        gtk_adjustment_set_value(vadj, max_v / 2);
    }
    
    // Calcular altura del árbol para pasar como nivel máximo
    int alturaArbol = AlturaArbol(Arbol.Raiz);
    
    // Dibujar desde el centro del área visible, nivel 0
    DibujarArbolVisual(cr, Arbol.Raiz, ancho / 2, 40, 0, alturaArbol);
    
    return FALSE;
}

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

static gboolean evento_mover(GtkWidget *widget, GdkEventMotion *event, gpointer data) {
    GtkLabel *labelCoords = GTK_LABEL(data);
    char buffer[64];
    sprintf(buffer, "Coordenadas: (%d, %d)", (int)event->x, (int)event->y);
    gtk_label_set_text(labelCoords, buffer);
    return TRUE;
}

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

static void limpiar_arbol(GtkWidget *widget, gpointer data) {
    GtkWidget *da1 = g_object_get_data(G_OBJECT(data), "da1");
    GtkWidget *da2 = g_object_get_data(G_OBJECT(data), "da2");
    
    LiberarArbol(Arbol.Raiz);
    Inicializar(&Arbol);
    
    gtk_widget_queue_draw(da1);
    gtk_widget_queue_draw(da2);
}

// =============================================================================
// Main
// =============================================================================
int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    
    Inicializar(&Arbol);
    
    // Ventana
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "KD-Tree Visualizador");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 500);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    
    // Contenedores
    GtkWidget *hbox = gtk_hbox_new(FALSE, 10);
    gtk_container_add(GTK_CONTAINER(window), hbox);
    
    // Panel izquierdo
    GtkWidget *vboxIzq = gtk_vbox_new(FALSE, 5);
    gtk_box_pack_start(GTK_BOX(hbox), vboxIzq, TRUE, TRUE, 10);
    
    GtkWidget *labelCoords = gtk_label_new("Coordenadas: (0, 0)");
    gtk_misc_set_alignment(GTK_MISC(labelCoords), 0, 0.5);
    gtk_box_pack_start(GTK_BOX(vboxIzq), labelCoords, FALSE, FALSE, 0);
    
    GtkWidget *drawingarea1 = gtk_drawing_area_new();
    gtk_widget_set_size_request(drawingarea1, 400, 400);
    gtk_box_pack_start(GTK_BOX(vboxIzq), drawingarea1, TRUE, TRUE, 0);
    
    // Panel derecho
    GtkWidget *vboxDer = gtk_vbox_new(FALSE, 5);
    gtk_box_pack_start(GTK_BOX(hbox), vboxDer, TRUE, TRUE, 10);
    
    gtk_box_pack_start(GTK_BOX(vboxDer), gtk_label_new("Estructura del Arbol"), FALSE, FALSE, 0);
    
    GtkWidget *btnRandom = gtk_button_new_with_label("Generar 10 puntos");
    gtk_box_pack_start(GTK_BOX(vboxDer), btnRandom, FALSE, FALSE, 0);
    
    GtkWidget *btnLimpiar = gtk_button_new_with_label("Limpiar");
    gtk_box_pack_start(GTK_BOX(vboxDer), btnLimpiar, FALSE, FALSE, 0);
    
    // ScrolledWindow para el árbol (horizontal scrollable)
    GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled), GTK_POLICY_ALWAYS, GTK_POLICY_ALWAYS);
    gtk_scrolled_window_set_min_content_width(GTK_SCROLLED_WINDOW(scrolled), 500);
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scrolled), 400);
    gtk_box_pack_start(GTK_BOX(vboxDer), scrolled, TRUE, TRUE, 0);
    
    // Drawing area más grande para permitir scrolling
    GtkWidget *drawingarea2 = gtk_drawing_area_new();
    gtk_widget_set_size_request(drawingarea2, 2000, 1500);
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
