#pragma once
#include "Creature.h"

Weapon getWeapon(int level) {
	Weapon w;

	int rangeMin = 0;
	int rangeMax = 4;

	if (level > 2) {
		rangeMin++;
	}

	switch (rangeMin + ran(rangeMax - rangeMin)) {
		case 0:
			w.minDamage = 1 + level;
			w.maxDamage = w.minDamage + ran(1 + level) + 2;
			w.swingTime = ranf(13.0f, 17.0f);
			strcpy_s(w.name, "Punchs");
			w.range = 1;
			break;
		case 1:
			w.minDamage = 4 + ran(4) + level;
			w.maxDamage = w.minDamage + 4 + ran(3 + level * 2);
			w.swingTime = ranf(15.0f, 20.0f);
			strcpy_s(w.name, "Sword");
			w.range = 1;
			break;
		case 2:
			w.minDamage = 2 + level;
			w.maxDamage = w.minDamage + ran(3 + level) + 2;
			w.swingTime = ranf(14.0f, 18.0f);
			strcpy_s(w.name, "Dagger");
			w.range = 1;
			break;
		case 3:
			w.minDamage = 6 + ran(6) + level;
			w.maxDamage = w.minDamage + 8 + ran(3 + level * 2);
			w.swingTime = ranf(17.0f, 22.0f);
			w.range = 1;
			strcpy_s(w.name, "Club");
			break;
		case 4:
			w.minDamage = 2 + ran(2) + level;
			w.maxDamage = w.minDamage + 12 + ran(3 + level * 2);
			w.swingTime = ranf(17.0f, 22.0f);
			w.range = 2;
			strcpy_s(w.name, "Spear");
			break;
	}
	return w;
}

Creature generateGoblin(int level) {
	Creature c;
	strcpy_s(c.name, "Goblin");
	c.level = level;
	c.hp = 20 + level * 10 + ran(4) * 5;
	c.mp = 0;
	c.minExp = 30.0f + level*10.0f;
	c.maxExp = c.minExp*1.2f;
	c.sight = 4.0f;
	c.movePerTick = 6.0f;
	c.weapon = getWeapon(0);
	c.type = GOBLIN;
	c.level = level;
	c.pixel.character = 'g';
	c.pixel.color = getColorIndex(0, 4, 2);
	return c;
}