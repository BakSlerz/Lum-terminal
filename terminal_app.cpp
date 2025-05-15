#include <gtk/gtk.h>
#include <vte/vte.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <signal.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstdlib>
#include <filesystem>

// Color theme structure
struct ColorTheme {
    std::string name;
    GdkRGBA foreground;
    GdkRGBA background;
    GdkRGBA palette[16]; // Standard 16 color palette
    double transparency;
};

// Configuration structure
struct TerminalConfig {
    std::string font_family = "Monospace";
    double font_size = 11.0;
    double transparency = 0.0;
    std::string current_theme_name = "Default";
    std::map<std::string, ColorTheme> color_themes;
    
    // Returns path to configuration directory
    static std::string get_config_dir() {
        std::string config_dir = std::string(getenv("HOME")) + "/.config/lum-terminal";
        
        // Creating configuration directory if it doesn't exist
        struct stat st = {0};
        if (stat(config_dir.c_str(), &st) == -1) {
            mkdir(config_dir.c_str(), 0700);
        }
        
        // Creating themes directory if it doesn't exist
        std::string themes_dir = config_dir + "/themes";
        if (stat(themes_dir.c_str(), &st) == -1) {
            mkdir(themes_dir.c_str(), 0700);
        }
        
        return config_dir;
    }
    
    // Returns path to configuration file
    static std::string get_config_path() {
        return get_config_dir() + "/config.ini";
    }
    
    // Returns path to themes directory
    static std::string get_themes_dir() {
        return get_config_dir() + "/themes";
    }
    
    // Saving configuration to file
    void save_config() {
        // Save main configuration file
        std::ofstream config_file(get_config_path());
        if (!config_file.is_open()) {
            std::cerr << "Cannot open configuration file for writing." << std::endl;
            return;
        }
        
        // Set precision for floating point numbers
        config_file.precision(6);
        config_file << std::fixed;
        
        // Saving basic settings
        config_file << "[General]" << std::endl;
        config_file << "font_family=" << font_family << std::endl;
        config_file << "font_size=" << font_size << std::endl;
        config_file << "transparency=" << transparency << std::endl;
        config_file << "current_theme=" << current_theme_name << std::endl;
        
        config_file.close();
        
        // Save each theme as a separate file
        for (const auto& theme_pair : color_themes) {
            save_theme(theme_pair.second);
        }
        
        std::cerr << "Configuration saved to " << get_config_path() << std::endl;
    }
    
    // Inicjalizacja palety kolorów, jeśli jest pusta
    void initialize_palette_if_empty(ColorTheme *theme) {
        bool needs_initialization = false;
        
        // Sprawdź, czy pierwszy kolor palety ma zerowe wartości
        if (theme->palette[0].red == 0.0 && theme->palette[0].green == 0.0 && 
            theme->palette[0].blue == 0.0 && theme->palette[0].alpha == 0.0) {
            needs_initialization = true;
        }
        
        // Sprawdź, czy kolory foreground i background są czarne (0,0,0)
        if (theme->foreground.red == 0.0 && theme->foreground.green == 0.0 && 
            theme->foreground.blue == 0.0 && theme->foreground.alpha == 0.0) {
            // Ustaw domyślny kolor tekstu w zależności od nazwy motywu
            if (theme->name == "Light") {
                theme->foreground = {0.1, 0.1, 0.1, 1.0}; // Prawie czarny
            } else {
                theme->foreground = {0.8, 0.8, 0.8, 1.0}; // Jasny szary
            }
            std::cerr << "Inicjalizacja koloru tekstu dla motywu: " << theme->name << std::endl;
        }
        
        if (theme->background.red == 0.0 && theme->background.green == 0.0 && 
            theme->background.blue == 0.0 && theme->background.alpha == 0.0) {
            // Ustaw domyślny kolor tła w zależności od nazwy motywu
            if (theme->name == "Light") {
                theme->background = {0.95, 0.95, 0.95, 1.0}; // Prawie biały
            } else {
                theme->background = {0.1, 0.1, 0.1, 1.0}; // Prawie czarny
            }
            std::cerr << "Inicjalizacja koloru tła dla motywu: " << theme->name << std::endl;
        }
        
        if (needs_initialization) {
            std::cerr << "Inicjalizacja pustej palety kolorów dla motywu: " << theme->name << std::endl;
            
            // Dostosuj paletę kolorów w zależności od motywu
            if (theme->name == "Light") {
                // Jaśniejsza paleta dla jasnego motywu
                theme->palette[0] = {0.2, 0.2, 0.2, 1.0};          // Czarny (jaśniejszy)
                theme->palette[1] = {0.8, 0.0, 0.0, 1.0};          // Czerwony
                theme->palette[2] = {0.0, 0.7, 0.0, 1.0};          // Zielony
                theme->palette[3] = {0.7, 0.7, 0.0, 1.0};          // Żółty
                theme->palette[4] = {0.0, 0.0, 0.8, 1.0};          // Niebieski
                theme->palette[5] = {0.7, 0.0, 0.7, 1.0};          // Magenta
                theme->palette[6] = {0.0, 0.7, 0.7, 1.0};          // Cyan
                theme->palette[7] = {0.7, 0.7, 0.7, 1.0};          // Biały
                theme->palette[8] = {0.5, 0.5, 0.5, 1.0};          // Jasny czarny (szary)
                theme->palette[9] = {1.0, 0.4, 0.4, 1.0};          // Jasny czerwony
                theme->palette[10] = {0.4, 1.0, 0.4, 1.0};         // Jasny zielony
                theme->palette[11] = {1.0, 1.0, 0.4, 1.0};         // Jasny żółty
                theme->palette[12] = {0.4, 0.4, 1.0, 1.0};         // Jasny niebieski
                theme->palette[13] = {1.0, 0.4, 1.0, 1.0};         // Jasny magenta
                theme->palette[14] = {0.4, 1.0, 1.0, 1.0};         // Jasny cyan
                theme->palette[15] = {1.0, 1.0, 1.0, 1.0};         // Jasny biały
            } else {
                // Standardowa paleta kolorów terminala
                theme->palette[0] = {0.0, 0.0, 0.0, 1.0};          // Czarny
                theme->palette[1] = {0.8, 0.0, 0.0, 1.0};          // Czerwony
                theme->palette[2] = {0.0, 0.8, 0.0, 1.0};          // Zielony
                theme->palette[3] = {0.8, 0.8, 0.0, 1.0};          // Żółty
                theme->palette[4] = {0.0, 0.0, 0.8, 1.0};          // Niebieski
                theme->palette[5] = {0.8, 0.0, 0.8, 1.0};          // Magenta
                theme->palette[6] = {0.0, 0.8, 0.8, 1.0};          // Cyan
                theme->palette[7] = {0.8, 0.8, 0.8, 1.0};          // Biały
                theme->palette[8] = {0.5, 0.5, 0.5, 1.0};          // Jasny czarny (szary)
                theme->palette[9] = {1.0, 0.0, 0.0, 1.0};          // Jasny czerwony
                theme->palette[10] = {0.0, 1.0, 0.0, 1.0};         // Jasny zielony
                theme->palette[11] = {1.0, 1.0, 0.0, 1.0};         // Jasny żółty
                theme->palette[12] = {0.0, 0.0, 1.0, 1.0};         // Jasny niebieski
                theme->palette[13] = {1.0, 0.0, 1.0, 1.0};         // Jasny magenta
                theme->palette[14] = {0.0, 1.0, 1.0, 1.0};         // Jasny cyan
                theme->palette[15] = {1.0, 1.0, 1.0, 1.0};         // Jasny biały
            }
        }
    }
    
    // Zapisywanie pojedynczego motywu do pliku
    void save_theme(const ColorTheme& theme) {
        // Tworzymy kopię motywu, aby móc ją zmodyfikować przed zapisem
        ColorTheme theme_copy = theme;
        
        // Upewnij się, że kolory są poprawnie zainicjalizowane przed zapisem
        initialize_palette_if_empty(&theme_copy);
        
        std::string theme_path = get_themes_dir() + "/" + theme_copy.name + ".theme";
        std::ofstream theme_file(theme_path);
        if (!theme_file.is_open()) {
            std::cerr << "Cannot open theme file for writing: " << theme_path << std::endl;
            return;
        }
        
        // Ustawienie precyzji dla liczb zmiennoprzecinkowych
        theme_file.precision(6);
        theme_file << std::fixed;
        
        // Zapisywanie podstawowych informacji o motywie
        theme_file << "[Theme]" << std::endl;
        theme_file << "name=" << theme_copy.name << std::endl;
        theme_file << "foreground=" << theme_copy.foreground.red << "," << theme_copy.foreground.green << "," 
                   << theme_copy.foreground.blue << "," << theme_copy.foreground.alpha << std::endl;
        theme_file << "background=" << theme_copy.background.red << "," << theme_copy.background.green << "," 
                   << theme_copy.background.blue << "," << theme_copy.background.alpha << std::endl;
        // Nie zapisujemy przezroczystości motywu, używamy globalnej przezroczystości
        // theme_file << "transparency=" << theme_copy.transparency << std::endl;
        
        // Zapisywanie palety kolorów
        theme_file << std::endl << "[Palette]" << std::endl;
        for (int i = 0; i < 16; i++) {
            theme_file << "color" << i << "=" 
                      << theme_copy.palette[i].red << "," 
                      << theme_copy.palette[i].green << "," 
                      << theme_copy.palette[i].blue << "," 
                      << theme_copy.palette[i].alpha << std::endl;
        }
        
        theme_file.close();
        std::cerr << "Motyw zapisany do " << theme_path << std::endl;
    }
    
