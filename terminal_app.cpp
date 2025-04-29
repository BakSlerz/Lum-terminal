#include <gtk/gtk.h>
#include <vte/vte.h>
#include <stdlib.h>

class TerminalWindow {
public:
    TerminalWindow() {
        // Inicjalizacja GTK
        gtk_init(nullptr, nullptr);

        // Tworzenie głównego okna
        window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gtk_window_set_title(GTK_WINDOW(window), "Lum Terminal");
        gtk_window_set_default_size(GTK_WINDOW(window), 800, 500);
        gtk_window_set_role(GTK_WINDOW(window), "lum-terminal");  // Rola okna
        gtk_window_set_wmclass(GTK_WINDOW(window), "LumTerminal", "Lum Terminal");  // Nazwa klasy aplikacji

        g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), nullptr);

        // Tworzenie kontenera
        box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
        gtk_container_add(GTK_CONTAINER(window), box);

        // Tworzenie terminala
        terminal = vte_terminal_new();
        gtk_box_pack_start(GTK_BOX(box), terminal, TRUE, TRUE, 0);

        // Ustawienie czcionki
        PangoFontDescription *font_desc = pango_font_description_from_string("Monospace 11");
        vte_terminal_set_font(VTE_TERMINAL(terminal), font_desc);
        pango_font_description_free(font_desc);

        // Ustawienie kolorów
        GdkRGBA fg_color = {0.85, 0.9, 0.94, 1}; // Kolor tekstu
        GdkRGBA bg_color = {0.1, 0.1, 0.1, 1};   // Kolor tła
        vte_terminal_set_colors(VTE_TERMINAL(terminal), &fg_color, &bg_color, NULL, 0);

        // Obsługa prawego kliknięcia
        g_signal_connect(terminal, "button-press-event", G_CALLBACK(on_right_click), nullptr);

        // Uruchomienie powłoki
        spawn_shell(VTE_TERMINAL(terminal));

        // Wyświetlenie okna
        gtk_widget_show_all(window);

        // Uruchomienie głównej pętli GTK
        gtk_main();
    }

    ~TerminalWindow() {}

private:
    GtkWidget *window;
    GtkWidget *box;
    GtkWidget *terminal;

    void spawn_shell(VteTerminal *terminal) {
        const gchar *shell = getenv("SHELL");
        if (shell == nullptr) {
            shell = "/bin/bash";
        }

        gchar *envp[] = {nullptr};

        vte_terminal_spawn_async(
            terminal,
            VTE_PTY_DEFAULT,
            nullptr,
            (char *[]){(char*)shell, nullptr},
            envp,
            G_SPAWN_DEFAULT,
            nullptr,
            nullptr,
            nullptr,
            -1,
            nullptr,
            nullptr,
            nullptr
        );
    }

    static gboolean on_right_click(GtkWidget *widget, GdkEventButton *event, gpointer data) {
        if (event->button == 3) {
            GtkWidget *menu, *item_copy, *item_paste;

            menu = gtk_menu_new();

            item_copy = gtk_menu_item_new_with_label("Copy");
            g_signal_connect(item_copy, "activate", G_CALLBACK(vte_terminal_copy_clipboard), widget);
            gtk_menu_shell_append(GTK_MENU_SHELL(menu), item_copy);

            item_paste = gtk_menu_item_new_with_label("Paste");
            g_signal_connect(item_paste, "activate", G_CALLBACK(vte_terminal_paste_clipboard), widget);
            gtk_menu_shell_append(GTK_MENU_SHELL(menu), item_paste);

            gtk_widget_show_all(menu);
            gtk_menu_popup_at_pointer(GTK_MENU(menu), (GdkEvent*)event);
        }
        return FALSE;
    }
};

int main() {
    TerminalWindow terminal_window;
    return 0;
}
