#include <gtk/gtk.h>
#include "othello.h"

#ifndef OTHELLO_GAME_H
#define OTHELLO_GAME_H


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

	bool ai[2]; //adott játkos automata-e
} t_game;

/**
 * A lep() és az őt meghívó fv-ek használják
 *
 * A tábla egy mezőjének koordinátái
 */
typedef struct {
	char x;
	char y;
} t_kurzor;

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
 * Visszatérés: hány megjátszható mező van a táblán
 */
char lehetosegSzamol(t_game *game);

/**
 * Megcsinálja a lépést, átfordítja a korongokat
 *
 * @param [t_kurzor] kurzor	amelyik mezőre az új korongot helyezzük. 2 elemű tömb
 * @param [GObject *] communicator	Ezen keresztül adja ki a signalokat. Ha NULL, akkor nem valódi a lépés, csak az ai számításaiohz kell
 *
 */
void lep(t_game *game, t_kurzor * kurzor, GObject *communicator);

/**
 * Összeszámolja, hogy hány fehér ill. fekete korong van a táblán
 *
 * Az eredményt a game változóba menti.
 */
void allasSzamol(t_game *game, GObject *communicator );

/**
 * Melyik mezőre lépjen az automata
 *
 * @param [t_game *]	game	jelenlegi állás
 * @return [t_kurzor]		kurzor		a meglépendő mező koordinátái (visszatérés)
 */
t_kurzor ai_legjobbMezo(t_game * game);

/**
 * Ha az aktuális játékos éppen automata, lép
 */
void ai_lep(GObject *communicator, t_game *game);

/**
 * Megváltoztatja, hogy melyik játékos legyen automata
 */
void changeAi(GObject *communicator, t_game * game, t_player player, bool allapot);



#endif //OTHELLO_GAME_H