    // Wczytywanie konfiguracji z pliku
    void load_config() {
        // Wczytaj główny plik konfiguracyjny
        std::ifstream config_file(get_config_path());
        if (config_file.is_open()) {
            std::string line;
            std::string current_section;
            
            while (std::getline(config_file, line)) {
                // Pomijanie pustych linii
                if (line.empty()) continue;
                
                // Sprawdzanie, czy linia jest sekcją
                if (line[0] == '[' && line[line.length() - 1] == ']') {
                    current_section = line.substr(1, line.length() - 2);
                    continue;
                }
                
                // Przetwarzanie par klucz=wartość
                size_t pos = line.find('=');
                if (pos != std::string::npos) {
                    std::string key = line.substr(0, pos);
                    std::string value = line.substr(pos + 1);
                    
                    if (current_section == "General") {
                        if (key == "font_family") {
                            font_family = value;
                        } else if (key == "font_size") {
                            font_size = std::stod(value);
                        } else if (key == "transparency") {
                            transparency = std::stod(value);
                        } else if (key == "current_theme") {
                            current_theme_name = value;
                        }
                    }
                }
            }
            
            config_file.close();
        } else {
            std::cerr << "Cannot open configuration file for reading. Using default settings." << std::endl;
        }
        
        // Wczytaj wszystkie motywy z katalogu themes
        load_themes();
    }
    
    // Wczytywanie wszystkich motywów z katalogu themes
    void load_themes() {
        DIR *dir;
        struct dirent *ent;
        std::string themes_dir = get_themes_dir();
        
        if ((dir = opendir(themes_dir.c_str())) != NULL) {
            while ((ent = readdir(dir)) != NULL) {
                std::string filename = ent->d_name;
                
                // Sprawdź, czy plik ma rozszerzenie .theme
                if (filename.length() > 6 && filename.substr(filename.length() - 6) == ".theme") {
                    std::string theme_path = themes_dir + "/" + filename;
                    load_theme(theme_path);
                }
            }
            closedir(dir);
        } else {
            std::cerr << "Cannot open themes directory: " << themes_dir << std::endl;
        }
    }
    
    // Wczytywanie pojedynczego motywu z pliku
    void load_theme(const std::string& theme_path) {
        std::ifstream theme_file(theme_path);
        if (!theme_file.is_open()) {
            std::cerr << "Cannot open theme file: " << theme_path << std::endl;
            return;
        }
        
        std::string line;
        std::string current_section;
        ColorTheme theme;
        
        while (std::getline(theme_file, line)) {
            // Pomijanie pustych linii
            if (line.empty()) continue;
            
            // Sprawdzanie, czy linia jest sekcją
            if (line[0] == '[' && line[line.length() - 1] == ']') {
                current_section = line.substr(1, line.length() - 2);
                continue;
            }
            
            // Przetwarzanie par klucz=wartość
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = line.substr(0, pos);
                std::string value = line.substr(pos + 1);
                
                if (current_section == "Theme") {
                    if (key == "name") {
                        theme.name = value;
                    } else if (key == "foreground") {
                        parse_color(value, theme.foreground);
                    } else if (key == "background") {
                        parse_color(value, theme.background);
                    } else if (key == "transparency") {
                        // Nie wczytujemy przezroczystości motywu, używamy globalnej przezroczystości
                        // theme.transparency = std::stod(value);
                        theme.transparency = transparency;
                    }
                } else if (current_section == "Palette") {
                    // Sprawdź, czy klucz ma format "colorN"
                    if (key.length() > 5 && key.substr(0, 5) == "color") {
                        int index = std::stoi(key.substr(5));
                        if (index >= 0 && index < 16) {
                            parse_color(value, theme.palette[index]);
                        }
                    }
                }
            }
        }
        
        theme_file.close();
        
        // Dodaj motyw do mapy, jeśli ma nazwę
        if (!theme.name.empty()) {
            color_themes[theme.name] = theme;
            std::cerr << "Wczytano motyw: " << theme.name << std::endl;
        }
    }
    
private:
    // Parsowanie koloru z formatu "r,g,b,a"
    void parse_color(const std::string& color_str, GdkRGBA& color) {
        std::istringstream ss(color_str);
        std::string token;
        
        if (std::getline(ss, token, ',')) {
            color.red = std::stod(token);
        }
        if (std::getline(ss, token, ',')) {
            color.green = std::stod(token);
        }
        if (std::getline(ss, token, ',')) {
            color.blue = std::stod(token);
        }
        if (std::getline(ss, token, ',')) {
            color.alpha = std::stod(token);
        }
    }
};

class TerminalTab {
public:
    GtkWidget *terminal;
    GtkWidget *label;
    GtkWidget *tab_container;
    GtkWidget *close_button;
    std::string title;
    GPid child_pid;

    TerminalTab(GtkNotebook *notebook, const std::string &title = "Terminal") : title(title), child_pid(0) {
        // Pola terminal, label, tab_container i close_button będą ustawione w add_new_tab
    }
    
    ~TerminalTab() {
        if (child_pid > 0) {
            kill(child_pid, SIGTERM);
        }
    }
};

class TerminalWindow {
public:
    TerminalWindow() {
        // Inicjalizacja GTK
        gtk_init(nullptr, nullptr);
        
        // Wczytanie konfiguracji
        config.load_config();
        
        // Tworzenie głównego okna
        window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gtk_window_set_title(GTK_WINDOW(window), "Lum Terminal");
        gtk_window_set_default_size(GTK_WINDOW(window), 800, 500);
        gtk_window_set_role(GTK_WINDOW(window), "lum-terminal");  // Rola okna
        gtk_window_set_wmclass(GTK_WINDOW(window), "LumTerminal", "Lum Terminal");  // Nazwa klasy aplikacji
        
        // Włączamy obsługę przezroczystości dla całego okna
        GdkScreen *screen = gtk_widget_get_screen(window);
        GdkVisual *visual = gdk_screen_get_rgba_visual(screen);
        if (visual != NULL) {
            gtk_widget_set_visual(window, visual);
        }
        gtk_widget_set_app_paintable(window, TRUE);

        g_signal_connect(window, "delete-event", G_CALLBACK(on_window_delete), this);
        g_signal_connect(window, "key-press-event", G_CALLBACK(on_key_press), this);

        // Tworzenie headerbar (pasek tytułowy w stylu GNOME)
        GtkWidget *headerbar = gtk_header_bar_new();
        gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(headerbar), TRUE);
        gtk_header_bar_set_title(GTK_HEADER_BAR(headerbar), "Lum Terminal");
        gtk_window_set_titlebar(GTK_WINDOW(window), headerbar);
        
        // Przycisk nowej zakładki
        GtkWidget *new_tab_button = gtk_button_new_from_icon_name("tab-new-symbolic", GTK_ICON_SIZE_BUTTON);
        gtk_widget_set_tooltip_text(new_tab_button, "New Tab");
        gtk_header_bar_pack_start(GTK_HEADER_BAR(headerbar), new_tab_button);
        g_signal_connect(new_tab_button, "clicked", G_CALLBACK(on_new_tab_clicked), this);
        
        // Przycisk menu
        GtkWidget *menu_button = gtk_menu_button_new();
        GtkWidget *menu_icon = gtk_image_new_from_icon_name("open-menu-symbolic", GTK_ICON_SIZE_BUTTON);
        gtk_button_set_image(GTK_BUTTON(menu_button), menu_icon);
        gtk_widget_set_tooltip_text(menu_button, "Menu");
        gtk_header_bar_pack_end(GTK_HEADER_BAR(headerbar), menu_button);
        
        // Przycisk wyszukiwania
        GtkWidget *search_button = gtk_button_new_from_icon_name("edit-find-symbolic", GTK_ICON_SIZE_BUTTON);
        gtk_widget_set_tooltip_text(search_button, "Search");
        gtk_header_bar_pack_end(GTK_HEADER_BAR(headerbar), search_button);
        g_signal_connect(search_button, "clicked", G_CALLBACK(on_search_clicked), this);
        
        // Tworzenie menu
        GtkWidget *main_menu = gtk_menu_new();
        
        // Opcja wyszukiwania
        GtkWidget *search_item = gtk_menu_item_new_with_label("Search");
        g_signal_connect(search_item, "activate", G_CALLBACK(on_search_clicked), this);
        gtk_menu_shell_append(GTK_MENU_SHELL(main_menu), search_item);
        
        // Separator
        GtkWidget *separator = gtk_separator_menu_item_new();
        gtk_menu_shell_append(GTK_MENU_SHELL(main_menu), separator);
        
        // Opcja motywów
        GtkWidget *theme_item = gtk_menu_item_new_with_label("Select Theme");
        g_signal_connect(theme_item, "activate", G_CALLBACK(on_theme_clicked), this);
        gtk_menu_shell_append(GTK_MENU_SHELL(main_menu), theme_item);
        
        // Opcja przezroczystości
        GtkWidget *transparency_item = gtk_menu_item_new_with_label("Adjust Transparency");
        g_signal_connect(transparency_item, "activate", G_CALLBACK(on_transparency_menu_clicked), this);
        gtk_menu_shell_append(GTK_MENU_SHELL(main_menu), transparency_item);
        
        // Opcja zmiany rozmiaru czcionki
        GtkWidget *font_size_item = gtk_menu_item_new_with_label("Font Size");
        g_signal_connect(font_size_item, "activate", G_CALLBACK(on_font_size_clicked), this);
        gtk_menu_shell_append(GTK_MENU_SHELL(main_menu), font_size_item);
        
        // Opcja zmiany czcionki
        GtkWidget *font_item = gtk_menu_item_new_with_label("Select Font");
        g_signal_connect(font_item, "activate", G_CALLBACK(on_font_clicked), this);
        gtk_menu_shell_append(GTK_MENU_SHELL(main_menu), font_item);
        
