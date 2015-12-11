#include <glib.h>
#include <gtk/gtk.h>

#include "othello.h"
#include "graphics.h"
#include "game.h"


void initGrid(GtkGrid *grid){
	GtkWidget * mezo;
	GtkWidget * eventBox;
	char * koord;

	char i, j;
	for(i = 0; i < 8; i++){
		for(j = 0; j < 8; j++){
			eventBox = gtk_event_box_new(); //erre lehet kattintani, ez tartalmazza majd a képet

			//ez alapján azonosítjuk kattintáskor, hogy milyen mezőre kattintottak
			koord = g_malloc(sizeof(char)*2);
			koord[0] = i;
			koord[1] = j;

			//klikkelés figyelése
			gtk_widget_set_events (eventBox, GDK_BUTTON_PRESS_MASK);
			g_signal_connect(eventBox, "button_press_event", G_CALLBACK(mezoClicked), koord);

			//kép betöltése
			mezo = gtk_image_new_from_pixbuf(mezo_buffers[MEZO_URES]);

			//eventBox hozzáadása a gridhez
			gtk_grid_attach(grid, eventBox, i, j, 1, 1);

			//kép hozzáadása az eventboxhoz
			gtk_container_add(GTK_CONTAINER(eventBox), mezo);

			//objektumok megjelenítése
			gtk_widget_show(eventBox);
			gtk_widget_show(mezo);

		}
	}



}





void initBuffers(){
	//https://developer.gnome.org/gdk-pixbuf/unstable/gdk-pixbuf-File-Loading.html#gdk-pixbuf-new-from-resource

	mezo_buffers[MEZO_URES] = gdk_pixbuf_new_from_resource("/othello/ures.png", NULL);
	mezo_buffers[MEZO_FEHER] = gdk_pixbuf_new_from_resource("/othello/feher.png", NULL);
	mezo_buffers[MEZO_FEKETE] = gdk_pixbuf_new_from_resource("/othello/fekete.png", NULL);
	mezo_buffers[MEZO_KATTINTHATO] = gdk_pixbuf_new_from_resource("/othello/kattinthato.png", NULL);

	kicsi_buffers[FEHER] = gdk_pixbuf_new_from_resource("/othello/feher_kicsi.png", NULL);
	kicsi_buffers[FEKETE] = gdk_pixbuf_new_from_resource("/othello/fekete_kicsi.png", NULL);
}

void changeMezo(GtkGrid *grid, char x, char y, t_mezo mire){
	GtkContainer * eventBox = GTK_CONTAINER(gtk_grid_get_child_at(grid, x, y));//adott koordinátájú képhez tartozó container bekérése

	GList * children = gtk_container_get_children(eventBox);


	GtkImage * mezo_kep = GTK_IMAGE(g_list_first(children)->data);
	//g_list_free(children);

	gtk_image_set_from_pixbuf(mezo_kep, mezo_buffers[mire]);


	//cursor beállítása
	GdkWindow * window = gtk_widget_get_window(GTK_WIDGET(mezo_kep));
	if(mire == MEZO_KATTINTHATO){
		gdk_window_set_cursor(window, gdk_cursor_new(GDK_HAND1));
	}else{
		gdk_window_set_cursor(window, gdk_cursor_new(GDK_ARROW));
	}
}

void mezoClicked(GtkEventBox * eventBox, GdkEvent  *event, char * koord){
	if(event->button.button != 1 ){ //csak balegér érdekes
		return;
	}

	//communicator lekérése
	GObject * communicator = G_OBJECT(gtk_widget_get_toplevel(GTK_WIDGET(eventBox)));

	t_kurzor kurzor = {koord[0], koord[1]};
	g_signal_emit_by_name(communicator, "user-new-move", &kurzor);
}

void refreshGrid(GtkGrid *grid, t_mezo table[8][8]){
	char x, y;
	for(x=0; x<8; x++){
	for(y=0; y<8; y++){
		changeMezo(grid, x, y, table[x][y] );
	}
	}
}

