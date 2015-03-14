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
Weapon* getWeapon(int level, bool overrideNoItem = false);
Equipment* getArmor(int level, bool overrideNoArmor = false);
Equipment* getShield(int level, bool overrideNoItem = false);
Equipment* getHelm(int level, bool overrideNoItem = false);
Equipment* getBoots(int level, bool overrideNoItem = false);
Equipment* getGloves(int level, bool overrideNoItem = false);
Equipment* getRing(int level, bool overrideNoItem = false);
Equipment* getAmulet(int level, bool overrideNoItem = false);

Consumable* getConsumable(int level, int force = -1, bool overrideNoItem = false);

Creature* generateGoblin(int level, Pos pos);
Creature* generateButterfly();