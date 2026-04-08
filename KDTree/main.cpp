#include <gtkmm.h>

class VentanaPrincipal : public Gtk::Window {
public:
    VentanaPrincipal() {
        set_title("Ejemplo KDTree");
        set_default_size(400, 300);

        // Crear un botón
        Gtk::Button* boton = Gtk::manage(new Gtk::Button("Presióname"));
        boton->signal_clicked().connect([]() {
            std::cout << "¡Botón presionado!" << std::endl;
        });

        // Añadir el botón a la ventana
        add(*boton);

        // Mostrar todos los widgets
        show_all_children();
    }
};

int main(int argc, char* argv[]) {
    // Inicializar GTK
    Gtk::Main kit(argc, argv);

    // Crear y ejecutar la ventana principal
    VentanaPrincipal ventana;
    Gtk::Main::run(ventana);

    return 0;
}