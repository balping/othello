#include <gtk/gtk.h>
#include "othello.h"

#ifndef OTHELLO_GAME_H
#define OTHELLO_GAME_H

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
 */
void initSignals();

/**
 * Mező típusa.
 *
 * Ezt alkalmazzuk rajzoláskor is a mezo_buffers indexeként
 */
typedef enum { MEZO_URES = 0, MEZO_FEHER = 1, MEZO_FEKETE = 2, MEZO_KATTINTHATO = 3} t_mezo;

/**
 * Játékos típusa.
 *
 * Leginkább a soron következő játékos nyilvántartásánál használatos
 */
typedef enum { FEHER = 1, FEKETE = 2 } t_player;

/**
 * Egész játékállapotot nyilvántartó struktúra
 */
typedef struct {
	t_mezo table[8][8]; //játéktábla
	t_player next; //soronkövetkező játékos
} t_game;


/**
 * Játék inicializálása
 */
void newGame(t_game *game, GObject *communicator);

/**
 * Fehérhez feketét, feketéhez fehéret rendel
 */
t_player otherPlayer(t_player player);

/**
 * Végigmegy a tábla összes mezőjén és ami a következő
 * lépésben megjátszható, azt megjelöli.
 *
 * Visszatérés: van-e megjátszható mező a táblán
 */
bool lehetosegSzamol(t_game *game);

void lep(t_game *game, char *kurzor, GObject *communicator);

#endif //OTHELLO_GAME_H