        // Opcja niestandardowych kolorów
        GtkWidget *custom_colors_item = gtk_menu_item_new_with_label("Custom Colors");
        g_signal_connect(custom_colors_item, "activate", G_CALLBACK(on_custom_colors_clicked), this);
        gtk_menu_shell_append(GTK_MENU_SHELL(main_menu), custom_colors_item);
        
        // Separator
        separator = gtk_separator_menu_item_new();
        gtk_menu_shell_append(GTK_MENU_SHELL(main_menu), separator);
        
        // Opcja zamknięcia zakładki
        GtkWidget *close_tab_item = gtk_menu_item_new_with_label("Close Tab");
        g_signal_connect(close_tab_item, "activate", G_CALLBACK(close_current_tab), this);
        gtk_menu_shell_append(GTK_MENU_SHELL(main_menu), close_tab_item);
        
        // Separator
        separator = gtk_separator_menu_item_new();
        gtk_menu_shell_append(GTK_MENU_SHELL(main_menu), separator);
        
        // Opcja About
        GtkWidget *about_item = gtk_menu_item_new_with_label("About");
        g_signal_connect(about_item, "activate", G_CALLBACK(on_about_clicked), this);
        gtk_menu_shell_append(GTK_MENU_SHELL(main_menu), about_item);
        
        // Wyświetlenie menu
        gtk_widget_show_all(main_menu);
        gtk_menu_button_set_popup(GTK_MENU_BUTTON(menu_button), main_menu);
        
        // Tworzenie głównego kontenera
        main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
        gtk_container_add(GTK_CONTAINER(window), main_box);

        // Tworzenie notebooka (zakładek)
        notebook = gtk_notebook_new();
        gtk_notebook_set_scrollable(GTK_NOTEBOOK(notebook), TRUE);
        gtk_notebook_set_show_tabs(GTK_NOTEBOOK(notebook), FALSE); // Ukryj pasek zakładek, gdy jest tylko jedna karta
        gtk_box_pack_start(GTK_BOX(main_box), notebook, TRUE, TRUE, 0);
        
        // Sygnał zmiany zakładki
        g_signal_connect(notebook, "switch-page", G_CALLBACK(on_tab_switch), this);

        // Inicjalizacja motywów kolorów
        initialize_color_themes();
        
        // Ustawienie aktualnego motywu z konfiguracji
        if (config.color_themes.find(config.current_theme_name) != config.color_themes.end()) {
            current_theme = &config.color_themes[config.current_theme_name];
        } else {
            current_theme = &config.color_themes["Default"];
        }

        // Dodanie pierwszej zakładki
        add_new_tab();

        // Wyświetlenie okna
        gtk_widget_show_all(window);

        // Uruchomienie głównej pętli GTK
        gtk_main();
    }

    ~TerminalWindow() {
        // Zapisanie konfiguracji przed zamknięciem
        config.save_config();
        
        for (auto tab : tabs) {
            delete tab;
        }
    }
    
    // Sprawdza, czy można bezpiecznie zamknąć okno
    bool can_close_window() {
        // Sprawdź, czy w którymkolwiek terminalu jest uruchomiony proces
        for (auto tab : tabs) {
            if (has_foreground_process(tab)) {
                GtkWidget *dialog = gtk_message_dialog_new(
                    GTK_WINDOW(window),
                    GTK_DIALOG_MODAL,
                    GTK_MESSAGE_QUESTION,
                    GTK_BUTTONS_YES_NO,
                    "There are processes running in terminal. Close anyway?");
                
                int response = gtk_dialog_run(GTK_DIALOG(dialog));
                gtk_widget_destroy(dialog);
                
                return (response == GTK_RESPONSE_YES);
            }
        }
        
        // Jeśli nie ma aktywnych procesów, można bezpiecznie zamknąć
        return true;
    }

