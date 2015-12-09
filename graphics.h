#include <glib.h>
#include <gtk/gtk.h>

#include "game.h"

#ifndef OTHELLO_GLADE_GRAPHICS_H
#define OTHELLO_GLADE_GRAPHICS_H

/**
 * Különböző mezők (üres, fehér, fekete, kattintható) képeire
 * mutató pointerek tömbje
 *
 * Ez globális változó, de használata indokolt, különben minden egyes
 * lépésben újra be kéne tölteni a buffereket, ezzel fölösleges memóriát
 * használva. Felszabadítani sem lehet őket, mivel a képernyőn megjelenített
 * kép objektumok még hivatkozhatnak rájuk.
 */
GdkPixbuf * mezo_buffers[4];

/**
 * Fehér ill. fekete kis méretű korongok képei
 *
 * Következő játékos, illetve állás kijelzésénél.
 * Szintén indokoltan globális
 */
GdkPixbuf * kicsi_buffers[2];



/*
 * Betölti az üres, fehér és fekete mezők képeit
 */
void initBuffers();

/*
 * Program indulásakor minden mezőt üresre állít be
 */
void initGrid(GtkGrid *grid);

/*
 * Adott koordinátájú mezőn lecseréli a megjelenített képet
 */
void changeMezo(GtkGrid *grid, char x, char y, t_mezo mire);

/**
 * A felhasználó kattintását kapja el
 */
void mezoClicked(GtkEventBox * eventBox, GdkEvent  *event, char * koord);

/**
 * A teljes játéktábla újrarajzolása
 */
void refreshGrid(GtkGrid *grid, t_mezo table[8][8]);

/**
 * Következő játékos állapotjelzőt frissíti a képernyőn
 */
void refreshNextPlayer(GtkImage *korongImage, t_player *next);

/**
 * Értesítés megjelenítése, miszerint ugyanaz a játékos jön mégegyszer
 */
void dialogUjrajon(GtkWindow *window, t_player *player);

/**
 * Értesítés megjelenítése, miszerint végetért a játék
 *
 * Kiírja a győztest
 */
void dialogGameOver(GtkWindow *window, t_game *game);

/**
 * Help -> about menüből megnyíló ablak
 */
void show_about();

/**
 * Kirajzolja a két korongot az állásjelzőn
 */
void initAllas(GtkGrid * allasGrid);

/**
 * Állapotjelzőt frissíti
 */
void refreshAllas(GtkGrid * allasGrid, t_game *game);

/**
 * Megjeleníti az állapotjelzőt
 */
void showInfobox(GtkBox * infobox);

/**
 * Elrejti az állapotjelzőt
 */
void hideInfobox(GtkBox * infobox);

/**
 * Megjeleníti a következő játékos kiírást
 */
void showNextbox(GtkBox * nextbox);

/**
 * Elrejti a következő játékos kiírást
 */
void hideNextbox(GtkBox * nextbox);


/**
 * Beállítja a játéktábla rácsát
 */
void init_css();



#endif //OTHELLO_GLADE_GRAPHICS_H
