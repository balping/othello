#include <glib.h>
#include <gtk/gtk.h>


#include "othello.h"
#include "graphics.h"
#include "game.h"
//https://developer.gnome.org/gio/2.32/gio-GResource.html

//http://prognotes.net/2015/06/gtk-3-c-program-using-glade-3/

void init_css();

int main(int argc, char *argv[]){

	GtkBuilder      *builder; 
	GtkWidget       *window;

	gtk_init(&argc, &argv);

	GBytes * gladeBuffer = g_resources_lookup_data("/othello/o1.glade", G_RESOURCE_LOOKUP_FLAGS_NONE, NULL);
	const char * glade = g_bytes_get_data(gladeBuffer, NULL);



	builder = gtk_builder_new();
	gtk_builder_add_from_string (builder, glade, -1, NULL); //-1 : a string 0 terminated

	//CSS beállítása
	init_css();


	window = GTK_WIDGET(gtk_builder_get_object(builder, "window1"));
	gtk_builder_connect_signals(builder, NULL);

	//icon beállítása
	gtk_window_set_icon(GTK_WINDOW(window), gdk_pixbuf_new_from_resource("/othello/icon.png", NULL));

	//communicator-ként használjuk a fő window objektumot
	GObject * communicator = G_OBJECT(window);
	initSignals();
	//g_signal_connect(communicator, "game-table-changed", gtk_main_quit, NULL);

	g_object_unref(builder);

	gtk_widget_show(window);



	gtk_main();

	return 0;
}

/*
 * Lefut, ha a grid objektum létrejött
 *
 * Glade hívja meg.
 */
void on_grid_realize(GtkGrid * grid){
	initBuffers();
	initGrid(grid);
}


void bind_communicator(GObject * communicator, GtkGrid * grid){
	//játék objektum létrehozása
	//TODO: kilépéskor törlés
	t_game * game = g_malloc(sizeof(t_game));

	//játék logika összekötése a grafikával és a vezérléssel
	g_signal_connect_swapped(communicator, "game-table-changed", G_CALLBACK(refreshGrid), grid);
	g_signal_connect_swapped(communicator, "user-new-game", G_CALLBACK(newGame), game);
	g_signal_connect_swapped(communicator, "user-new-move", G_CALLBACK(lep), game);
	g_signal_connect_swapped(communicator, "game-player-onceagain", G_CALLBACK(dialogUjrajon), gtk_widget_get_window(GTK_WIDGET(communicator)));
}

//soronkövetkező játékos frissítése
void bind_next_player(GtkImage * korongImage, GObject * communicator){
	g_signal_connect_swapped(communicator, "game-next-player-changed", G_CALLBACK(refreshNextPlayer), korongImage);
}

void show_about(){
	gtk_show_about_dialog (NULL,
		"program-name", "Othello",
		"version", "v0.0.1",
		"comments", "GTK+-on alapuló\nklasszikus Othello játék",
		"copyright", "Kovács Balázs Marcell, 2015",
		"license-type", GTK_LICENSE_MIT_X11,

		"icon", gdk_pixbuf_new_from_resource("/othello/icon.png", NULL),

		NULL);
}

//http://www.gtkforums.com/viewtopic.php?f=3&t=988&p=72088=GTK3+with+CSS#p72088
void init_css(){
	GtkCssProvider * provider = gtk_css_provider_new();
	GdkDisplay *display = gdk_display_get_default ();
	GdkScreen *screen = gdk_display_get_default_screen (display);


	//gtk_css_provider_load_from_resource (provider, "/othello/style.css"); //ehhez nem elég friss a gtk verzióm
	
	GBytes * cssBuffer = g_resources_lookup_data("/othello/style.css", G_RESOURCE_LOOKUP_FLAGS_NONE, NULL);
	gtk_css_provider_load_from_data(provider, g_bytes_get_data(cssBuffer, NULL), -1, NULL);

	gtk_style_context_add_provider_for_screen (screen, GTK_STYLE_PROVIDER (provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

	g_object_unref (provider);
}


/**
 * Glade hívja meg. Egyetlen funkciója, hogy meghívja a new_game() fv-t
 */
void on_button_new_game_clicked(GtkButton* button, GObject * communicator){
	g_signal_emit_by_name(communicator, "user-new-game");

}


