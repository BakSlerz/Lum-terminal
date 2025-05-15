# Makefile for Lum Terminal

# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17
LDFLAGS = $(shell pkg-config --libs gtk+-3.0 vte-2.91)
CPPFLAGS = $(shell pkg-config --cflags gtk+-3.0 vte-2.91)

# File names
TARGET = lum-terminal
SOURCES = terminal_app.cpp
OBJECTS = $(SOURCES:.cpp=.o)

# Installation paths
PREFIX = /usr/local
BINDIR = $(PREFIX)/bin
DATADIR = $(PREFIX)/share
APPDIR = $(DATADIR)/applications
ICONDIR = $(DATADIR)/icons/hicolor/scalable/apps
CONFIG_DIR = $(HOME)/.config/lum-terminal
THEMES_DIR = $(CONFIG_DIR)/themes

# Default target
all: $(TARGET)

# Program compilation
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Direct compilation without object files (for simplicity)
build:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $(TARGET) $(SOURCES) $(LDFLAGS)

# Rule for object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

# Creating configuration directories
config-dirs:
	@echo "Creating configuration directories..."
	@mkdir -p $(CONFIG_DIR)
	@mkdir -p $(THEMES_DIR)
	@echo "Configuration directories created."

# Creating default theme
default-theme: config-dirs
	@echo "Creating default theme..."
	@echo "[Theme]" > $(THEMES_DIR)/Default.theme
	@echo "name=Default" >> $(THEMES_DIR)/Default.theme
	@echo "foreground=0.9,0.9,0.9,1.0" >> $(THEMES_DIR)/Default.theme
	@echo "background=0.1,0.1,0.1,1.0" >> $(THEMES_DIR)/Default.theme
	@echo "transparency=0.0" >> $(THEMES_DIR)/Default.theme
	@echo "" >> $(THEMES_DIR)/Default.theme
	@echo "[Palette]" >> $(THEMES_DIR)/Default.theme
	@echo "color0=0.0,0.0,0.0,1.0" >> $(THEMES_DIR)/Default.theme
	@echo "color1=0.8,0.0,0.0,1.0" >> $(THEMES_DIR)/Default.theme
	@echo "color2=0.0,0.8,0.0,1.0" >> $(THEMES_DIR)/Default.theme
	@echo "color3=0.8,0.8,0.0,1.0" >> $(THEMES_DIR)/Default.theme
	@echo "color4=0.0,0.0,0.8,1.0" >> $(THEMES_DIR)/Default.theme
	@echo "color5=0.8,0.0,0.8,1.0" >> $(THEMES_DIR)/Default.theme
	@echo "color6=0.0,0.8,0.8,1.0" >> $(THEMES_DIR)/Default.theme
	@echo "color7=0.8,0.8,0.8,1.0" >> $(THEMES_DIR)/Default.theme
	@echo "color8=0.5,0.5,0.5,1.0" >> $(THEMES_DIR)/Default.theme
	@echo "color9=1.0,0.0,0.0,1.0" >> $(THEMES_DIR)/Default.theme
	@echo "color10=0.0,1.0,0.0,1.0" >> $(THEMES_DIR)/Default.theme
	@echo "color11=1.0,1.0,0.0,1.0" >> $(THEMES_DIR)/Default.theme
	@echo "color12=0.0,0.0,1.0,1.0" >> $(THEMES_DIR)/Default.theme
	@echo "color13=1.0,0.0,1.0,1.0" >> $(THEMES_DIR)/Default.theme
	@echo "color14=0.0,1.0,1.0,1.0" >> $(THEMES_DIR)/Default.theme
	@echo "color15=1.0,1.0,1.0,1.0" >> $(THEMES_DIR)/Default.theme
	@echo "Default theme created."

