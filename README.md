# Lum Terminal
* Lum Terminal is a lightweight, custom terminal emulator built using GTK and VTE, designed to be a part of the Lum graphical environment. It offers a simple user interface, basic terminal functionality, and support for right-click context menus with copy and paste options.

# Features

* Right-click context menu with copy and paste functionality.

* Shell spawning support (default to /bin/bash or the user's shell).

* Simple and clean interface, ideal for integration into custom desktop environments.

# Dependencies
* GTK+3
* VTE
* Pango

# Ubuntu/Debian-based 

sudo apt update
sudo apt install build-essential libgtk-3-dev libvte-2.91-dev libpango1.0-dev

# Arch/Arch based

* sudo pacman -S base-devel gtk3 vte3 pango

# Other distro

For other distributions, use the appropriate package manager to install gtk3, vte3, and pango.

# Building

git clone https://github.com/BakSlerz/Lumterminal

cd lum-terminal

make

./install.sh

