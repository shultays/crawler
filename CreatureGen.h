#pragma once
#include"Tools.h"
enum {
	HEAL_POT = 1,
	REGEN_POT,
	MANA_POT,
	BUTTERFLY_SCROLL,
	LIGHTING_SCROLL,
	FIRE_SCROLL,
	ICE_SCROLL,
	CHARM_SCROLL,
	ENCH_WEAPON,
	ENCH_ARMOR,
	CONS_MAX
};

class Equipment;
class Weapon;
class Consumable;
class Creature;

void addBuff(Equipment* e, vector<int>& weights, int level, int maxBuffCount = 1, int noBuffChance = 100, bool addName = true);
Weapon* getWeapon(int level);
Equipment* getArmor(int level, bool overrideNoArmor = false);
Equipment* getShield(int level);
Equipment* getHelm(int level);
Equipment* getBoots(int level);
Equipment* getGloves(int level);
Equipment* getRing(int level);
Equipment* getAmulet(int level);

Consumable* getConsumable(int level, int force = -1);

Creature* generateGoblin(int level, Pos pos);
Creature* generateButterfly();