private:
    GtkWidget *window;
    GtkWidget *main_box;
    GtkWidget *notebook;
    std::vector<TerminalTab*> tabs;
    TerminalConfig config;
    ColorTheme *current_theme;
    
    // Inicjalizacja palety kolorów, jeśli jest pusta
    void initialize_palette_if_empty(ColorTheme *theme) {
        bool needs_initialization = false;
        
        // Sprawdź, czy pierwszy kolor palety ma zerowe wartości
        if (theme->palette[0].red == 0.0 && theme->palette[0].green == 0.0 && 
            theme->palette[0].blue == 0.0 && theme->palette[0].alpha == 0.0) {
            needs_initialization = true;
        }
        
        // Sprawdź, czy kolory foreground i background są czarne (0,0,0)
        if (theme->foreground.red == 0.0 && theme->foreground.green == 0.0 && 
            theme->foreground.blue == 0.0 && theme->foreground.alpha == 0.0) {
            // Ustaw domyślny kolor tekstu w zależności od nazwy motywu
            if (theme->name == "Light") {
                theme->foreground = {0.1, 0.1, 0.1, 1.0}; // Prawie czarny
            } else {
                theme->foreground = {0.8, 0.8, 0.8, 1.0}; // Jasny szary
            }
            std::cerr << "Inicjalizacja koloru tekstu dla motywu: " << theme->name << std::endl;
        }
        
        if (theme->background.red == 0.0 && theme->background.green == 0.0 && 
            theme->background.blue == 0.0 && theme->background.alpha == 0.0) {
            // Ustaw domyślny kolor tła w zależności od nazwy motywu
            if (theme->name == "Light") {
                theme->background = {0.95, 0.95, 0.95, 1.0}; // Prawie biały
            } else {
                theme->background = {0.1, 0.1, 0.1, 1.0}; // Prawie czarny
            }
            std::cerr << "Inicjalizacja koloru tła dla motywu: " << theme->name << std::endl;
        }
        
        if (needs_initialization) {
            std::cerr << "Inicjalizacja pustej palety kolorów dla motywu: " << theme->name << std::endl;
            
            // Dostosuj paletę kolorów w zależności od motywu
            if (theme->name == "Light") {
                // Jaśniejsza paleta dla jasnego motywu
                theme->palette[0] = {0.2, 0.2, 0.2, 1.0};          // Czarny (jaśniejszy)
                theme->palette[1] = {0.8, 0.0, 0.0, 1.0};          // Czerwony
                theme->palette[2] = {0.0, 0.7, 0.0, 1.0};          // Zielony
                theme->palette[3] = {0.7, 0.7, 0.0, 1.0};          // Żółty
                theme->palette[4] = {0.0, 0.0, 0.8, 1.0};          // Niebieski
                theme->palette[5] = {0.7, 0.0, 0.7, 1.0};          // Magenta
                theme->palette[6] = {0.0, 0.7, 0.7, 1.0};          // Cyan
                theme->palette[7] = {0.7, 0.7, 0.7, 1.0};          // Biały
                theme->palette[8] = {0.5, 0.5, 0.5, 1.0};          // Jasny czarny (szary)
                theme->palette[9] = {1.0, 0.4, 0.4, 1.0};          // Jasny czerwony
                theme->palette[10] = {0.4, 1.0, 0.4, 1.0};         // Jasny zielony
                theme->palette[11] = {1.0, 1.0, 0.4, 1.0};         // Jasny żółty
                theme->palette[12] = {0.4, 0.4, 1.0, 1.0};         // Jasny niebieski
                theme->palette[13] = {1.0, 0.4, 1.0, 1.0};         // Jasny magenta
                theme->palette[14] = {0.4, 1.0, 1.0, 1.0};         // Jasny cyan
                theme->palette[15] = {1.0, 1.0, 1.0, 1.0};         // Jasny biały
            } else {
                // Standardowa paleta kolorów terminala
                theme->palette[0] = {0.0, 0.0, 0.0, 1.0};          // Czarny
                theme->palette[1] = {0.8, 0.0, 0.0, 1.0};          // Czerwony
                theme->palette[2] = {0.0, 0.8, 0.0, 1.0};          // Zielony
                theme->palette[3] = {0.8, 0.8, 0.0, 1.0};          // Żółty
                theme->palette[4] = {0.0, 0.0, 0.8, 1.0};          // Niebieski
                theme->palette[5] = {0.8, 0.0, 0.8, 1.0};          // Magenta
                theme->palette[6] = {0.0, 0.8, 0.8, 1.0};          // Cyan
                theme->palette[7] = {0.8, 0.8, 0.8, 1.0};          // Biały
                theme->palette[8] = {0.5, 0.5, 0.5, 1.0};          // Jasny czarny (szary)
                theme->palette[9] = {1.0, 0.0, 0.0, 1.0};          // Jasny czerwony
                theme->palette[10] = {0.0, 1.0, 0.0, 1.0};         // Jasny zielony
                theme->palette[11] = {1.0, 1.0, 0.0, 1.0};         // Jasny żółty
                theme->palette[12] = {0.0, 0.0, 1.0, 1.0};         // Jasny niebieski
                theme->palette[13] = {1.0, 0.0, 1.0, 1.0};         // Jasny magenta
                theme->palette[14] = {0.0, 1.0, 1.0, 1.0};         // Jasny cyan
                theme->palette[15] = {1.0, 1.0, 1.0, 1.0};         // Jasny biały
            }
        }
    }


    
    void show_transparency_dialog() {
        GtkWidget *dialog = gtk_dialog_new_with_buttons(
            "Adjust Transparency", GTK_WINDOW(window),
            (GtkDialogFlags)(GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT),
            "_Cancel", GTK_RESPONSE_CANCEL,
            "_Apply", GTK_RESPONSE_ACCEPT,
            NULL);
            
        GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
        gtk_container_set_border_width(GTK_CONTAINER(content_area), 10);
        
        GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
        GtkWidget *label = gtk_label_new("Transparency level:");
        GtkWidget *scale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0.0, 1.0, 0.05);
        gtk_range_set_value(GTK_RANGE(scale), config.transparency);
        gtk_widget_set_size_request(scale, 200, -1);
        
        gtk_box_pack_start(GTK_BOX(box), label, FALSE, FALSE, 0);
        gtk_box_pack_start(GTK_BOX(box), scale, TRUE, TRUE, 0);
        
        gtk_container_add(GTK_CONTAINER(content_area), box);
        gtk_widget_show_all(dialog);
        
        if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
            // Ustaw nową wartość przezroczystości
            double new_transparency = gtk_range_get_value(GTK_RANGE(scale));
            
            // Aktualizujemy tylko globalną przezroczystość
            config.transparency = new_transparency;
            
            // Zastosuj zmiany do wszystkich terminali
            apply_theme_to_all_terminals();
            
            // Zapisz konfigurację po zmianie
            config.save_config();
        }
        
        gtk_widget_destroy(dialog);
    }

    void add_new_tab(const std::string &title = "Terminal") {
        g_print("Tworzenie nowej zakładki...\n");
        
        // Tworzenie nowego terminala
        GtkWidget *terminal = vte_terminal_new();
        gtk_widget_show(terminal);
        
        // Tworzenie etykiety zakładki
        GtkWidget *tab_container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
        GtkWidget *label = gtk_label_new(title.c_str());
        gtk_box_pack_start(GTK_BOX(tab_container), label, TRUE, TRUE, 0);
        
        // Przycisk zamykania
        GtkWidget *close_button = gtk_button_new_from_icon_name("window-close-symbolic", GTK_ICON_SIZE_MENU);
        gtk_button_set_relief(GTK_BUTTON(close_button), GTK_RELIEF_NONE);
        gtk_box_pack_start(GTK_BOX(tab_container), close_button, FALSE, FALSE, 0);
        gtk_widget_show_all(tab_container);
        
        // Dodanie zakładki do notebooka
        int index = gtk_notebook_append_page(GTK_NOTEBOOK(notebook), terminal, tab_container);
        gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(notebook), terminal, TRUE);
        
        // Tworzenie obiektu TerminalTab
        TerminalTab *tab = new TerminalTab(GTK_NOTEBOOK(notebook), title);
        tab->terminal = terminal;
        tab->label = label;
        tab->tab_container = tab_container;
        tab->close_button = close_button;
        tabs.push_back(tab);
        
        // Sygnały dla terminala
        g_signal_connect(terminal, "button-press-event", G_CALLBACK(on_right_click), this);
        g_signal_connect(terminal, "child-exited", G_CALLBACK(on_terminal_exit), this);
        g_signal_connect(terminal, "window-title-changed", G_CALLBACK(on_title_changed), tab);
        g_signal_connect(close_button, "clicked", G_CALLBACK(on_tab_close_clicked), this);
        
        // Ustawienie czcionki z konfiguracji
        PangoFontDescription *font_desc = pango_font_description_from_string(config.font_family.c_str());
        pango_font_description_set_size(font_desc, (int)(config.font_size * PANGO_SCALE));
        vte_terminal_set_font(VTE_TERMINAL(terminal), font_desc);
        pango_font_description_free(font_desc);
        
        // Zastosowanie aktualnego motywu
        apply_theme_to_terminal(VTE_TERMINAL(terminal));
        
        // Uruchomienie powłoki
        spawn_shell(VTE_TERMINAL(terminal), &tab->child_pid);
        
        // Przełączenie na nową zakładkę
        gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), index);
        gtk_widget_grab_focus(terminal);
        
        // Pokaż pasek zakładek, jeśli jest więcej niż jedna karta
        if (tabs.size() > 1) {
            gtk_notebook_set_show_tabs(GTK_NOTEBOOK(notebook), TRUE);
        }
        
        g_print("Zakładka utworzona, indeks: %d\n", index);
    }

    void initialize_color_themes() {
        // Sprawdź, czy mamy już motywy w konfiguracji
        if (config.color_themes.empty()) {
            std::cerr << "Brak motywów w konfiguracji, tworzę domyślne motywy." << std::endl;
            
            // Domyślny motyw
            ColorTheme default_theme;
            default_theme.name = "Default";
            default_theme.foreground = {0.85, 0.9, 0.94, 1.0};
            default_theme.background = {0.1, 0.1, 0.1, 1.0};
            default_theme.transparency = config.transparency;  // Użyj przezroczystości z konfiguracji
            
            // Standardowa paleta kolorów dla terminala
            default_theme.palette[0] = {0.0, 0.0, 0.0, 1.0};         // Czarny
            default_theme.palette[1] = {0.8, 0.0, 0.0, 1.0};         // Czerwony
            default_theme.palette[2] = {0.0, 0.8, 0.0, 1.0};         // Zielony
            default_theme.palette[3] = {0.8, 0.8, 0.0, 1.0};         // Żółty
            default_theme.palette[4] = {0.0, 0.0, 0.8, 1.0};         // Niebieski
            default_theme.palette[5] = {0.8, 0.0, 0.8, 1.0};         // Magenta
            default_theme.palette[6] = {0.0, 0.8, 0.8, 1.0};         // Cyan
            default_theme.palette[7] = {0.8, 0.8, 0.8, 1.0};         // Biały
            default_theme.palette[8] = {0.5, 0.5, 0.5, 1.0};         // Jasny czarny
            default_theme.palette[9] = {1.0, 0.0, 0.0, 1.0};         // Jasny czerwony
            default_theme.palette[10] = {0.0, 1.0, 0.0, 1.0};        // Jasny zielony
            default_theme.palette[11] = {1.0, 1.0, 0.0, 1.0};        // Jasny żółty
            default_theme.palette[12] = {0.0, 0.0, 1.0, 1.0};        // Jasny niebieski
            default_theme.palette[13] = {1.0, 0.0, 1.0, 1.0};        // Jasny magenta
            default_theme.palette[14] = {0.0, 1.0, 1.0, 1.0};        // Jasny cyan
            default_theme.palette[15] = {1.0, 1.0, 1.0, 1.0};        // Jasny biały
            
            config.color_themes["Default"] = default_theme;
            
            // Motyw jasny
            ColorTheme light_theme;
            light_theme.name = "Light";
            light_theme.foreground = {0.1, 0.1, 0.1, 1.0};
            light_theme.background = {0.95, 0.95, 0.95, 1.0};
            light_theme.transparency = config.transparency;  // Użyj przezroczystości z konfiguracji
            
            // Kopiowanie palety z domyślnego motywu
            for (int i = 0; i < 16; i++) {
                light_theme.palette[i] = default_theme.palette[i];
            }
            
            config.color_themes["Light"] = light_theme;
            
            // Motyw Solarized Dark
            ColorTheme solarized_dark;
            solarized_dark.name = "Solarized Dark";
            solarized_dark.foreground = {0.51, 0.58, 0.59, 1.0};
            solarized_dark.background = {0.0, 0.17, 0.21, 1.0};
            solarized_dark.transparency = config.transparency;  // Użyj przezroczystości z konfiguracji
            
            // Paleta Solarized Dark
            solarized_dark.palette[0] = {0.03, 0.21, 0.26, 1.0};     // base03
            solarized_dark.palette[1] = {0.86, 0.20, 0.18, 1.0};     // red
            solarized_dark.palette[2] = {0.52, 0.60, 0.0, 1.0};      // green
            solarized_dark.palette[3] = {0.71, 0.54, 0.0, 1.0};      // yellow
            solarized_dark.palette[4] = {0.15, 0.55, 0.82, 1.0};     // blue
            solarized_dark.palette[5] = {0.83, 0.21, 0.51, 1.0};     // magenta
            solarized_dark.palette[6] = {0.16, 0.63, 0.60, 1.0};     // cyan
            solarized_dark.palette[7] = {0.58, 0.63, 0.63, 1.0};     // base0
            solarized_dark.palette[8] = {0.40, 0.48, 0.51, 1.0};     // base00
            solarized_dark.palette[9] = {0.96, 0.30, 0.28, 1.0};     // orange
            solarized_dark.palette[10] = {0.62, 0.70, 0.0, 1.0};     // bright green
            solarized_dark.palette[11] = {0.81, 0.64, 0.0, 1.0};     // bright yellow
            solarized_dark.palette[12] = {0.25, 0.65, 0.92, 1.0};    // bright blue
            solarized_dark.palette[13] = {0.93, 0.31, 0.61, 1.0};    // bright magenta
            solarized_dark.palette[14] = {0.26, 0.73, 0.70, 1.0};    // bright cyan
            solarized_dark.palette[15] = {0.93, 0.91, 0.84, 1.0};    // base3
            
            config.color_themes["Solarized Dark"] = solarized_dark;
            
            // Motyw Matrix
            ColorTheme matrix;
            matrix.name = "Matrix";
            matrix.foreground = {0.0, 0.9, 0.0, 1.0};
            matrix.background = {0.0, 0.1, 0.0, 1.0};
            matrix.transparency = config.transparency;  // Użyj przezroczystości z konfiguracji
            
            // Paleta Matrix
            matrix.palette[0] = {0.0, 0.1, 0.0, 1.0};      // Ciemny zielony
            matrix.palette[1] = {0.0, 0.5, 0.0, 1.0};      // Średni zielony
            matrix.palette[2] = {0.0, 0.8, 0.0, 1.0};      // Jasny zielony
            matrix.palette[3] = {0.0, 1.0, 0.0, 1.0};      // Bardzo jasny zielony
            matrix.palette[4] = {0.0, 0.3, 0.0, 1.0};      // Ciemny zielony
            matrix.palette[5] = {0.0, 0.6, 0.0, 1.0};      // Średni zielony
            matrix.palette[6] = {0.0, 0.9, 0.0, 1.0};      // Jasny zielony
            matrix.palette[7] = {0.2, 1.0, 0.2, 1.0};      // Bardzo jasny zielony
            matrix.palette[8] = {0.0, 0.2, 0.0, 1.0};      // Ciemny zielony
            matrix.palette[9] = {0.0, 0.7, 0.0, 1.0};      // Średni zielony
            matrix.palette[10] = {0.0, 1.0, 0.0, 1.0};     // Jasny zielony
            matrix.palette[11] = {0.4, 1.0, 0.4, 1.0};     // Bardzo jasny zielony
            matrix.palette[12] = {0.0, 0.4, 0.0, 1.0};     // Ciemny zielony
            matrix.palette[13] = {0.0, 0.8, 0.0, 1.0};     // Średni zielony
            matrix.palette[14] = {0.2, 1.0, 0.2, 1.0};     // Jasny zielony
            matrix.palette[15] = {0.6, 1.0, 0.6, 1.0};     // Bardzo jasny zielony
            
            config.color_themes["Matrix"] = matrix;
            
            // Zapisz motywy do plików
            config.save_config();
        }
    }

    void apply_theme_to_terminal(VteTerminal *terminal) {
        // Pobierz aktualny motyw
        ColorTheme *current_theme = &config.color_themes[config.current_theme_name];
        
        // Sprawdź, czy motyw istnieje, jeśli nie, użyj domyślnego
        if (config.color_themes.find(config.current_theme_name) == config.color_themes.end()) {
            std::cerr << "Motyw " << config.current_theme_name << " nie istnieje, używam domyślnego." << std::endl;
            config.current_theme_name = "Default";
            current_theme = &config.color_themes["Default"];
        }
        
        // Ustawiamy przezroczystość tła
        GdkScreen *screen = gtk_widget_get_screen(GTK_WIDGET(terminal));
        GdkVisual *visual = gdk_screen_get_rgba_visual(screen);
        if (visual != NULL) {
            gtk_widget_set_visual(GTK_WIDGET(terminal), visual);
        }
        
        // Sprawdź, czy paleta jest pusta i zainicjalizuj ją, jeśli tak
        bool needs_initialization = false;
        if (current_theme->palette[0].red == 0.0 && current_theme->palette[0].green == 0.0 && 
            current_theme->palette[0].blue == 0.0 && current_theme->palette[0].alpha == 0.0) {
            needs_initialization = true;
        }
        
        if (needs_initialization) {
            std::cerr << "Inicjalizacja palety kolorów dla motywu: " << current_theme->name << std::endl;
            
            // Standardowa paleta kolorów terminala
            current_theme->palette[0] = {0.0, 0.0, 0.0, 1.0};          // Czarny
            current_theme->palette[1] = {0.8, 0.0, 0.0, 1.0};          // Czerwony
            current_theme->palette[2] = {0.0, 0.8, 0.0, 1.0};          // Zielony
            current_theme->palette[3] = {0.8, 0.8, 0.0, 1.0};          // Żółty
            current_theme->palette[4] = {0.0, 0.0, 0.8, 1.0};          // Niebieski
            current_theme->palette[5] = {0.8, 0.0, 0.8, 1.0};          // Magenta
            current_theme->palette[6] = {0.0, 0.8, 0.8, 1.0};          // Cyan
            current_theme->palette[7] = {0.8, 0.8, 0.8, 1.0};          // Biały
            current_theme->palette[8] = {0.5, 0.5, 0.5, 1.0};          // Jasny czarny (szary)
            current_theme->palette[9] = {1.0, 0.0, 0.0, 1.0};          // Jasny czerwony
            current_theme->palette[10] = {0.0, 1.0, 0.0, 1.0};         // Jasny zielony
            current_theme->palette[11] = {1.0, 1.0, 0.0, 1.0};         // Jasny żółty
            current_theme->palette[12] = {0.0, 0.0, 1.0, 1.0};         // Jasny niebieski
            current_theme->palette[13] = {1.0, 0.0, 1.0, 1.0};         // Jasny magenta
            current_theme->palette[14] = {0.0, 1.0, 1.0, 1.0};         // Jasny cyan
            current_theme->palette[15] = {1.0, 1.0, 1.0, 1.0};         // Jasny biały
        }
        
        // Używamy globalnej przezroczystości, a nie przezroczystości z motywu
        // Modyfikujemy kolor tła, aby uwzględnić przezroczystość
        GdkRGBA bg_with_alpha = current_theme->background;
        bg_with_alpha.alpha = 1.0 - config.transparency;
        
        // Tworzymy kopię palety kolorów, aby ustawić wartość alpha na 0 dla wszystkich kolorów
        GdkRGBA palette_copy[16];
        for (int i = 0; i < 16; i++) {
            palette_copy[i] = current_theme->palette[i];
            palette_copy[i].alpha = 0.0;  // Ustawiamy alpha na 0 dla wszystkich kolorów w palecie
        }
        
        // Zastosowanie kolorów z aktualnego motywu
        // Tekst pozostaje nieprzezroczysty, tylko tło jest przezroczyste
        // Używamy wszystkich 16 kolorów z palety
        vte_terminal_set_colors(terminal, &current_theme->foreground, &bg_with_alpha, palette_copy, 16);
        
        // Upewnij się, że kolory są faktycznie stosowane
        vte_terminal_set_color_foreground(terminal, &current_theme->foreground);
        vte_terminal_set_color_background(terminal, &bg_with_alpha);
        
        // Włączamy obsługę kompozycji dla terminala
        gtk_widget_set_app_paintable(GTK_WIDGET(terminal), TRUE);
        
        // Minimalne debugowanie
        std::cerr << "Zastosowano motyw: " << current_theme->name << std::endl;
    }

    void apply_theme_to_all_terminals() {
        for (auto tab : tabs) {
            apply_theme_to_terminal(VTE_TERMINAL(tab->terminal));
        }
    }

    void spawn_shell(VteTerminal *terminal, GPid *child_pid) {
        const gchar *shell = getenv("SHELL");
        if (shell == nullptr) {
            shell = "/bin/bash";
        }

        gchar *envp[] = {nullptr};
        
        // Callback do obsługi zakończenia procesu
        auto spawn_callback = [](VteTerminal *terminal, GPid pid, GError *error, gpointer user_data) {
            if (error) {
                g_warning("Error spawning terminal: %s", error->message);
                return;
            }
            
            GPid *child_pid_ptr = static_cast<GPid*>(user_data);
            if (child_pid_ptr) {
                *child_pid_ptr = pid;
            }
        };

        vte_terminal_spawn_async(
            terminal,
            VTE_PTY_DEFAULT,
            nullptr,
            (char *[]){(char*)shell, nullptr},
            envp,
            G_SPAWN_DEFAULT,
            nullptr,
            nullptr,
            nullptr,  // Nie używamy child_setup_data_destroy
            -1,
            nullptr,
            (VteTerminalSpawnAsyncCallback)spawn_callback,
            child_pid
        );
    }

    void show_search_dialog() {
        int current_page = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));
        if (current_page < 0 || current_page >= static_cast<int>(tabs.size())) {
            return;
        }
        
        GtkWidget *dialog = gtk_dialog_new_with_buttons(
            "Search", GTK_WINDOW(window),
            (GtkDialogFlags)(GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT),
            "_Cancel", GTK_RESPONSE_CANCEL,
            "_Search", GTK_RESPONSE_ACCEPT,
            NULL);
            
        GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
        
        GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
        gtk_container_set_border_width(GTK_CONTAINER(box), 10);
        
        GtkWidget *label = gtk_label_new("Search for:");
        GtkWidget *entry = gtk_entry_new();
        gtk_entry_set_activates_default(GTK_ENTRY(entry), TRUE);
        
        gtk_box_pack_start(GTK_BOX(box), label, FALSE, FALSE, 0);
        gtk_box_pack_start(GTK_BOX(box), entry, TRUE, TRUE, 0);
        
        gtk_container_add(GTK_CONTAINER(content_area), box);
        
        GtkWidget *regex_check = gtk_check_button_new_with_label("Regular expression");
        gtk_container_add(GTK_CONTAINER(content_area), regex_check);
        
        GtkWidget *case_check = gtk_check_button_new_with_label("Case sensitive");
        gtk_container_add(GTK_CONTAINER(content_area), case_check);
        
        gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_ACCEPT);
        gtk_widget_show_all(dialog);
        
        if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
            const char *text = gtk_entry_get_text(GTK_ENTRY(entry));
            gboolean regex = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(regex_check));
            gboolean case_sensitive = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(case_check));
            
            VteRegex *regex_obj = NULL;
            GError *error = NULL;
            
            // Używamy G_REGEX_CASELESS zamiast PCRE2_CASELESS
            if (regex) {
                regex_obj = vte_regex_new_for_search(text, -1, case_sensitive ? 0 : G_REGEX_CASELESS, &error);
            } else {
                // Escape regex special characters for literal search
                std::string escaped = "";
                for (const char *p = text; *p; p++) {
                    if (strchr("[]{}().*+?|^$\\", *p)) {
                        escaped += '\\';
                    }
                    escaped += *p;
                }
                regex_obj = vte_regex_new_for_search(escaped.c_str(), -1, case_sensitive ? 0 : G_REGEX_CASELESS, &error);
            }
            
            if (error) {
                GtkWidget *error_dialog = gtk_message_dialog_new(
                    GTK_WINDOW(dialog),
                    GTK_DIALOG_DESTROY_WITH_PARENT,
                    GTK_MESSAGE_ERROR,
                    GTK_BUTTONS_CLOSE,
                    "Error in search pattern: %s", error->message);
                gtk_dialog_run(GTK_DIALOG(error_dialog));
                gtk_widget_destroy(error_dialog);
                g_error_free(error);
            } else if (regex_obj) {
                VteTerminal *terminal = VTE_TERMINAL(tabs[current_page]->terminal);
                vte_terminal_search_set_regex(terminal, regex_obj, 0);
                vte_terminal_search_set_wrap_around(terminal, TRUE);
                
                if (!vte_terminal_search_find_next(terminal)) {
                    GtkWidget *info_dialog = gtk_message_dialog_new(
                        GTK_WINDOW(window),
                        GTK_DIALOG_DESTROY_WITH_PARENT,
                        GTK_MESSAGE_INFO,
                        GTK_BUTTONS_OK,
                        "Pattern not found");
                    gtk_dialog_run(GTK_DIALOG(info_dialog));
                    gtk_widget_destroy(info_dialog);
                }
                
                vte_regex_unref(regex_obj);
            }
        }
        
        gtk_widget_destroy(dialog);
    }

    void show_theme_dialog() {
        GtkWidget *dialog = gtk_dialog_new_with_buttons(
            "Select Theme", GTK_WINDOW(window),
            (GtkDialogFlags)(GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT),
            "_Cancel", GTK_RESPONSE_CANCEL,
            "_Apply", GTK_RESPONSE_ACCEPT,
            NULL);
            
        GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
        gtk_container_set_border_width(GTK_CONTAINER(content_area), 10);
        
        GtkWidget *list_box = gtk_list_box_new();
        gtk_list_box_set_selection_mode(GTK_LIST_BOX(list_box), GTK_SELECTION_SINGLE);
        
        // Dodanie motywów do listy
        int index = 0;
        int current_index = 0;
        for (auto &theme_pair : config.color_themes) {
            GtkWidget *row = gtk_list_box_row_new();
            GtkWidget *label = gtk_label_new(theme_pair.first.c_str());
            gtk_container_add(GTK_CONTAINER(row), label);
            gtk_list_box_insert(GTK_LIST_BOX(list_box), row, -1);
            
            if (theme_pair.second.name == current_theme->name) {
                current_index = index;
            }
            index++;
        }
        
        // Zaznaczenie aktualnego motywu
        gtk_list_box_select_row(GTK_LIST_BOX(list_box), 
                               gtk_list_box_get_row_at_index(GTK_LIST_BOX(list_box), current_index));
        
        GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
        gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), 
                                      GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
        gtk_container_add(GTK_CONTAINER(scrolled_window), list_box);
        gtk_widget_set_size_request(scrolled_window, 200, 200);
        
        gtk_container_add(GTK_CONTAINER(content_area), scrolled_window);
        gtk_widget_show_all(dialog);
        
        if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
            GtkListBoxRow *selected_row = gtk_list_box_get_selected_row(GTK_LIST_BOX(list_box));
            if (selected_row) {
                int selected_index = gtk_list_box_row_get_index(selected_row);
                
                // Znalezienie wybranego motywu
                index = 0;
                for (auto &theme_pair : config.color_themes) {
                    if (index == selected_index) {
                        current_theme = &config.color_themes[theme_pair.first];
                        config.current_theme_name = theme_pair.first;
                        apply_theme_to_all_terminals();
                        
                        // Zapisz konfigurację po zmianie
                        config.save_config();
                        break;
                    }
                    index++;
                }
            }
        }
        
        gtk_widget_destroy(dialog);
    }

    TerminalTab* get_current_tab() {
        int current_page = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));
        if (current_page >= 0 && current_page < static_cast<int>(tabs.size())) {
            return tabs[current_page];
        }
        return nullptr;
    }

    // Sprawdza, czy w terminalu jest uruchomiony proces inny niż domyślna powłoka
    bool has_foreground_process(TerminalTab *tab) {
        // Jeśli nie ma procesu potomnego, nie ma co sprawdzać
        if (tab->child_pid <= 0) return false;
        
        // Sprawdź, czy proces ma dzieci (co oznacza, że uruchomiono inny program)
        char proc_path[256];
        snprintf(proc_path, sizeof(proc_path), "/proc/%d/task/%d/children", tab->child_pid, tab->child_pid);
        
        FILE *f = fopen(proc_path, "r");
        if (!f) return false;
        
        char buffer[1024];
        size_t bytes_read = fread(buffer, 1, sizeof(buffer) - 1, f);
        fclose(f);
        
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            // Jeśli są jakieś procesy potomne, to znaczy, że coś jest uruchomione
            if (strlen(buffer) > 0) {
                return true;
            }
        }
        
        return false;
    }

    void close_tab(int tab_index) {
        if (tab_index >= 0 && tab_index < static_cast<int>(tabs.size())) {
            TerminalTab *tab = tabs[tab_index];
            
            // Sprawdź, czy w terminalu jest uruchomiony jakiś proces
            if (has_foreground_process(tab)) {
                GtkWidget *dialog = gtk_message_dialog_new(
                    GTK_WINDOW(window),
                    GTK_DIALOG_MODAL,
                    GTK_MESSAGE_QUESTION,
                    GTK_BUTTONS_YES_NO,
                    "There is a process running in this terminal. Close anyway?");
                
                int response = gtk_dialog_run(GTK_DIALOG(dialog));
                gtk_widget_destroy(dialog);
                
                if (response != GTK_RESPONSE_YES) {
                    return;  // Anuluj zamknięcie
                }
            }
            
            tabs.erase(tabs.begin() + tab_index);
            gtk_notebook_remove_page(GTK_NOTEBOOK(notebook), tab_index);
            delete tab;
            
            // Jeśli nie ma więcej zakładek, zamknij aplikację
            if (tabs.empty()) {
                // Dodaj nową zakładkę lub zamknij aplikację
                if (gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebook)) == 0) {
                    gtk_main_quit();
                }
            } else if (tabs.size() == 1) {
                // Ukryj pasek zakładek, gdy zostanie tylko jedna karta
                gtk_notebook_set_show_tabs(GTK_NOTEBOOK(notebook), FALSE);
            }
        }
    }

    void show_font_size_dialog() {
        GtkWidget *dialog = gtk_dialog_new_with_buttons(
            "Font Size", GTK_WINDOW(window),
            (GtkDialogFlags)(GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT),
            "_Cancel", GTK_RESPONSE_CANCEL,
            "_Apply", GTK_RESPONSE_ACCEPT,
            NULL);
            
        GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
        
        GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
        gtk_container_set_border_width(GTK_CONTAINER(box), 10);
        
        GtkWidget *label = gtk_label_new("Font Size:");
        GtkWidget *scale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 8.0, 24.0, 1.0);
        gtk_range_set_value(GTK_RANGE(scale), config.font_size); // Aktualny rozmiar czcionki
        gtk_widget_set_size_request(scale, 200, -1);
        
        gtk_box_pack_start(GTK_BOX(box), label, FALSE, FALSE, 0);
        gtk_box_pack_start(GTK_BOX(box), scale, TRUE, TRUE, 0);
        
        gtk_container_add(GTK_CONTAINER(content_area), box);
        gtk_widget_show_all(dialog);
        
        if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
            config.font_size = gtk_range_get_value(GTK_RANGE(scale));
            
            // Zastosuj nowy rozmiar czcionki do wszystkich terminali
            for (auto tab : tabs) {
                PangoFontDescription *font_desc = pango_font_description_from_string(config.font_family.c_str());
                pango_font_description_set_size(font_desc, (int)(config.font_size * PANGO_SCALE));
                vte_terminal_set_font(VTE_TERMINAL(tab->terminal), font_desc);
                pango_font_description_free(font_desc);
            }
            
            // Zapisz konfigurację po zmianie
            config.save_config();
        }
        
        gtk_widget_destroy(dialog);
    }
    
    void show_font_dialog() {
        GtkWidget *dialog = gtk_font_chooser_dialog_new("Select Font", GTK_WINDOW(window));
        
        // Ustaw aktualną czcionkę
        std::string font_string = config.font_family + " " + std::to_string((int)config.font_size);
        PangoFontDescription *current_font = pango_font_description_from_string(font_string.c_str());
        gtk_font_chooser_set_font_desc(GTK_FONT_CHOOSER(dialog), current_font);
        pango_font_description_free(current_font);
        
        if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
            PangoFontDescription *font_desc = gtk_font_chooser_get_font_desc(GTK_FONT_CHOOSER(dialog));
            
            // Aktualizacja zmiennych font_family i font_size
            config.font_family = pango_font_description_get_family(font_desc);
            config.font_size = pango_font_description_get_size(font_desc) / PANGO_SCALE;
            
            // Zastosuj nową czcionkę do wszystkich terminali
            for (auto tab : tabs) {
                vte_terminal_set_font(VTE_TERMINAL(tab->terminal), font_desc);
            }
            
            pango_font_description_free(font_desc);
            
            // Zapisz konfigurację po zmianie
            config.save_config();
        }
        
        gtk_widget_destroy(dialog);
    }
    
    void show_custom_colors_dialog() {
        GtkWidget *dialog = gtk_dialog_new_with_buttons(
            "Custom Colors", GTK_WINDOW(window),
            (GtkDialogFlags)(GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT),
            "_Cancel", GTK_RESPONSE_CANCEL,
            "_Apply", GTK_RESPONSE_ACCEPT,
            NULL);
            
        GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
        
        GtkWidget *grid = gtk_grid_new();
        gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
        gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
        gtk_container_set_border_width(GTK_CONTAINER(grid), 10);
        
        // Foreground color
        GtkWidget *fg_label = gtk_label_new("Foreground:");
        GtkWidget *fg_button = gtk_color_button_new_with_rgba(&current_theme->foreground);
        gtk_grid_attach(GTK_GRID(grid), fg_label, 0, 0, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), fg_button, 1, 0, 1, 1);
        
        // Background color
        GtkWidget *bg_label = gtk_label_new("Background:");
        GtkWidget *bg_button = gtk_color_button_new_with_rgba(&current_theme->background);
        gtk_grid_attach(GTK_GRID(grid), bg_label, 0, 1, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), bg_button, 1, 1, 1, 1);
        
        // Nazwa motywu
        GtkWidget *name_label = gtk_label_new("Theme Name:");
        GtkWidget *name_entry = gtk_entry_new();
        gtk_entry_set_text(GTK_ENTRY(name_entry), "Custom");
        gtk_grid_attach(GTK_GRID(grid), name_label, 0, 2, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), name_entry, 1, 2, 1, 1);
        
        gtk_container_add(GTK_CONTAINER(content_area), grid);
        gtk_widget_show_all(dialog);
        
        if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
            GdkRGBA fg_color, bg_color;
            gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(fg_button), &fg_color);
            gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(bg_button), &bg_color);
            
            // Pobierz nazwę motywu
            std::string theme_name = gtk_entry_get_text(GTK_ENTRY(name_entry));
            if (theme_name.empty()) {
                theme_name = "Custom";
            }
            
            // Utwórz nowy motyw lub zaktualizuj istniejący
            ColorTheme custom_theme;
            custom_theme.name = theme_name;
            custom_theme.foreground = fg_color;
            custom_theme.background = bg_color;
            custom_theme.transparency = config.transparency;
            
            // Inicjalizuj paletę kolorów dla nowego motywu
            custom_theme.palette[0] = {0.0, 0.0, 0.0, 1.0};          // Czarny
            custom_theme.palette[1] = {0.8, 0.0, 0.0, 1.0};          // Czerwony
            custom_theme.palette[2] = {0.0, 0.8, 0.0, 1.0};          // Zielony
            custom_theme.palette[3] = {0.8, 0.8, 0.0, 1.0};          // Żółty
            custom_theme.palette[4] = {0.0, 0.0, 0.8, 1.0};          // Niebieski
            custom_theme.palette[5] = {0.8, 0.0, 0.8, 1.0};          // Magenta
            custom_theme.palette[6] = {0.0, 0.8, 0.8, 1.0};          // Cyan
            custom_theme.palette[7] = {0.8, 0.8, 0.8, 1.0};          // Biały
            custom_theme.palette[8] = {0.5, 0.5, 0.5, 1.0};          // Jasny czarny (szary)
            custom_theme.palette[9] = {1.0, 0.0, 0.0, 1.0};          // Jasny czerwony
            custom_theme.palette[10] = {0.0, 1.0, 0.0, 1.0};         // Jasny zielony
            custom_theme.palette[11] = {1.0, 1.0, 0.0, 1.0};         // Jasny żółty
            custom_theme.palette[12] = {0.0, 0.0, 1.0, 1.0};         // Jasny niebieski
            custom_theme.palette[13] = {1.0, 0.0, 1.0, 1.0};         // Jasny magenta
            custom_theme.palette[14] = {0.0, 1.0, 1.0, 1.0};         // Jasny cyan
            custom_theme.palette[15] = {1.0, 1.0, 1.0, 1.0};         // Jasny biały
            
            // Dodaj motyw do konfiguracji
            config.color_themes[theme_name] = custom_theme;
            
            // Ustaw jako aktualny motyw
            current_theme = &config.color_themes[theme_name];
            config.current_theme_name = theme_name;
            
            // Zastosuj nowe kolory do wszystkich terminali
            apply_theme_to_all_terminals();
            
            // Zapisz konfigurację po zmianie
            config.save_config();
        }
        
        gtk_widget_destroy(dialog);
    }
    
    void show_about_dialog() {
        GtkWidget *dialog = gtk_about_dialog_new();
        gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(dialog), "Lum Terminal");
        gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), "1.0");
        gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog), "© 2023 Lum Team");
        gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog), "A modern terminal emulator");
        gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(dialog), "https://github.com/lum-team/terminal");
        
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    }
    
    void change_font_size(int delta) {
        // Zmiana rozmiaru czcionki o delta punktów
        config.font_size += delta;
        
        // Ograniczenie rozmiaru czcionki
        if (config.font_size < 8.0) config.font_size = 8.0;
        if (config.font_size > 24.0) config.font_size = 24.0;
        
        // Zastosowanie nowego rozmiaru czcionki do wszystkich terminali
        for (auto tab : tabs) {
            PangoFontDescription *font_desc = pango_font_description_from_string(config.font_family.c_str());
            pango_font_description_set_size(font_desc, (int)(config.font_size * PANGO_SCALE));
            vte_terminal_set_font(VTE_TERMINAL(tab->terminal), font_desc);
            pango_font_description_free(font_desc);
        }
        
        // Zapisz konfigurację po zmianie
        config.save_config();
    }
    
    void reset_font_size() {
        // Reset rozmiaru czcionki do domyślnego
        config.font_size = 11.0;
        
        // Zastosowanie domyślnego rozmiaru czcionki do wszystkich terminali
        for (auto tab : tabs) {
            PangoFontDescription *font_desc = pango_font_description_from_string(config.font_family.c_str());
            pango_font_description_set_size(font_desc, (int)(config.font_size * PANGO_SCALE));
            vte_terminal_set_font(VTE_TERMINAL(tab->terminal), font_desc);
            pango_font_description_free(font_desc);
        }
        
        // Zapisz konfigurację po zmianie
        config.save_config();
    }
    
    // Callbacks statyczne
    static void on_new_tab_clicked(GtkWidget *widget, gpointer data) {
        TerminalWindow *self = static_cast<TerminalWindow*>(data);
        g_print("Dodawanie nowej zakładki...\n");
        self->add_new_tab();
    }

    static void on_tab_close_clicked(GtkWidget *widget, gpointer data) {
        TerminalWindow *self = static_cast<TerminalWindow*>(data);
        
        // Znalezienie zakładki z tym przyciskiem
        for (size_t i = 0; i < self->tabs.size(); i++) {
            if (self->tabs[i]->close_button == widget) {
                self->close_tab(i);
                break;
            }
        }
    }

    static void on_terminal_exit(VteTerminal *terminal, int status, gpointer data) {
        TerminalWindow *self = static_cast<TerminalWindow*>(data);
        
        // Znalezienie zakładki z tym terminalem
        for (size_t i = 0; i < self->tabs.size(); i++) {
            if (VTE_TERMINAL(self->tabs[i]->terminal) == terminal) {
                self->close_tab(i);
                break;
            }
        }
    }

    static void on_title_changed(VteTerminal *terminal, gpointer data) {
        TerminalTab *tab = static_cast<TerminalTab*>(data);
        const char *title = vte_terminal_get_window_title(terminal);
        if (title) {
            tab->title = title;
            gtk_label_set_text(GTK_LABEL(tab->label), title);
        }
    }

    static void on_tab_switch(GtkNotebook *notebook, GtkWidget *page, guint page_num, gpointer data) {
        TerminalWindow *self = static_cast<TerminalWindow*>(data);
        if (page_num < self->tabs.size()) {
            gtk_widget_grab_focus(self->tabs[page_num]->terminal);
        }
    }

    static void on_search_clicked(GtkWidget *widget, gpointer data) {
        TerminalWindow *self = static_cast<TerminalWindow*>(data);
        self->show_search_dialog();
    }

    static void on_theme_clicked(GtkWidget *widget, gpointer data) {
        TerminalWindow *self = static_cast<TerminalWindow*>(data);
        self->show_theme_dialog();
    }

    static void on_transparency_menu_clicked(GtkWidget *widget, gpointer data) {
        TerminalWindow *self = static_cast<TerminalWindow*>(data);
        self->show_transparency_dialog();
    }
    
    static void on_font_size_clicked(GtkWidget *widget, gpointer data) {
        TerminalWindow *self = static_cast<TerminalWindow*>(data);
        self->show_font_size_dialog();
    }
    
    static void on_font_clicked(GtkWidget *widget, gpointer data) {
        TerminalWindow *self = static_cast<TerminalWindow*>(data);
        self->show_font_dialog();
    }
    
    static void on_custom_colors_clicked(GtkWidget *widget, gpointer data) {
        TerminalWindow *self = static_cast<TerminalWindow*>(data);
        self->show_custom_colors_dialog();
    }
    
    static void on_about_clicked(GtkWidget *widget, gpointer data) {
        TerminalWindow *self = static_cast<TerminalWindow*>(data);
        self->show_about_dialog();
    }
    
    static gboolean on_window_delete(GtkWidget *widget, GdkEvent *event, gpointer data) {
        TerminalWindow *self = static_cast<TerminalWindow*>(data);
        
        // Sprawdź, czy można bezpiecznie zamknąć okno
        if (self->can_close_window()) {
            // Pozwól na zamknięcie okna
            gtk_main_quit();
            return FALSE;
        } else {
            // Anuluj zamknięcie okna
            return TRUE;
        }
    }

    static gboolean on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer data) {
        TerminalWindow *self = static_cast<TerminalWindow*>(data);
        
        // Ctrl+Shift+T - nowa zakładka
        if ((event->state & (GDK_CONTROL_MASK | GDK_SHIFT_MASK)) == (GDK_CONTROL_MASK | GDK_SHIFT_MASK) && 
            event->keyval == GDK_KEY_T) {
            self->add_new_tab();
            return TRUE;
        }
        
        // Ctrl+Shift+W - zamknięcie zakładki
        if ((event->state & (GDK_CONTROL_MASK | GDK_SHIFT_MASK)) == (GDK_CONTROL_MASK | GDK_SHIFT_MASK) && 
            event->keyval == GDK_KEY_W) {
            int current_page = gtk_notebook_get_current_page(GTK_NOTEBOOK(self->notebook));
            if (current_page >= 0) {
                self->close_tab(current_page);
            }
            return TRUE;
        }
        
        // Ctrl+ - powiększenie czcionki
        if ((event->state & GDK_CONTROL_MASK) && 
            (event->keyval == GDK_KEY_plus || event->keyval == GDK_KEY_equal)) {
            self->change_font_size(1);
            return TRUE;
        }
        
        // Ctrl- - zmniejszenie czcionki
        if ((event->state & GDK_CONTROL_MASK) && 
            event->keyval == GDK_KEY_minus) {
            self->change_font_size(-1);
            return TRUE;
        }
        
        // Ctrl+0 - reset rozmiaru czcionki
        if ((event->state & GDK_CONTROL_MASK) && 
            event->keyval == GDK_KEY_0) {
            self->reset_font_size();
            return TRUE;
        }
        
        // Ctrl+Shift+F - wyszukiwanie
        if ((event->state & (GDK_CONTROL_MASK | GDK_SHIFT_MASK)) == (GDK_CONTROL_MASK | GDK_SHIFT_MASK) && 
            event->keyval == GDK_KEY_F) {
            self->show_search_dialog();
            return TRUE;
        }
        
        // Ctrl+Page_Down - następna zakładka
        if ((event->state & GDK_CONTROL_MASK) && event->keyval == GDK_KEY_Page_Down) {
            int n_pages = gtk_notebook_get_n_pages(GTK_NOTEBOOK(self->notebook));
            int current = gtk_notebook_get_current_page(GTK_NOTEBOOK(self->notebook));
            gtk_notebook_set_current_page(GTK_NOTEBOOK(self->notebook), (current + 1) % n_pages);
            return TRUE;
        }
        
        // Ctrl+Page_Up - poprzednia zakładka
        if ((event->state & GDK_CONTROL_MASK) && event->keyval == GDK_KEY_Page_Up) {
            int n_pages = gtk_notebook_get_n_pages(GTK_NOTEBOOK(self->notebook));
            int current = gtk_notebook_get_current_page(GTK_NOTEBOOK(self->notebook));
            gtk_notebook_set_current_page(GTK_NOTEBOOK(self->notebook), (current + n_pages - 1) % n_pages);
            return TRUE;
        }
        
        // Ctrl+ + - powiększenie tekstu
        if ((event->state & GDK_CONTROL_MASK) && (event->keyval == GDK_KEY_plus || event->keyval == GDK_KEY_equal)) {
            int current_page = gtk_notebook_get_current_page(GTK_NOTEBOOK(self->notebook));
            if (current_page >= 0 && current_page < static_cast<int>(self->tabs.size())) {
                VteTerminal *terminal = VTE_TERMINAL(self->tabs[current_page]->terminal);
                double font_scale = vte_terminal_get_font_scale(terminal);
                vte_terminal_set_font_scale(terminal, font_scale * 1.1);
            }
            return TRUE;
        }
        
        // Ctrl+ - - pomniejszenie tekstu
        if ((event->state & GDK_CONTROL_MASK) && event->keyval == GDK_KEY_minus) {
            int current_page = gtk_notebook_get_current_page(GTK_NOTEBOOK(self->notebook));
            if (current_page >= 0 && current_page < static_cast<int>(self->tabs.size())) {
                VteTerminal *terminal = VTE_TERMINAL(self->tabs[current_page]->terminal);
                double font_scale = vte_terminal_get_font_scale(terminal);
                vte_terminal_set_font_scale(terminal, font_scale / 1.1);
            }
            return TRUE;
        }
        
        // Ctrl+0 - reset rozmiaru tekstu
        if ((event->state & GDK_CONTROL_MASK) && event->keyval == GDK_KEY_0) {
            int current_page = gtk_notebook_get_current_page(GTK_NOTEBOOK(self->notebook));
            if (current_page >= 0 && current_page < static_cast<int>(self->tabs.size())) {
                VteTerminal *terminal = VTE_TERMINAL(self->tabs[current_page]->terminal);
                vte_terminal_set_font_scale(terminal, 1.0);
            }
            return TRUE;
        }
        
        return FALSE;
    }

    // Funkcje pomocnicze dla menu kontekstowego
    static void close_current_tab(GtkWidget *widget, gpointer data) {
        TerminalWindow *self = static_cast<TerminalWindow*>(data);
        int current_page = gtk_notebook_get_current_page(GTK_NOTEBOOK(self->notebook));
        if (current_page >= 0) {
            self->close_tab(current_page);
        }
    }
    
    static void show_search(GtkWidget *widget, gpointer data) {
        TerminalWindow *self = static_cast<TerminalWindow*>(data);
        self->show_search_dialog();
    }
    
    static void show_theme_selector(GtkWidget *widget, gpointer data) {
        TerminalWindow *self = static_cast<TerminalWindow*>(data);
        self->show_theme_dialog();
    }
    
    static void show_transparency_settings(GtkWidget *widget, gpointer data) {
        TerminalWindow *self = static_cast<TerminalWindow*>(data);
        self->show_transparency_dialog();
    }
    
    static gboolean on_right_click(GtkWidget *widget, GdkEventButton *event, gpointer data) {
        if (event->button == 3) {
            TerminalWindow *self = static_cast<TerminalWindow*>(data);
            
            GtkWidget *menu = gtk_menu_new();
            
            // Kopiowanie
            GtkWidget *item_copy = gtk_menu_item_new_with_label("Copy");
            g_signal_connect(item_copy, "activate", G_CALLBACK(vte_terminal_copy_clipboard), widget);
            gtk_menu_shell_append(GTK_MENU_SHELL(menu), item_copy);
            
            // Wklejanie
            GtkWidget *item_paste = gtk_menu_item_new_with_label("Paste");
            g_signal_connect(item_paste, "activate", G_CALLBACK(vte_terminal_paste_clipboard), widget);
            gtk_menu_shell_append(GTK_MENU_SHELL(menu), item_paste);
            
            // Separator
            GtkWidget *separator = gtk_separator_menu_item_new();
            gtk_menu_shell_append(GTK_MENU_SHELL(menu), separator);
            
            // Nowa zakładka
            GtkWidget *item_new_tab = gtk_menu_item_new_with_label("New Tab");
            g_signal_connect(item_new_tab, "activate", G_CALLBACK(on_new_tab_clicked), self);
            gtk_menu_shell_append(GTK_MENU_SHELL(menu), item_new_tab);
            
            // Zamknięcie zakładki
            GtkWidget *item_close_tab = gtk_menu_item_new_with_label("Close Tab");
            g_signal_connect(item_close_tab, "activate", G_CALLBACK(close_current_tab), self);
            gtk_menu_shell_append(GTK_MENU_SHELL(menu), item_close_tab);
            
            // Separator
            separator = gtk_separator_menu_item_new();
            gtk_menu_shell_append(GTK_MENU_SHELL(menu), separator);
            
            // Wyszukiwanie
            GtkWidget *item_search = gtk_menu_item_new_with_label("Search");
            g_signal_connect(item_search, "activate", G_CALLBACK(show_search), self);
            gtk_menu_shell_append(GTK_MENU_SHELL(menu), item_search);
            
            // Wybór motywu
            GtkWidget *item_theme = gtk_menu_item_new_with_label("Select Theme");
            g_signal_connect(item_theme, "activate", G_CALLBACK(show_theme_selector), self);
            gtk_menu_shell_append(GTK_MENU_SHELL(menu), item_theme);
            
            // Ustawienia przezroczystości
            GtkWidget *item_transparency = gtk_menu_item_new_with_label("Adjust Transparency");
            g_signal_connect(item_transparency, "activate", G_CALLBACK(show_transparency_settings), self);
            gtk_menu_shell_append(GTK_MENU_SHELL(menu), item_transparency);
            
            gtk_widget_show_all(menu);
            gtk_menu_popup_at_pointer(GTK_MENU(menu), (GdkEvent*)event);
            return TRUE;
        }
        return FALSE;
    }
};

int main(int argc, char *argv[]) {
    // Dodanie obsługi argumentów wiersza poleceń
    gboolean version = FALSE;
    gboolean help = FALSE;
    
    GOptionEntry entries[] = {
        { "version", 'v', 0, G_OPTION_ARG_NONE, &version, "Show version information", NULL },
        { "help", 'h', 0, G_OPTION_ARG_NONE, &help, "Show help", NULL },
        { NULL }
    };
    
    GError *error = NULL;
    GOptionContext *context = g_option_context_new("- Lum Terminal");
    g_option_context_add_main_entries(context, entries, NULL);
    g_option_context_add_group(context, gtk_get_option_group(TRUE));
    
    if (!g_option_context_parse(context, &argc, &argv, &error)) {
        g_print("Option parsing failed: %s\n", error->message);
        g_error_free(error);
        return 1;
    }
    
    if (version) {
        g_print("Lum Terminal version 1.1.0\n");
        return 0;
    }
    
    if (help) {
        g_print("%s\n", g_option_context_get_help(context, TRUE, NULL));
        return 0;
    }
    
    g_option_context_free(context);
    
    // Uruchomienie aplikacji
    TerminalWindow terminal_window;
    return 0;
}
