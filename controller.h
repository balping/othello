#ifndef OTHELLO_CONTROLLER_H
#define OTHELLO_CONTROLLER_H


/**
 * A játéklogika és a megjelenítés signalokon keresztül kommunikál,
 * hogy a logika és a megjelenítés a lehető legjobban elkülönüljön.
 * Ez a függvény hozzáadja a communicator objecthez a játék által
 * kibocsátott signalokat. Communicatornak a főablakot választjuk
 * (window1 nevű objectum glade-ben)
 *
 * Signalok:
 * 		game-table-changed:	táblán valami megváltozott, újrarenderelés szükséges
 * 							params: t_mezo table[8][8]
 *
 * 		user-new-game:		a felhasználó megnyomta az új játék gombot
 * 							params: t_game * game
 *
 * 		game-started:		játék initializálva
 *
 * 		user-new-move:		a felhasználó rákattintott egy mezőre
 * 							params: t_kurzor * kurzor: a kattintott mező 2 koordinátája
 *
 * 		game-move-done:		lépés történt
 * 							params: t_game * game
 *
 * 		game-next-player-changed:
 * 							megváltozott a következő játékos
 * 							params: t_player * next
 *
 * 		game-player-onceagain:
 * 							játékos újrajön
 * 							params: t_player * player
 *
 * 		game-end:			vége a játéknak
 * 							params: t_game * game
 *
 * 		game-allas-changed:	megváltozott a játék állása
 * 							params: t_game * game
 *
 */
void initSignals();


/*
 * Lefut, ha a grid objektum létrejött
 *
 * Glade hívja meg. Egyetlen funkciója, hogy meghívja az initGrid fv-t
 */
void on_grid_realize(GtkGrid * grid);

/**
 * Glade hívja meg. Egyetlen funkciója, hogy meghívja a new_game() fv-t
 */
void on_button_new_game_clicked(GtkWidget* button, GObject * communicator);

/**
 * Akkor hívódik meg, ha átkapcsolják, hogy automata legyen-e a fekete
 */
void ai_fekete_switched(GtkSwitch * fekete_switch, void*nyel, void * cg[2]);

/**
 * Akkor hívódik meg, ha átkapcsolják, hogy automata legyen-e a fehér
 */
void ai_feher_switched(GtkSwitch * feher_switch, void*nyel, void * cg[2]);

#endif //OTHELLO_CONTROLLER_H
