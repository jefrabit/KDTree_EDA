#include <gtk/gtk.h>
#include <time.h>
#include "kdtree.h"
#include "dibujo.h"

KDTree arb;
int pxi = 0, pyi = 0, pxf = 400, pyf = 400;

static gboolean dibujar(GtkWidget *w, cairo_t *cr, gpointer d) {
    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_paint(cr);
    
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_set_line_width(cr, 2);
    cairo_rectangle(cr, pxi, pyi, pxf - pxi, pyf - pyi);
    cairo_stroke(cr);
    
    Dibujar(arb.raiz, cr, pxi, pyi, pxf, pyf);
    return FALSE;
}

static gboolean dibujar2(GtkWidget *w, cairo_t *cr, gpointer d) {
    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_paint(cr);
    
    if (arb.raiz == NULL) {
        cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
        cairo_set_font_size(cr, 16);
        cairo_move_to(cr, 100, 150);
        cairo_show_text(cr, "Vacio");
        return FALSE;
    }
    
    int alt = Altura(arb.raiz);
    int minX = 1000000, maxX = -1000000;
    Limites(arb.raiz, 0, 0, &minX, &maxX);
    
    if (minX == 1000000) { minX = 0; maxX = 600; }
    
    int ancho = (maxX - minX) + 400;
    if (ancho < 800) ancho = 800;
    int alto = alt * 60 + 150;
    if (alto < 500) alto = 500;
    
    int iniX = 150 - minX;
    
    gtk_widget_set_size_request(w, ancho, alto);
    gtk_widget_size_allocate(w, &(GtkAllocation){0, 0, ancho, alto});
    
    DibujarArbol(arb.raiz, cr, iniX, 40, 0);
    
    return FALSE;
}

static gboolean click(GtkWidget *w, GdkEventButton *e, gpointer d) {
    GtkWidget *d1 = GTK_WIDGET(d);
    GtkWidget *d2 = g_object_get_data(G_OBJECT(d), "d2");
    GtkLabel *lbl = g_object_get_data(G_OBJECT(d), "lbl");
    
    int x = (int)e->x;
    int y = (int)e->y;
    
    char buf[64];
    sprintf(buf, "Coords: (%d, %d)", x, y);
    gtk_label_set_text(lbl, buf);
    
    if (x < pxf && x > pxi && y < pyf && y > pyi) {
        Punto p = {x, y};
        Insertar(&arb, p);
        gtk_widget_queue_draw(d1);
        gtk_widget_queue_draw(d2);
    }
    return TRUE;
}

static gboolean mover(GtkWidget *w, GdkEventMotion *e, gpointer d) {
    GtkLabel *lbl = GTK_LABEL(d);
    char buf[64];
    sprintf(buf, "Coords: (%d, %d)", (int)e->x, (int)e->y);
    gtk_label_set_text(lbl, buf);
    return TRUE;
}

static void gen_aleat(GtkWidget *w, gpointer d) {
    GtkWidget *d1 = g_object_get_data(G_OBJECT(d), "d1");
    GtkWidget *d2 = g_object_get_data(G_OBJECT(d), "d2");
    
    Liberar(arb.raiz);
    Inicializar(&arb);
    
    srand(time(NULL));
    for (int i = 0; i < 10; i++) {
        int x = pxi + 10 + rand() % (pxf - pxi - 20);
        int y = pyi + 10 + rand() % (pyf - pyi - 20);
        Punto p = {x, y};
        Insertar(&arb, p);
    }
    gtk_widget_queue_draw(d1);
    gtk_widget_queue_draw(d2);
}

static void limpiar(GtkWidget *w, gpointer d) {
    GtkWidget *d1 = g_object_get_data(G_OBJECT(d), "d1");
    GtkWidget *d2 = g_object_get_data(G_OBJECT(d), "d2");
    Liberar(arb.raiz);
    Inicializar(&arb);
    gtk_widget_queue_draw(d1);
    gtk_widget_queue_draw(d2);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    Inicializar(&arb);
    
    GtkWidget *win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(win), "KD-Tree 2D");
    gtk_window_set_default_size(GTK_WINDOW(win), 800, 500);
    g_signal_connect(win, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    
    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_container_add(GTK_CONTAINER(win), hbox);
    
    GtkWidget *vbox1 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(hbox), vbox1, TRUE, TRUE, 10);
    
    GtkWidget *lbl = gtk_label_new("Coords: (0, 0)");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(vbox1), lbl, FALSE, FALSE, 0);
    
    GtkWidget *area1 = gtk_drawing_area_new();
    gtk_widget_set_size_request(area1, 400, 400);
    gtk_box_pack_start(GTK_BOX(vbox1), area1, TRUE, TRUE, 0);
    
    GtkWidget *vbox2 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(hbox), vbox2, TRUE, TRUE, 10);
    
    gtk_box_pack_start(GTK_BOX(vbox2), gtk_label_new("Estructura"), FALSE, FALSE, 0);
    
    GtkWidget *btn1 = gtk_button_new_with_label("Aleatorio");
    gtk_box_pack_start(GTK_BOX(vbox2), btn1, FALSE, FALSE, 0);
    
    GtkWidget *btn2 = gtk_button_new_with_label("Limpiar");
    gtk_box_pack_start(GTK_BOX(vbox2), btn2, FALSE, FALSE, 0);
    
    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_ALWAYS, GTK_POLICY_ALWAYS);
    gtk_box_pack_start(GTK_BOX(vbox2), scroll, TRUE, TRUE, 0);
    
    GtkWidget *area2 = gtk_drawing_area_new();
    gtk_widget_set_size_request(area2, 800, 500);
    gtk_container_add(GTK_CONTAINER(scroll), area2);
    
    g_object_set_data(G_OBJECT(area1), "d2", area2);
    g_object_set_data(G_OBJECT(area1), "lbl", lbl);
    g_object_set_data(G_OBJECT(btn1), "d1", area1);
    g_object_set_data(G_OBJECT(btn1), "d2", area2);
    g_object_set_data(G_OBJECT(btn2), "d1", area1);
    g_object_set_data(G_OBJECT(btn2), "d2", area2);
    
    g_signal_connect(area1, "draw", G_CALLBACK(dibujar), NULL);
    g_signal_connect(area2, "draw", G_CALLBACK(dibujar2), NULL);
    g_signal_connect(area1, "button-press-event", G_CALLBACK(click), area1);
    g_signal_connect(area1, "motion-notify-event", G_CALLBACK(mover), lbl);
    g_signal_connect(btn1, "clicked", G_CALLBACK(gen_aleat), btn1);
    g_signal_connect(btn2, "clicked", G_CALLBACK(limpiar), btn2);
    
    gtk_widget_set_events(area1, GDK_BUTTON_PRESS_MASK | GDK_POINTER_MOTION_MASK);
    
    gtk_widget_show_all(win);
    gtk_main();
    return 0;
}