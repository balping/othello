## Moved to GitLab

**Warning**: This project has been moved to GitLab: https://gitlab.com/balping/othello

# Othello

GTK+-ra épülő klasszikus Othello játék.

Forráskód a [GitLab-en](https://gitlab.com/balping/othello/) elérhető.


## Játékszabály

A játékot két játékos játssza, az egyik a fehér, a másik a fekete. A játékosok felváltva jönnek. Egy lépésben a soron következő játékos 1 db. korongot helyez a tábla egy üres mezejére úgy, hogy legalább egy ellenséges korongot átfordítson a következőek szerint:

* az újonnan lerakott korong és egy, már a táblán levő saját színű korong közrefog ellenséges korongokat
* a közrefogás mind a 8 irányba értelmezendő
* az összes közrefogott ellenséges korong átfordul

Ha a soron következő játékos nem tud rakni a  fentiek szerint, akkor az ellenfél következik.

Ha egyik játékos sem tud rakni (vagy ha a tábla betelik), a játék véget ér.

A győztes, akinek a játék végén több korongja van a táblán.

## Használat

Egér segítségével kiválaszthatjuk, hogy hova szeretnénk rakni. A program világosabb színnel jelzi a lehetséges mezőket.

A soron következő játékost a program a jobb felső sarokban kijelzi az aktuális állással együtt.

Játékot az Új játék gomb megnyomásával lehet indítani.

A program tartalmaz egy beépített automata ellenfelet, ezt a játék során bármikor ki- és bekapcsolhatjuk.


## Fordítás

Linux alatt:


	$ mkdir build
	$ cd build
	$ cmake ..
	$ make


Futtatás:
```
$ ./othello
```

Dependenciák:
* cmake >=2.8.6
* GTK >=3.0
* pkg-config
* glib-compile-resources