# Creating light theme
light-theme: config-dirs
	@echo "Creating light theme..."
	@echo "[Theme]" > $(THEMES_DIR)/Light.theme
	@echo "name=Light" >> $(THEMES_DIR)/Light.theme
	@echo "foreground=0.1,0.1,0.1,1.0" >> $(THEMES_DIR)/Light.theme
	@echo "background=0.95,0.95,0.95,1.0" >> $(THEMES_DIR)/Light.theme
	@echo "transparency=0.0" >> $(THEMES_DIR)/Light.theme
	@echo "" >> $(THEMES_DIR)/Light.theme
	@echo "[Palette]" >> $(THEMES_DIR)/Light.theme
	@echo "color0=0.0,0.0,0.0,1.0" >> $(THEMES_DIR)/Light.theme
	@echo "color1=0.8,0.0,0.0,1.0" >> $(THEMES_DIR)/Light.theme
	@echo "color2=0.0,0.6,0.0,1.0" >> $(THEMES_DIR)/Light.theme
	@echo "color3=0.6,0.6,0.0,1.0" >> $(THEMES_DIR)/Light.theme
	@echo "color4=0.0,0.0,0.8,1.0" >> $(THEMES_DIR)/Light.theme
	@echo "color5=0.8,0.0,0.8,1.0" >> $(THEMES_DIR)/Light.theme
	@echo "color6=0.0,0.6,0.6,1.0" >> $(THEMES_DIR)/Light.theme
	@echo "color7=0.7,0.7,0.7,1.0" >> $(THEMES_DIR)/Light.theme
	@echo "color8=0.3,0.3,0.3,1.0" >> $(THEMES_DIR)/Light.theme
	@echo "color9=1.0,0.0,0.0,1.0" >> $(THEMES_DIR)/Light.theme
	@echo "color10=0.0,0.8,0.0,1.0" >> $(THEMES_DIR)/Light.theme
	@echo "color11=0.8,0.8,0.0,1.0" >> $(THEMES_DIR)/Light.theme
	@echo "color12=0.0,0.0,1.0,1.0" >> $(THEMES_DIR)/Light.theme
	@echo "color13=1.0,0.0,1.0,1.0" >> $(THEMES_DIR)/Light.theme
	@echo "color14=0.0,0.8,0.8,1.0" >> $(THEMES_DIR)/Light.theme
	@echo "color15=0.0,0.0,0.0,1.0" >> $(THEMES_DIR)/Light.theme
	@echo "Light theme created."

# Creating Matrix theme
matrix-theme: config-dirs
	@echo "Creating Matrix theme..."
	@echo "[Theme]" > $(THEMES_DIR)/Matrix.theme
	@echo "name=Matrix" >> $(THEMES_DIR)/Matrix.theme
	@echo "foreground=0.0,1.0,0.0,1.0" >> $(THEMES_DIR)/Matrix.theme
	@echo "background=0.0,0.0,0.0,1.0" >> $(THEMES_DIR)/Matrix.theme
	@echo "transparency=0.1" >> $(THEMES_DIR)/Matrix.theme
	@echo "" >> $(THEMES_DIR)/Matrix.theme
	@echo "[Palette]" >> $(THEMES_DIR)/Matrix.theme
	@echo "color0=0.0,0.0,0.0,1.0" >> $(THEMES_DIR)/Matrix.theme
	@echo "color1=0.0,0.5,0.0,1.0" >> $(THEMES_DIR)/Matrix.theme
	@echo "color2=0.0,0.8,0.0,1.0" >> $(THEMES_DIR)/Matrix.theme
	@echo "color3=0.0,0.6,0.0,1.0" >> $(THEMES_DIR)/Matrix.theme
	@echo "color4=0.0,0.4,0.0,1.0" >> $(THEMES_DIR)/Matrix.theme
	@echo "color5=0.0,0.5,0.0,1.0" >> $(THEMES_DIR)/Matrix.theme
	@echo "color6=0.0,0.7,0.0,1.0" >> $(THEMES_DIR)/Matrix.theme
	@echo "color7=0.0,0.8,0.0,1.0" >> $(THEMES_DIR)/Matrix.theme
	@echo "color8=0.0,0.3,0.0,1.0" >> $(THEMES_DIR)/Matrix.theme
	@echo "color9=0.0,0.6,0.0,1.0" >> $(THEMES_DIR)/Matrix.theme
	@echo "color10=0.0,1.0,0.0,1.0" >> $(THEMES_DIR)/Matrix.theme
	@echo "color11=0.0,0.8,0.0,1.0" >> $(THEMES_DIR)/Matrix.theme
	@echo "color12=0.0,0.5,0.0,1.0" >> $(THEMES_DIR)/Matrix.theme
	@echo "color13=0.0,0.7,0.0,1.0" >> $(THEMES_DIR)/Matrix.theme
	@echo "color14=0.0,0.9,0.0,1.0" >> $(THEMES_DIR)/Matrix.theme
	@echo "color15=0.0,1.0,0.0,1.0" >> $(THEMES_DIR)/Matrix.theme
	@echo "Matrix theme created."

