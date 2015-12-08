#include <stdlib.h>

#include "game.h"
#include "othello.h"
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
	bool lehete = false;
	signed char xx, yy;
	signed char dx, dy;
	bool volte = false;

	t_player jatekos = game->next;
	t_player other_jatekos = otherPlayer(jatekos);

	for(x = 0; x <= 7; x++){
		for(y = 0; y <= 7; y++){
			if(game->table[x][y] == MEZO_URES || game->table[x][y] == MEZO_KATTINTHATO){
				//fel
				xx = x;
				yy = y;
				do{
					yy--;
					lehete = yy >= 0 && game->table[xx][yy] == other_jatekos;
				}while(lehete);
				if(yy >= 0 && abs(yy - y) != 1 && game->table[xx][yy] == jatekos){
					lehete = true;
				}

				//jobbra-fel
				if(!lehete){
					xx = x;
					yy = y;
					do{
						yy--;
						xx++;
						lehete = yy >= 0 && xx <= 7 && game->table[xx][yy] == other_jatekos;
					}while(lehete);
					if(yy >= 0 && xx <= 7 && abs(yy - y) != 1 && game->table[xx][yy] == jatekos){
						lehete = true;
					}
				}

				//jobbra
				if(!lehete){
					xx = x;
					yy = y;
					do{
						xx++;
						lehete = xx <= 7 && game->table[xx][yy] == other_jatekos;
					}while(lehete);
					if(xx <= 7 && abs(xx - x) != 1 && game->table[xx][yy] == jatekos){
						lehete = true;
					}
				}

				//jobbra-le
				if(!lehete){
					xx = x;
					yy = y;
					do{
						yy++;
						xx++;
						lehete = yy <= 7 && xx <= 7 && game->table[xx][yy] == other_jatekos;
					}while(lehete);
					if(yy <= 7 && xx <= 7 && abs(yy - y) != 1 && game->table[xx][yy] == jatekos){
						lehete = true;
					}
				}

				//le
				if(!lehete){
					xx = x;
					yy = y;
					do{
						yy++;
						lehete = yy <= 7 && game->table[xx][yy] == other_jatekos;
					}while(lehete);
					if(yy <= 7 && abs(yy - y) != 1 && game->table[xx][yy] == jatekos){
						lehete = true;
					}
				}

				//balra-le
				if(!lehete){
					xx = x;
					yy = y;
					do{
						yy++;
						xx--;
						lehete = yy <= 7 && xx >= 0 && game->table[xx][yy] == other_jatekos;
					}while(lehete);
					if(yy <= 7 && xx >= 0 && abs(yy - y) != 1 && game->table[xx][yy] == jatekos){
						lehete = true;
					}
				}

				//balra
				if(!lehete){
					xx = x;
					yy = y;
					do{
						xx--;
						lehete = xx >= 0 && game->table[xx][yy] == other_jatekos;
					}while(lehete);
					if(xx >= 0 && abs(xx - x) != 1 && game->table[xx][yy] == jatekos){
						lehete = true;
					}
				}

				//balra-fel
				if(!lehete){
					xx = x;
					yy = y;
					do{
						yy--;
						xx--;
						lehete = yy >= 0 && xx >= 0 && game->table[xx][yy] == other_jatekos;
					}while(lehete);
					if(yy >= 0 && xx >= 0 && abs(yy - y) != 1 && game->table[xx][yy] == jatekos){
						lehete = true;
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
		bool lehete;
		char x, y;
		x = kurzorx;
		y = kurzory;

		//fel
		xx = x;
		yy = y;
		do{
			yy--;
			lehete = false;
			if(yy >= 0 && game->table[xx][yy] == other_jatekos){
				lehete = true;
			}
		}while(lehete);
		if(yy >= 0 && abs(yy - y) != 1 && game->table[xx][yy] == jatekos){
			xx = x;
			yy = y;
			do{
				yy--;
				lehete = false;
				if(yy >= 0 && game->table[xx][yy] == other_jatekos){
					game->table[xx][yy] = (t_mezo) jatekos;
					lehete = true;
				}
			}while(lehete);
		}

		//jobbra-fel
		xx = x;
		yy = y;
		do{
			yy--;
			xx++;
			lehete = false;
			if(yy >= 0 && xx <= 7 && game->table[xx][yy] == other_jatekos){
				lehete = true;
			}
		}while(lehete);
		if(yy >= 0 && xx <= 7 && abs(yy - y) != 1 && game->table[xx][yy] == jatekos){
			xx = x;
			yy = y;
			do{
				yy--;
				xx++;
				lehete = false;
				if(yy >= 0 && xx <= 7 && game->table[xx][yy] == other_jatekos){
					game->table[xx][yy] = (t_mezo) jatekos;
					lehete = true;
				}
			}while(lehete);
		}

		//jobbra
		xx = x;
		yy = y;
		do{
			xx++;
			lehete = false;
			if(xx <= 7 && game->table[xx][yy] == other_jatekos){
				lehete = true;
			}
		}while(lehete);
		if(xx <= 7 && abs(xx - x) != 1 && game->table[xx][yy] == jatekos){
			xx = x;
			yy = y;
			do{
				xx++;
				lehete = false;
				if(xx <= 7 && game->table[xx][yy] == other_jatekos){
					game->table[xx][yy] = (t_mezo) jatekos;
					lehete = true;
				}
			}while(lehete);
		}


		//jobbra-le
		xx = x;
		yy = y;
		do{
			yy++;
			xx++;
			lehete = false;
			if(yy <= 7 && xx <= 7 && game->table[xx][yy] == other_jatekos){
				lehete = true;
			}
		}while(lehete);
		if(yy <= 7 && xx <= 7 && abs(yy - y) != 1 && game->table[xx][yy] == jatekos){
			xx = x;
			yy = y;
			do{
				yy++;
				xx++;
				lehete = false;
				if(yy <= 7 && xx <= 7 && game->table[xx][yy] == other_jatekos){
					game->table[xx][yy] = (t_mezo) jatekos;
					lehete = true;
				}
			}while(lehete);
		}

		//le
		xx = x;
		yy = y;
		do{
			yy++;
			lehete = false;
			if(yy <= 7 && game->table[xx][yy] == other_jatekos){
				lehete = true;
			}
		}while(lehete);
		if(yy <= 7 && abs(yy - y) != 1 && game->table[xx][yy] == jatekos){
			xx = x;
			yy = y;
			do{
				yy++;
				lehete = false;
				if(yy <= 7 && game->table[xx][yy] == other_jatekos){
					game->table[xx][yy] = (t_mezo) jatekos;
					lehete = true;
				}
			}while(lehete);
		}

		//balra-le
		xx = x;
		yy = y;
		do{
			yy++;
			xx--;
			lehete = false;
			if(yy <= 7 && xx >= 0 && game->table[xx][yy] == other_jatekos){
				lehete = true;
			}
		}while(lehete);
		if(yy <= 7 && xx >= 0 && abs(yy - y) != 1 && game->table[xx][yy] == jatekos){
			xx = x;
			yy = y;
			do{
				yy++;
				xx--;
				lehete = false;
				if(yy <= 7 && xx >= 0 && game->table[xx][yy] == other_jatekos){
					game->table[xx][yy] = (t_mezo) jatekos;
					lehete = true;
				}
			}while(lehete);
		}

		//balra
		xx = x;
		yy = y;
		do{
			xx--;
			lehete = false;
			if(xx >= 0 && game->table[xx][yy] == other_jatekos){
				lehete = true;
			}
		}while(lehete);
		if(xx >= 0 && abs(xx - x) != 1 && game->table[xx][yy] == jatekos){
			xx = x;
			yy = y;
			do{
				xx--;
				lehete = false;
				if(xx >= 0 && game->table[xx][yy] == other_jatekos){
					game->table[xx][yy] = (t_mezo) jatekos;
					lehete = true;
				}
			}while(lehete);
		}

		//balra-fel
		xx = x;
		yy = y;
		do{
			yy--;
			xx--;
			lehete = false;
			if(yy >= 0 && xx >= 0 && game->table[xx][yy] == other_jatekos){
				lehete = true;
			}
		}while(lehete);
		if(yy >= 0 && xx >= 0 && abs(yy - y) != 1 && game->table[xx][yy] == jatekos){
			xx = x;
			yy = y;
			do{
				yy--;
				xx--;
				lehete = false;
				if(yy >= 0 && xx >= 0 && game->table[xx][yy] == other_jatekos){
					game->table[xx][yy] = (t_mezo) jatekos;
					lehete = true;
				}
			}while(lehete);
		}

		game->next = other_jatekos;
		lehetosegSzamol(game);

		//refreshGrid meghívása
		g_signal_emit_by_name(communicator, "game-table-changed", game->table);
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
}
