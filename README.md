#The Dungeon

A dungeon crawler game that I developed for 7dlr (http://7drl.org/). My first application that uses curses (pdcurses actually) which was a lot of fun!

Unlike other dungeon crawlers, you don't control the adventurer but the dungeon itself. You can summon creatures (only goblins actually) or control individual creatures. Each action you take uses action points which replenish itself with time so you can't just spam monsters.

There are skills, eqipments, leveling, consumables in the game. Adventurer can loot enemies, use consumables and spells when necessary. I tried to write some basic AI which decides what to do for adventurer in each turn (and for your goblins). They try to use healing skills while being at low, try to cover near walls so they can't surround the adventurer.

I don't plan updating this game anymore.

##Screenshots

![ss](https://raw.githubusercontent.com/shultays/crawler/master/ss0.png)
![ss1](https://raw.githubusercontent.com/shultays/crawler/master/ss1.png)

##Instalation

It needs pdcurses dependencies. You can use VS project files to compile
