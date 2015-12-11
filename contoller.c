#include <glib.h>
#include <gtk/gtk.h>
#include <stdlib.h>


#include "othello.h"
#include "graphics.h"
#include "controller.h"


int main(int argc, char *argv[]){

	GtkBuilder      *builder; 
	GtkWidget       *window;

	gtk_init(&argc, &argv);

	//https://developer.gnome.org/gio/2.32/gio-GResource.html
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


	// ---------- játék logika összekapcsolása a vezérléssel és a grafikával --------

	//communicator-ként használjuk a fő window objektumot
	GObject * communicator = G_OBJECT(window);

	//a lent használt signálokat bejegyezzük
	initSignals();

	//játék objektum létrehozása
	t_game * game = g_malloc(sizeof(t_game));

	//tábla újrarajzolása
	g_signal_connect_swapped(communicator, "game-table-changed", G_CALLBACK(refreshGrid), GTK_GRID( gtk_builder_get_object(builder, "grid_game")));

	//új játék indítása
	g_signal_connect_swapped(communicator, "user-new-game", G_CALLBACK(newGame), game);

	//manuális lépés
	g_signal_connect_swapped(communicator, "user-new-move", G_CALLBACK(lep), game);

	//ha egy lépés befejeződött, akkor arról informáljuk az automatát, hátha lépni szeretne
	g_signal_connect(communicator, "game-move-done", G_CALLBACK(ai_lep), NULL);

	//ha ugyanaz a játékos jön újra, arról figyelmeztetést adunk
	g_signal_connect_swapped(communicator, "game-player-onceagain", G_CALLBACK(dialogUjrajon), gtk_widget_get_window(GTK_WIDGET(communicator)));

	//játék vége, eredmény kijelzése
	g_signal_connect_swapped(communicator, "game-end", G_CALLBACK(dialogGameOver), gtk_widget_get_window(GTK_WIDGET(communicator)));

	//soronkövetkező játékos frissítése az állapotjelzőn
	g_signal_connect_swapped(communicator, "game-next-player-changed", G_CALLBACK(refreshNextPlayer), gtk_builder_get_object(builder, "image_next_player"));

	//bufferek betöltése
	initBuffers();

	//állapotjelsző frissítése
	GtkGrid * allasGrid = GTK_GRID(gtk_builder_get_object(builder, "allasgrid"));
	initAllas(allasGrid);
	g_signal_connect_swapped(communicator, "game-allas-changed", G_CALLBACK(refreshAllas), allasGrid);

	//állapotjelsző megjelenítése és elrejtése attól függően, hogy megy-e a játék
	GtkBox * infobox = GTK_BOX(gtk_builder_get_object(builder, "infobox"));
	g_signal_connect_swapped(communicator, "game-started", G_CALLBACK(showInfobox), infobox);
	//g_signal_connect_swapped(communicator, "game-end", G_CALLBACK(hideInfobox), infobox);
	hideInfobox(infobox);

	//következő játékos megjelenítése és elrejtése attól függően, hogy megy-e a játék
	/*GtkBox * nextbox = GTK_BOX(gtk_builder_get_object(builder, "nextbox"));
	g_signal_connect_swapped(communicator, "game-started", G_CALLBACK(showNextbox), nextbox);
	g_signal_connect_swapped(communicator, "game-end", G_CALLBACK(hideNextbox), nextbox);
	hideNextbox(nextbox);*/
	//ez azért van kikapcsolva, mert valamiért nem működik együtt az állapotjelző ki-be kapcsolgatásával

	//ai kapcsolók
	void * cg[2] = {(void*)communicator, (void*)game};
	g_signal_connect(gtk_builder_get_object(builder, "white_ai"), "notify::active", G_CALLBACK(ai_feher_switched), cg);
	g_signal_connect(gtk_builder_get_object(builder, "black_ai"), "notify::active", G_CALLBACK(ai_fekete_switched), cg);




	g_object_unref(builder);

	gtk_widget_show(window);



	gtk_main();

	free(game);

	return 0;
}

