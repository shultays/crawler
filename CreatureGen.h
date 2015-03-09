#pragma once
#include "Creature.h"

Weapon* getWeapon(int level) {
	Weapon *w = new Weapon();

	int rangeMin = 0;
	int rangeMax = 4;

	bool no_weapon = level < 4?ran(4-level)!=0:0;
	if(level == -1){
		no_weapon = true;
	}
	if(no_weapon){
		rangeMin = rangeMax = 0;
	}else{
		rangeMin = 1;
		rangeMax = 4;
	}
	w->type = rangeMin + ran(rangeMax - rangeMin + 1);
	switch (w->type) {
		case 0:
			w->minDamage = 1 + level;
			w->maxDamage = w->minDamage + 2;
			w->swingTime = 15.0f;
			strcpy_s(w->name, "Punchs");
			w->range = 1;
			break;
		case 1:
			w->minDamage = 4 + ran(4) + level;
			w->maxDamage = w->minDamage + 4 + ran(3 + level * 2);
			w->swingTime = ranf(15.0f, 20.0f);
			strcpy_s(w->name, "Sword");
			w->range = 1;
			break;
		case 2:
			w->minDamage = 2 + level;
			w->maxDamage = w->minDamage + ran(3 + level) + 2;
			w->swingTime = ranf(14.0f, 18.0f);
			strcpy_s(w->name, "Dagger");
			w->range = 1;
			break;
		case 3:
			w->minDamage = 6 + ran(6) + level;
			w->maxDamage = w->minDamage + 8 + ran(3 + level * 2);
			w->swingTime = ranf(17.0f, 22.0f);
			w->range = 1;
			strcpy_s(w->name, "Club");
			break;
		case 4:
			w->minDamage = 2 + ran(2) + level;
			w->maxDamage = w->minDamage + 12 + ran(3 + level * 2);
			w->swingTime = ranf(17.0f, 22.0f);
			w->range = 2;
			strcpy_s(w->name, "Spear");
			break;
	}
	return w;
}

Armor* getArmor(int level) {
	Armor *a = new Armor();

	int rangeMin = 0;
	int rangeMax = 4;

	bool no_armor;
	if(level < 2){
		no_armor = true;
	}else if(level <= 5){
		no_armor = ran(5 - level + 1)!=0;
	}else{
		no_armor = false;
	}
	

	if(no_armor){
		rangeMin = rangeMax = 0;
	}else{
		rangeMin = 1;
		rangeMax = (int)((level-2)*0.5f) + 1;
		if(rangeMax<1) rangeMax = 1;
		if(rangeMax>5) rangeMax = 5;
	}
	a->type = rangeMin + ran(rangeMax - rangeMin + 1);
	switch (a->type) {
	case 0:
		a->DR = 0;
		a->slowness = 1.0f;
		strcpy_s(a->name, "No Armor");
		break;
	case 1:
		a->DR = 1+ran(2 + (level-2)/6);
		a->slowness = 1.0f;
		strcpy_s(a->name, "Cloth Armor");
		break;
	case 2:
		a->DR = 2+ran(2 + (level-2)/4);
		a->slowness = 1.1f;
		strcpy_s(a->name, "Leather Armor");
		break;
	case 3:
		a->DR = 4+ran(4 + (level-2)/3);
		a->slowness = 1.3f;
		strcpy_s(a->name, "Mail Armor");
		break;
	case 4:
		a->DR = 5+ran(6 + (int)((level-2)/2.5f));
		a->slowness = 1.3f;
		strcpy_s(a->name, "Plate Armor");
		break;
	case 5:
		a->DR = 6+ran(8 + (int)(level/2.5f));
		a->slowness = 1.5f;
		strcpy_s(a->name, "Full Plate Armor");
		break;
	}
	return a;
}

Creature* generateGoblin(int level) {
	Creature* c = new Creature();
	strcpy_s(c->name, "Goblin");
	c->level = level;
	c->hpMax = 20 + level * 10 + ran(4) * 5;
	c->hp = c->hpMax;
	c->mp = 0;
	c->minExp = 30.0f + level*10.0f;
	c->maxExp = c->minExp*1.2f;
	c->sight = 6.0f;
	c->movePerTick = 6.0f;
	c->weapon = getWeapon(level);
	c->armor = getArmor(level);
	c->explores = false;
	c->wandersAround = true;
	c->type = GOBLIN;
	c->level = level;
	c->pixel.character = 'g';
	c->pixel.color = getColorIndex(0, 4, 2);
	c->tickToRegen = 100.0f;
	return c;
}