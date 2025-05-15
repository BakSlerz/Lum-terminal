#!/bin/bash

# Installation script for Lum Terminal

# Check if user has root privileges
if [ "$EUID" -eq 0 ]; then
    INSTALL_TYPE="system"
else
    INSTALL_TYPE="local"
    echo "No root privileges. Installation will be performed locally for the current user."
fi

# Function to install dependencies
install_dependencies() {
    if [ -f /etc/os-release ]; then
        . /etc/os-release
        case "$ID" in
            debian|ubuntu)
                echo "Installing dependencies for Debian/Ubuntu..."
                sudo apt-get update
                sudo apt-get install -y build-essential libgtk-3-dev libvte-2.91-dev
                ;;
            arch)
                echo "Installing dependencies for Arch Linux..."
                sudo pacman -Sy --noconfirm base-devel gtk3 vte3
                ;;
            fedora)
                echo "Installing dependencies for Fedora..."
                sudo dnf install -y gcc-c++ gtk3 vte291-devel
                ;;
            opensuse*)
                echo "Installing dependencies for openSUSE..."
                sudo zypper install -y gcc-c++ gtk3-devel vte-devel
                ;;
            alpine)
                echo "Installing dependencies for Alpine..."
                sudo apk add --no-cache build-base gtk+3.0 vte3-dev
                ;;
            *)
                echo "Distribution not recognized. Install dependencies manually."
                exit 1
                ;;
        esac
    else
        echo "Operating system not recognized. Install dependencies manually."
        exit 1
    fi
}

# Ask about installing dependencies
read -p "Do you want to install dependencies? (y/n): " install_deps
if [ "$install_deps" = "y" ] || [ "$install_deps" = "Y" ]; then
    install_dependencies
fi

# Creating directory structure
echo "Creating directory structure..."
mkdir -p ~/.config/lum-terminal/themes

# Check if icon exists
if [ ! -f "lum-terminal.svg" ]; then
    echo "Missing icon file lum-terminal.svg. Creating a simple icon..."
    cat > lum-terminal.svg << EOF
<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<svg xmlns="http://www.w3.org/2000/svg" width="48" height="48" viewBox="0 0 48 48">
  <rect width="44" height="36" x="2" y="6" rx="3" ry="3" fill="#333" />
  <rect width="40" height="32" x="4" y="8" rx="1.5" ry="1.5" fill="#222" />
  <path d="M8 12 L16 20 L8 28" stroke="#0f0" stroke-width="2" fill="none" />
  <path d="M18 28 L30 28" stroke="#0f0" stroke-width="2" />
</svg>
EOF
fi

# Compiling the application
echo "Compiling the application..."
make clean
make

# Creating .desktop file
echo "Creating .desktop file..."
make desktop

# Installation
echo "Installing the application..."
if [ "$INSTALL_TYPE" = "system" ]; then
    sudo make install
else
    make install-local
fi

echo "Installation complete. You can run the application with the command 'lum-terminal'"

