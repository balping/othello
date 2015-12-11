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
	game->table[3][3] = MEZO_FEHER;
	game->table[3][4] = MEZO_FEKETE;
	game->table[4][3] = MEZO_FEKETE;
	game->table[4][4] = MEZO_FEHER;

	//mindig a fekete kezd
	game->next = FEKETE;


	lehetosegSzamol(game);
	allasSzamol(game, communicator);


	g_signal_emit_by_name(communicator, "game-started");
	g_signal_emit_by_name(communicator, "game-table-changed", game->table);
	g_signal_emit_by_name(communicator, "game-next-player-changed", &game->next);
	g_signal_emit_by_name(communicator, "game-move-done", game);

}


t_player otherPlayer(t_player player){
	if(player == FEKETE){
		return FEHER;
	}else{
		return FEKETE;
	}
}

char lehetosegSzamol(t_game *game){
	char x, y;
	bool lehete;
	signed char xx, yy;
	signed char dx, dy;
	char n_kattinthato = 0; //hány megjátszható mező van a táblán

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
					n_kattinthato++;
				}else{
					game->table[x][y] = MEZO_URES;
				}

			}
		}
	}

	return n_kattinthato;

}


void lep(t_game *game, t_kurzor * kurzor, GObject *communicator){
	t_player jatekos = game->next;
	t_player other_jatekos = otherPlayer(jatekos);


	if(game->table[kurzor->x][kurzor->y] == MEZO_KATTINTHATO){
		game->table[kurzor->x][kurzor->y] = (t_mezo) jatekos;
		signed char xx, yy;
		signed char dx, dy;
		bool lehete;
		char x, y;
		x = kurzor->x;
		y = kurzor->y;

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

		//csak az ai hívta meg, nem valódi lépés
		if(communicator == NULL){
			return;
		}

		allasSzamol(game, communicator);

		//ha nics érvényes lépés, u.a. a játékos jön
		bool lehetelepni = lehetosegSzamol(game);
		if(!lehetelepni){
			game->next = jatekos;


			lehetelepni = lehetosegSzamol(game);
			if(lehetelepni){
				g_signal_emit_by_name(communicator, "game-table-changed", game->table);
				g_signal_emit_by_name(communicator, "game-player-onceagain", &jatekos);
				g_signal_emit_by_name(communicator, "game-move-done", game);

				return;
			}else{
				g_signal_emit_by_name(communicator, "game-table-changed", game->table);
				g_signal_emit_by_name(communicator, "game-end", game);

				return;
			}
		}




		//refreshGrid meghívása
		g_signal_emit_by_name(communicator, "game-table-changed", game->table);
		g_signal_emit_by_name(communicator, "game-next-player-changed", &game->next);
		g_signal_emit_by_name(communicator, "game-move-done", game);
	}
}

void allasSzamol(t_game *game, GObject *communicator){
	char x,y;

	char c_feher = 0;  //fehérek száma
	char c_fekete = 0; //feketék száma

	for(x=0;x<8;x++){
	for(y=0;y<8;y++){
		if(game->table[x][y] == MEZO_FEHER){
			c_feher++;
		}else if(game->table[x][y] == MEZO_FEKETE){
			c_fekete++;
		}
	}
	}

	game->count_feher = c_feher;
	game->count_fekete = c_fekete;

	g_signal_emit_by_name(communicator, "game-allas-changed", game);
}

t_kurzor ai_legjobbMezo(t_game *game){

	//legjobb mező
	struct {
		t_kurzor kurzor;
		char score;
	} best;

	//elméletileg max 64 lépés lehetséges (erősen felülbecsülve!)
	//mivel minimumot keresünk, mindenképp lesz mi inicializálja
	//a best.x, best.y változókat
	best.score = 100;

	t_game proba_allas;
	char score_tmp;
	t_kurzor kurzor;

	//végig lépdelünk az összes kattintható mezőn
	//és  kiszámoljuk minden esetben, hogy az ellenfélnek
	//hány legális lépése lenne. Minimumra törekszünk
	for(kurzor.x=0; kurzor.x<8; kurzor.x++){
	for(kurzor.y=0; kurzor.y<8; kurzor.y++){
		if(game->table[kurzor.x][kurzor.y] == MEZO_KATTINTHATO){
			//lemásoljuk a játékállapotot

			proba_allas = *game;

			//megcsináljuk a próbalépést
			lep(&proba_allas, &kurzor, NULL);

			//megszámoljuk az ellenfél hányat tudna lépni
			score_tmp = lehetosegSzamol(&proba_allas);

			//minimumot keresünk
			if(score_tmp < best.score){
				best.kurzor.x = kurzor.x;
				best.kurzor.y = kurzor.y;
				best.score = score_tmp;
			}

		}
	}
	}

	//visszatérés
	return best.kurzor;
}


void ai_lep(GObject *communicator, t_game *game){
	//csak akkor lép az ai, ha a beállítás úgy kívánja
	if(game->ai[game->next]){
		t_kurzor kurzor = ai_legjobbMezo(game);
		lep(game, &kurzor, communicator);
	}

}


void changeAi(GObject *communicator, t_game *game, t_player player, bool allapot){
	game->ai[player] = allapot;

	//trükközés, hogy ha az éppen soronkövetkező játékost kapcsolják be, mint ai, akkor lépjen
	g_signal_emit_by_name(communicator, "game-move-done", game);
}

