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
 *
 * 		user-new-move:		a felhasználó rákattintott egy mezőre
 * 							params: char * kurzor: a kattintott mező 2 koordinátája
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
 */
void initSignals();

/**
 * Mező típusa.
 *
 * Ezt alkalmazzuk rajzoláskor is a mezo_buffers indexeként
 */
typedef enum { MEZO_FEHER = 0, MEZO_FEKETE = 1, MEZO_URES = 2,  MEZO_KATTINTHATO = 3} t_mezo;

/**
 * Játékos típusa.
 *
 * Leginkább a soron következő játékos nyilvántartásánál használatos
 */
typedef enum { FEHER = 0, FEKETE = 1 } t_player;

/**
 * Egész játékállapotot nyilvántartó struktúra
 */
typedef struct {
	t_mezo table[8][8]; //játéktábla
	t_player next; //soronkövetkező játékos
	char count_feher;
	char count_fekete;
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

/**
 * Megcsinálja a lépést, átfordítja a korongokat
 *
 * @param [char *] kurzor	amelyik mezőre az új korongot helyezzük. 2 elemű tömb
 */
void lep(t_game *game, char *kurzor, GObject *communicator);

/**
 * Összeszámolja, hogy hány fehér ill. fekete korong van a táblán
 *
 * Az eredményt a game változóba menti.
 */
void allasSzamol(t_game *game, GObject *communicator );


#endif //OTHELLO_GAME_H