void show_help(GtkImageMenuItem * menu, GtkWindow * helpwindow){
	gtk_window_set_icon(helpwindow, gdk_pixbuf_new_from_resource("/othello/icon.png", NULL));
	gtk_window_set_deletable(helpwindow, false);
	gtk_window_set_resizable(helpwindow, false);
	gtk_widget_show(GTK_WIDGET( helpwindow));
}

void hide_help(GtkButton * button, GtkWindow * helpwindow){
	gtk_widget_hide(GTK_WIDGET(helpwindow));
}





void on_grid_realize(GtkGrid * grid){
	initGrid(grid);
}


void on_button_new_game_clicked(GtkWidget* button, GObject * communicator){
	g_signal_emit_by_name(communicator, "user-new-game");
}


void ai_feher_switched(GtkSwitch * feher_switch, void*nyel, void * cg[2]){
	t_game * game = (t_game *) cg[1];
	GObject * communicator = G_OBJECT(cg[0]);
	changeAi(communicator,game,FEHER,(bool)gtk_switch_get_active(feher_switch));
}

void ai_fekete_switched(GtkSwitch * fekete_switch, void*nyel, void * cg[2]){
	t_game * game = (t_game *) cg[1];
	GObject * communicator = G_OBJECT(cg[0]);
	changeAi(communicator,game,FEKETE,(bool)gtk_switch_get_active(fekete_switch));
}



void initSignals(){
	g_signal_new("game-table-changed",
				 G_TYPE_OBJECT, G_SIGNAL_RUN_FIRST,
				 0, NULL, NULL,
				 g_cclosure_marshal_VOID__BOXED,
				 G_TYPE_NONE, 1, G_TYPE_POINTER);

	g_signal_new("user-new-game",
				 G_TYPE_OBJECT, G_SIGNAL_RUN_FIRST,
				 0, NULL, NULL,
				 g_cclosure_marshal_VOID__BOXED,
				 G_TYPE_NONE, 0);

	g_signal_new("game-started",
				 G_TYPE_OBJECT, G_SIGNAL_RUN_FIRST,
				 0, NULL, NULL,
				 g_cclosure_marshal_VOID__BOXED,
				 G_TYPE_NONE, 0);

	g_signal_new("user-new-move",
				 G_TYPE_OBJECT, G_SIGNAL_RUN_FIRST,
				 0, NULL, NULL,
				 g_cclosure_marshal_VOID__BOXED,
				 G_TYPE_NONE, 1, G_TYPE_POINTER);

	g_signal_new("game-move-done",
				 G_TYPE_OBJECT, G_SIGNAL_RUN_FIRST,
				 0, NULL, NULL,
				 g_cclosure_marshal_VOID__BOXED,
				 G_TYPE_NONE, 1, G_TYPE_POINTER);

	g_signal_new("game-next-player-changed",
				 G_TYPE_OBJECT, G_SIGNAL_RUN_FIRST,
				 0, NULL, NULL,
				 g_cclosure_marshal_VOID__BOXED,
				 G_TYPE_NONE, 1, G_TYPE_POINTER);

	g_signal_new("game-player-onceagain",
				 G_TYPE_OBJECT, G_SIGNAL_RUN_FIRST,
				 0, NULL, NULL,
				 g_cclosure_marshal_VOID__BOXED,
				 G_TYPE_NONE, 1, G_TYPE_POINTER);

	g_signal_new("game-end",
				 G_TYPE_OBJECT, G_SIGNAL_RUN_FIRST,
				 0, NULL, NULL,
				 g_cclosure_marshal_VOID__BOXED,
				 G_TYPE_NONE, 1, G_TYPE_POINTER);

	g_signal_new("game-allas-changed",
				 G_TYPE_OBJECT, G_SIGNAL_RUN_FIRST,
				 0, NULL, NULL,
				 g_cclosure_marshal_VOID__BOXED,
				 G_TYPE_NONE, 1, G_TYPE_POINTER);

}