void refreshNextPlayer(GtkImage *korongImage, t_player *next){
	gtk_image_set_from_pixbuf(korongImage, kicsi_buffers[ (t_mezo) *next ]);
}

void dialogUjrajon(GtkWindow *window, t_player *player){
	GtkWidget *dialog;
	dialog = gtk_message_dialog_new(NULL,
									 GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
									 GTK_MESSAGE_INFO,
									 GTK_BUTTONS_OK,
									 *player == FEHER ? "Újra a fehér következik." : "Újra a fekete következik.");
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}

void dialogGameOver(GtkWindow *window, t_game *game){
	char *message;
	if(game->count_feher > game->count_fekete){
		message = "Fehér nyert.";
	}else if(game->count_feher < game->count_fekete){
		message = "Fekete nyert";
	}else{
		message = "Döntetlen";
	}

	GtkWidget *dialog;
	dialog = gtk_message_dialog_new(NULL,
									GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
									GTK_MESSAGE_INFO,
									GTK_BUTTONS_OK,
									message);
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}

void show_about(){
	gtk_show_about_dialog (NULL,
						   "program-name", "Othello",
						   "version", "v1.0.0",
						   "comments", "GTK+-on alapuló\nklasszikus Othello játék",
						   "copyright", "Kovács Balázs Marcell, 2015",
						   "license-type", GTK_LICENSE_MIT_X11,

						   "icon", gdk_pixbuf_new_from_resource("/othello/icon.png", NULL),

						   NULL);
}

void initAllas(GtkGrid *allasGrid){
	GtkWidget * feherkorong = gtk_image_new_from_pixbuf(kicsi_buffers[FEHER]);
	GtkWidget * feketekorong = gtk_image_new_from_pixbuf(kicsi_buffers[FEKETE]);

	gtk_grid_attach(allasGrid, feherkorong, 0, 0, 1, 1);
	gtk_grid_attach(allasGrid, feketekorong, 1, 0, 1, 1);

	gtk_widget_show(feherkorong);
	gtk_widget_show(feketekorong);
}

void refreshAllas(GtkGrid *allasGrid, t_game *game){
	GtkLabel * feherlabel = GTK_LABEL(gtk_grid_get_child_at(allasGrid, 0, 1));
	GtkLabel * feketelabel = GTK_LABEL(gtk_grid_get_child_at(allasGrid, 1, 1));

	//szám szöveggé
	gchar *feherText =  g_strdup_printf("%d", (int)game->count_feher);
	gchar *feketeText =  g_strdup_printf("%d", (int)game->count_fekete);

	gtk_label_set_text(feherlabel, feherText);
	gtk_label_set_text(feketelabel, feketeText);
}

void showInfobox(GtkBox * infobox){
	gtk_widget_set_visible(GTK_WIDGET(infobox), true);
}

void hideInfobox(GtkBox *infobox){
	gtk_widget_set_visible(GTK_WIDGET(infobox), false);
}

void showNextbox(GtkBox *nextbox){
	gtk_widget_set_visible(GTK_WIDGET(nextbox), true);
}

void hideNextbox(GtkBox *nextbox){
	gtk_widget_set_visible(GTK_WIDGET(nextbox), false);
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


void init_css(){
//http://www.gtkforums.com/viewtopic.php?f=3&t=988&p=72088=GTK3+with+CSS#p72088
	GtkCssProvider * provider = gtk_css_provider_new();
	GdkDisplay *display = gdk_display_get_default ();
	GdkScreen *screen = gdk_display_get_default_screen (display);


	//gtk_css_provider_load_from_resource (provider, "/othello/style.css"); //ehhez nem elég friss a gtk verzióm

	GBytes * cssBuffer = g_resources_lookup_data("/othello/style.css", G_RESOURCE_LOOKUP_FLAGS_NONE, NULL);
	gtk_css_provider_load_from_data(provider, g_bytes_get_data(cssBuffer, NULL), -1, NULL);

	gtk_style_context_add_provider_for_screen (screen, GTK_STYLE_PROVIDER (provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

	g_object_unref (provider);
}