# Creating all themes
themes: default-theme light-theme matrix-theme

# Creating .desktop file
desktop:
	@echo "Creating .desktop file..."
	@echo "[Desktop Entry]" > lum-terminal.desktop
	@echo "Name=Lum Terminal" >> lum-terminal.desktop
	@echo "Comment=Modern terminal with theme support" >> lum-terminal.desktop
	@echo "Exec=$(BINDIR)/lum-terminal" >> lum-terminal.desktop
	@echo "Icon=lum-terminal" >> lum-terminal.desktop
	@echo "Terminal=false" >> lum-terminal.desktop
	@echo "Type=Application" >> lum-terminal.desktop
	@echo "Categories=GTK;System;TerminalEmulator;" >> lum-terminal.desktop
	@echo "StartupNotify=true" >> lum-terminal.desktop
	@echo ".desktop file created."

# Creating SVG icon
icon:
	@echo "Creating SVG icon..."
	@echo '<?xml version="1.0" encoding="UTF-8" standalone="no"?>' > lum-terminal.svg
	@echo '<svg xmlns="http://www.w3.org/2000/svg" width="48" height="48" viewBox="0 0 48 48">' >> lum-terminal.svg
	@echo '  <rect width="44" height="36" x="2" y="6" rx="3" ry="3" fill="#333" />' >> lum-terminal.svg
	@echo '  <rect width="40" height="32" x="4" y="8" rx="1.5" ry="1.5" fill="#222" />' >> lum-terminal.svg
	@echo '  <path d="M8 12 L16 20 L8 28" stroke="#0f0" stroke-width="2" fill="none" />' >> lum-terminal.svg
	@echo '  <path d="M18 28 L30 28" stroke="#0f0" stroke-width="2" />' >> lum-terminal.svg
	@echo '</svg>' >> lum-terminal.svg
	@echo "SVG icon created."

# Installing the program
install: $(TARGET) desktop icon
	@echo "Installing application..."
	@mkdir -p $(DESTDIR)$(BINDIR)
	@mkdir -p $(DESTDIR)$(APPDIR)
	@mkdir -p $(DESTDIR)$(ICONDIR)
	@install -m 755 $(TARGET) $(DESTDIR)$(BINDIR)/
	@install -m 644 lum-terminal.desktop $(DESTDIR)$(APPDIR)/
	@install -m 644 lum-terminal.svg $(DESTDIR)$(ICONDIR)/
	@echo "Refreshing icon cache..."
	@-gtk-update-icon-cache -f -t $(DESTDIR)$(DATADIR)/icons/hicolor 2>/dev/null || true
	@echo "Installation complete. Run 'lum-terminal'"

# Uninstalling the program
uninstall:
	@echo "Uninstalling application..."
	@rm -f $(DESTDIR)$(BINDIR)/$(TARGET)
	@rm -f $(DESTDIR)$(APPDIR)/lum-terminal.desktop
	@rm -f $(DESTDIR)$(ICONDIR)/lum-terminal.svg
	@echo "Refreshing icon cache..."
	@-gtk-update-icon-cache -f -t $(DESTDIR)$(DATADIR)/icons/hicolor 2>/dev/null || true
	@echo "Uninstallation complete."

# Cleaning temporary files
clean:
	@echo "Cleaning temporary files..."
	@rm -f $(TARGET) $(OBJECTS) lum-terminal.desktop lum-terminal.svg
	@echo "Cleaning complete."

# Cleaning everything (temporary and installed files)
distclean: clean uninstall

# Local installation (for users without root privileges)
install-local: $(TARGET) desktop icon themes
	@echo "Installing application locally..."
	@mkdir -p $(HOME)/.local/bin
	@mkdir -p $(HOME)/.local/share/applications
	@mkdir -p $(HOME)/.local/share/icons/hicolor/scalable/apps
	@install -m 755 $(TARGET) $(HOME)/.local/bin/
	@install -m 644 lum-terminal.desktop $(HOME)/.local/share/applications/
	@install -m 644 lum-terminal.svg $(HOME)/.local/share/icons/hicolor/scalable/apps/
	@echo "Refreshing icon cache..."
	@-gtk-update-icon-cache -f -t $(HOME)/.local/share/icons/hicolor 2>/dev/null || true
	@echo "Local installation complete. Run 'lum-terminal'"

# Mark targets that are not files
.PHONY: all build install uninstall clean distclean desktop icon config-dirs default-theme light-theme matrix-theme themes install-local