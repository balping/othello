#include <glib.h>
#include <gtk/gtk.h>

#include "othello.h"
#include "graphics.h"


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

	g_signal_emit_by_name(communicator, "user-new-move", koord);
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
