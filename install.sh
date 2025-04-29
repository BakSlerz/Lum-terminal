#!/bin/bash

# Nazwa aplikacji
APP_NAME="lum-terminal"
APP_SRC="terminal_app.cpp"
BUILD_DIR="$HOME/build_lum_terminal"
INSTALL_DIR="$BUILD_DIR/usr/local"
ICON_NAME="lum-terminal"
ICON_FILE="lum-terminal.png"  # Możesz też użyć .svg
ICON_SRC_PATH="./$ICON_FILE"
ICON_DEST_DIR="$BUILD_DIR/usr/share/icons/hicolor/256x256/apps"
DESKTOP_FILE="$BUILD_DIR/usr/share/applications/$APP_NAME.desktop"

# Funkcja do instalacji zależności na różnych dystrybucjach
install_dependencies() {
    if [ -f /etc/os-release ]; then
        . /etc/os-release
        case "$ID" in
            debian|ubuntu)
                echo "Instalowanie zależności dla Debian/Ubuntu..."
                sudo apt-get update
                sudo apt-get install -y build-essential libgtk-3-dev libvte-2.91-dev
                ;;
            arch)
                echo "Instalowanie zależności dla Arch Linux..."
                sudo pacman -Sy --noconfirm base-devel gtk3 vte3
                ;;
            fedora)
                echo "Instalowanie zależności dla Fedora..."
                sudo dnf install -y gcc-c++ gtk3 vte291-devel
                ;;
            opensuse*)
                echo "Instalowanie zależności dla openSUSE..."
                sudo zypper install -y gcc-c++ gtk3-devel vte-devel
                ;;
            alpine)
                echo "Instalowanie zależności dla Alpine..."
                sudo apk add --no-cache build-base gtk+3.0 vte3-dev
                ;;
            *)
                echo "Nie rozpoznano tej dystrybucji. Zainstaluj zależności ręcznie."
                exit 1
                ;;
        esac
    else
        echo "Nie można rozpoznać systemu operacyjnego. Zainstaluj zależności ręcznie."
        exit 1
    fi
}

# Tworzenie struktury katalogów
echo "Tworzenie struktury katalogów..."
mkdir -p "$INSTALL_DIR/bin"
mkdir -p "$ICON_DEST_DIR"
mkdir -p "$BUILD_DIR/usr/share/applications"

# Kompilacja aplikacji
echo "Kompilowanie aplikacji..."
g++ -o "$INSTALL_DIR/bin/$APP_NAME" "$APP_SRC" $(pkg-config --cflags --libs gtk+-3.0 vte-2.91)

# Kopiowanie ikony, jeśli istnieje
if [ -f "$ICON_SRC_PATH" ]; then
    echo "Kopiowanie ikony aplikacji..."
    cp "$ICON_SRC_PATH" "$ICON_DEST_DIR/$ICON_NAME.png"
else
    echo "Brak pliku ikony ($ICON_SRC_PATH). Dodaj plik PNG i uruchom ponownie."
    exit 1
fi

# Tworzenie pliku .desktop
echo "Tworzenie pliku .desktop..."
cat <<EOF > "$DESKTOP_FILE"
[Desktop Entry]
Name=Lum Terminal
Comment=Minimalistyczny emulator terminala
Exec=/usr/local/bin/$APP_NAME
Icon=$ICON_NAME
Terminal=false
Type=Application
Categories=Utility;TerminalEmulator;
EOF

# Instalacja aplikacji
echo "Instalowanie aplikacji do systemu..."
sudo cp -r "$BUILD_DIR/usr" /

# Odświeżenie cache ikon
echo "Odświeżanie cache ikon..."
if command -v gtk-update-icon-cache &>/dev/null; then
    sudo gtk-update-icon-cache /usr/share/icons/hicolor
fi

echo "✅ Instalacja zakończona. Uruchom aplikację jako „Lum Terminal” z menu systemowego."

