import gi
gi.require_version("Gtk", "3.0")
gi.require_version("Vte", "2.91")
from gi.repository import Gtk, Vte, GLib, Pango, Gdk
import os

class TerminalWindow(Gtk.Window):
    def __init__(self):
        Gtk.Window.__init__(self, title="Terminal")
        self.set_default_size(800, 500)

        # Terminal
        self.terminal = Vte.Terminal()
        self.spawn_shell()

        # Czcionka
        font = Pango.FontDescription("Monospace 11")
        self.terminal.set_font(font)

        # Kolory
        self.terminal.set_colors(
            foreground=Gdk.RGBA(0.85, 0.9, 0.94, 1),   # tekst
            background=Gdk.RGBA(0.1, 0.1, 0.1, 1),     # tło
            palette=[]
        )

        # Menu PPM
        self.terminal.connect("button-press-event", self.on_right_click)

        self.add(self.terminal)

    def spawn_shell(self):
        self.terminal.spawn_async(
            Vte.PtyFlags.DEFAULT,
            os.environ['HOME'],
            ["/bin/bash"],  # lub "/bin/zsh"
            [],
            GLib.SpawnFlags.DEFAULT,
            None,
            None,
            -1,
            None,
            None
        )

    def on_right_click(self, widget, event):
        if event.button == 3:
            menu = Gtk.Menu()

            item_copy = Gtk.MenuItem(label="Kopiuj")
            item_paste = Gtk.MenuItem(label="Wklej")
            item_copy.connect("activate", lambda _: self.terminal.copy_clipboard())
            item_paste.connect("activate", lambda _: self.terminal.paste_clipboard())

            menu.append(item_copy)
            menu.append(item_paste)
            menu.show_all()
            menu.popup(None, None, None, None, event.button, event.time)

if __name__ == "__main__":
    win = TerminalWindow()
    win.connect("destroy", Gtk.main_quit)
    win.show_all()
    Gtk.main()
