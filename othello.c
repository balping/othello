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

//glade hívja meg
void bind_communicator(GObject * communicator, GtkGrid * grid){
	//játék objektum létrehozása
	//TODO: kilépéskor törlés
	t_game * game = g_malloc(sizeof(t_game));

	//játék logika összekötése a grafikával és a vezérléssel
	g_signal_connect_swapped(communicator, "game-table-changed", G_CALLBACK(refreshGrid), grid);
	g_signal_connect_swapped(communicator, "user-new-game", G_CALLBACK(newGame), game);
	g_signal_connect_swapped(communicator, "user-new-move", G_CALLBACK(lep), game);
	g_signal_connect(communicator, "game-move-done", G_CALLBACK(ai_lep), NULL);
	g_signal_connect_swapped(communicator, "game-player-onceagain", G_CALLBACK(dialogUjrajon), gtk_widget_get_window(GTK_WIDGET(communicator)));
	g_signal_connect_swapped(communicator, "game-end", G_CALLBACK(dialogGameOver), gtk_widget_get_window(GTK_WIDGET(communicator)));

	g_signal_connect_swapped(communicator, "user-ai-fekete-changed", G_CALLBACK(changeAiFekete), game);
	g_signal_connect_swapped(communicator, "user-ai-feher-changed", G_CALLBACK(changeAiFeher), game);
}

//soronkövetkező játékos frissítése
//glade hívja meg
void bind_next_player(GtkImage * korongImage, GObject * communicator){
	g_signal_connect_swapped(communicator, "game-next-player-changed", G_CALLBACK(refreshNextPlayer), korongImage);
}

//állapotjelsző frissítése
//glade hívja meg
void bind_allas_update(GtkGrid * allasGrid, GObject * communicator){
	initAllas(allasGrid);
	g_signal_connect_swapped(communicator, "game-allas-changed", G_CALLBACK(refreshAllas), allasGrid);
}

//állapotjelsző megjelenítése és elrejtése attól függően, hogy megy-e a játék
//glade hívja meg
void bind_infobox(GtkBox * infobox, GObject * communicator){

	g_signal_connect_swapped(communicator, "game-started", G_CALLBACK(showInfobox), infobox);
	//g_signal_connect_swapped(communicator, "game-end", G_CALLBACK(hideInfobox), infobox);
	hideInfobox(infobox);
}

//következő játékos megjelenítése és elrejtése attól függően, hogy megy-e a játék
//glade hívja meg
void bind_nextbox(GtkBox * nextbox, GObject * communicator){

	//g_signal_connect_swapped(communicator, "game-started", G_CALLBACK(showNextbox), nextbox);
	//g_signal_connect_swapped(communicator, "game-end", G_CALLBACK(hideNextbox), nextbox);
	//hideNextbox(nextbox);
}

/**
 * Glade hívja meg. Egyetlen funkciója, hogy meghívja a new_game() fv-t
 */
void on_button_new_game_clicked(GtkWidget* button, GObject * communicator){
	g_signal_emit_by_name(communicator, "user-new-game");

}


void ai_fekete_switched(GtkSwitch * fekete_switch, GObject * communicator){
	//g_signal_emit_by_name(communicator, "user-ai-fekete-changed", gtk_switch_get_active(fekete_switch));
}

void ai_feher_switched(GtkSwitch * feher_switch, GObject * communicator){
	//g_signal_emit_by_name(communicator, "user-ai-feher-changed", gtk_switch_get_active(feher_switch));
}

