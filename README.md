Lum Terminal
Lum Terminal is a lightweight, custom terminal emulator built using GTK and VTE, designed to be a part of the Lum graphical environment. It offers a simple user interface, basic terminal functionality, and support for right-click context menus with copy and paste options.

Features
Customizable font and color scheme.

Right-click context menu with copy and paste functionality.

Shell spawning support (default to /bin/bash or the user's shell).

Simple and clean interface, ideal for integration into custom desktop environments.

Dependencies
To build and run Lum Terminal, you will need to have the following libraries and tools installed:

GTK+ 3 - For building the user interface.

VTE - Terminal widget for handling terminal emulation.

Pango - For handling text rendering and fonts.

On Ubuntu/Debian-based systems, you can install these dependencies by running:
bash
sudo apt update
sudo apt install build-essential libgtk-3-dev libvte-2.91-dev libpango1.0-dev
On Arch Linux-based systems, use the following:
bash

sudo pacman -S base-devel gtk3 vte3 pango
For other distributions, use the appropriate package manager to install gtk3, vte3, and pango.
Building
To compile and run Lum Terminal, follow these steps:

Clone the repository:

bash
git clone https://github.com/yourusername/lum-terminal.git
cd lum-terminal
./install.sh

