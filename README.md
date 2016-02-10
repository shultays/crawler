#The Dungeon

A dungeon crawler game that I developed for 7dlr (http://7drl.org/). My first application that uses curses (pdcurses actually) which was a lot of fun!

Unlike other dungeon crawlers, you don't control the adventurer but the dungeon itself. You can summon creatures (only goblins actually) or control individual creatures. Each action you take uses action points which replenish itself with time so you can't just spam monsters. You get stronger creatures as adventurer dives further into dungeon.

There are skills, eqipments, leveling, consumables in the game. Adventurer can loot enemies, use consumables and spells when necessary. I tried to write some basic AI which decides what to do for adventurer in each turn (and for your goblins). They try to use healing skills while being at low, try to cover near walls so they can't surround the adventurer.

I don't plan updating this game anymore.

##Screenshots

![ss](https://raw.githubusercontent.com/shultays/crawler/master/ss0.png)
![ss1](https://raw.githubusercontent.com/shultays/crawler/master/ss1.png)

##Usage
Game starts paused. At any time you want you can use space to pause/resume game. While game is paused, you can use your powers, scroll screen and inspect things.

Use arrow keys to scroll screen and inspect tiles. The items/creatures on the tile are displayed on right. Use page up/page down to scroll right side of window.

There are 3 actions you can take. Position your cursor to the place you want the action to happen.

* Generate a stronger creature
 
Create a single creature which has above average stats and items comparing to current level of dungeon.
* Generate a creature group

Creates a group of creates (2-4) that has same stats and levels as other creatures in this level.
* Take control of a unit

Takes control of the unit under your cursor. You can cast skills more efficiently or lure the adventurer if you want.

There is a vision limitation while creating creatures. You can't create creatures visible to player or other creatures.

Other than this, game play itself. Adventurer is pretty imba and plus he has healing potions. But evantually he dies even if you do nothing.

##Instalation

It needs pdcurses dependencies. You can use VS project files to compile
