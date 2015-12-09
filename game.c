#include <stdlib.h>

#include "game.h"
#include "graphics.h"


void newGame(t_game *game, GObject *communicator){

	//tábla feltöltése üres mezőkkel
	char x, y;
	for(x = 0; x < 8; x++){
		for(y = 0; y < 8; y++){
			game->table[x][y] = MEZO_URES;
		}
	}

	//középső kezdő korongok elhelyezése
	game->table[3][3] = MEZO_FEKETE;
	game->table[3][4] = MEZO_FEHER;
	game->table[4][3] = MEZO_FEHER;
	game->table[4][4] = MEZO_FEKETE;

	//mindig a fekete kezd
	game->next = FEKETE;

	lehetosegSzamol(game);

	//refreshGrid meghívása
	g_signal_emit_by_name(communicator, "game-table-changed", game->table);
	g_signal_emit_by_name(communicator, "game-next-player-changed", &game->next);

}


t_player otherPlayer(t_player player){
	if(player == FEKETE){
		return FEHER;
	}else{
		return FEKETE;
	}
}

bool lehetosegSzamol(t_game *game){
	char x, y;
	bool lehete;
	signed char xx, yy;
	signed char dx, dy;
	bool volte = false;

	t_player jatekos = game->next;
	t_player other_jatekos = otherPlayer(jatekos);

	//a tábla minden mezején végigmegyünk
	for(x = 0; x <= 7; x++){
		for(y = 0; y <= 7; y++){
			//ha az adott mezőn már van bábu, oda biztosan nem lehet rakni
			if(game->table[x][y] == MEZO_URES || game->table[x][y] == MEZO_KATTINTHATO){
				lehete = false;

				//szélrózsa minden irányába vizsgáljuk a lehetséges lépéseket
				for(dx = -1; dx <= 1; dx++){
				for(dy = -1; dy <= 1; dy++){
					if(!lehete && (dx != 0 || dy != 0)){ //valamilyen irányba legyen változás
						xx = x;
						yy = y;
						//adott irányba elmegyünk, amig ellenséges korongok jönnek
						do{
							yy+=dy;
							xx+=dx;
							lehete = 0 <= yy && yy <= 7 && 0 <= xx && xx <= 7 && game->table[xx][yy] == other_jatekos;
						}while(lehete);
						//ha a következő bábu saját, akkor az adott mezőre lehet lépni
						if(0 <= yy && yy <= 7 && 0 <= xx && xx <= 7 &&
							  abs(yy - y) != 1 && abs(xx - x) != 1 &&
							  game->table[xx][yy] == jatekos){
								lehete = true;
						}
					}

					//elég, hogy 1 irányba jó legyen: kilépés a ciklusból
					if(lehete){
						dx = dy = 2;
					}
				}
				}


				if(lehete){
					game->table[x][y] = MEZO_KATTINTHATO;
					volte = true;
				}else{
					game->table[x][y] = MEZO_URES;
				}

			}
		}
	}

	return volte;

}


void lep(t_game *game, char *kurzor, GObject *communicator){
	char kurzorx = kurzor[0];
	char kurzory = kurzor[1];

	t_player jatekos = game->next;
	t_player other_jatekos = otherPlayer(jatekos);

	if(game->table[kurzorx][kurzory] == MEZO_KATTINTHATO){
		game->table[kurzorx][kurzory] = (t_mezo) jatekos;
		signed char xx, yy;
		signed char dx, dy;
		bool lehete;
		char x, y;
		x = kurzorx;
		y = kurzory;

		//szérózsa minden irányába megyünk
		for(dx = -1; dx <= 1; dx++){
		for(dy = -1; dy <= 1; dy++){
			if(dx != 0 || dy != 0){ //de egyhelyben azért nem toporgunk
				xx = x;
				yy = y;
				//adott irányba elmegyünk, amig ellenséges korongok jönnek
				do{
					yy+=dy;
					xx+=dx;
					lehete = 0 <= yy && yy <= 7 && 0 <= xx && xx <= 7 && game->table[xx][yy] == other_jatekos;
				}while(lehete);
				//ha a következő bábu saját, akkor lehet fordigatni
				if(0 <= yy && yy <= 7 && 0 <= xx && xx <= 7 && abs(yy - y) != 1 && abs(xx - x) != 1 && game->table[xx][yy] == jatekos){

					//visszafele lépegetve minden bábut átfordítunk
					while(x != xx || y!= yy){
						xx -= dx;
						yy -= dy;
						game->table[xx][yy] = (t_mezo)jatekos;
					}
				}
			}
		}
		}


		game->next = other_jatekos;

		//ha nics érvényes lépés, u.a. a játékos jön
		bool lehetelepni = lehetosegSzamol(game);
		if(!lehetelepni){
			game->next = jatekos;


			lehetelepni = lehetosegSzamol(game);
			if(lehetelepni){
				g_signal_emit_by_name(communicator, "game-table-changed", game->table);
				g_signal_emit_by_name(communicator, "game-player-onceagain", &jatekos);

				return;
			}else{
				g_signal_emit_by_name(communicator, "game-end", game);
			}
		}



		//refreshGrid meghívása
		g_signal_emit_by_name(communicator, "game-table-changed", game->table);
		g_signal_emit_by_name(communicator, "game-next-player-changed", &game->next);
	}
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

	g_signal_new("user-new-move",
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
}
