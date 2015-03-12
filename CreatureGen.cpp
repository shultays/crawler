#include "CreatureGen.h"
#include "Creature.h"

void addBuff(Equipment* e, vector<int>& weights, int level, int maxBuffCount, int noBuffChance, bool addName) {
	int firstAddedI = -1;
	int secondAddedI = -1;
	for (int i = 0; i < maxBuffCount; i++) {
		if (ran(100) < noBuffChance) continue;
		Buff* buff = getBuff(weights, level - i);
		if (buff) {
			e->addBuff(buff);
			weights[buff->buffIndex] = 0;

			if (firstAddedI == -1) {
				firstAddedI = buff->buffIndex;
			} else if (secondAddedI == -1) {
				secondAddedI = buff->buffIndex;
			}
		}
	}

	if (addName) {
		int post = firstAddedI;
		int pre = -1;
		if (secondAddedI != -1) {
			pre = post;
			post = secondAddedI;
		}
		if (pre == -1 && post != -1 && ran(10 - level) == 0) {
			pre = post;
			post = -1;
		}
		if (post != -1) {
			char buff[256];
			sprintf_s(buff, buffNamers[post], e->name);
			strcpy_s(e->name, buff);
		}
		if (pre != -1) {
			char buff[256];
			sprintf_s(buff, firstBuffNamers[pre], e->name);
			strcpy_s(e->name, buff);
		}
	}
}

Weapon* getWeapon(int level) {
	bool no_weapon = level < 4 ? ran(4 - level) != 0 : 0;
	if (level == -1) {
		no_weapon = true;
	}

	int rangeMin = 0;
	int rangeMax = 4;

	if (no_weapon) {
		return NULL;
	} else {
		rangeMin = 1;
		rangeMax = 4;
	}
	Weapon *w = new Weapon();
	w->slot = WEAPON;
	w->type = rangeMin + ran(rangeMax - rangeMin + 1);
	switch (w->type) {
		case 0:
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

	vector<int> weights(BUFF_CNT);
	int noBuffChance = 60 - level * 10;
	if (noBuffChance < 10) noBuffChance = 10;

	weights[NO_BUFF] = 6;


	weights[POISONED] = 10;
	weights[FIRE_DAMAGE] = 10;
	weights[ICE_DAMAGE] = 10;
	weights[LIGHTING_DAMAGE] = 10;

	weights[ATK_SPEED] = 3;
	weights[MV_SPEED] = 1;
	weights[DR_ADD] = 0;
	weights[ATK_DMG] = 1;
	weights[ATK_MUL] = 1;
	weights[SIGHT] = 1;

	weights[HP] = 1;
	weights[MP] = 1;
	weights[HP_REGEN] = 0;
	weights[MP_REGEN] = 0;

	weights[EVASION] = 0;
	weights[BLOCK] = 1;


	int maxBuffCount = ran(level / 3 + 1) + 1;
	if (maxBuffCount > 3) maxBuffCount = 3;

	addBuff(w, weights, level - 1, maxBuffCount, noBuffChance);

	return w;
}

Equipment* getArmor(int level, bool overrideNoArmor) {
	int rangeMin = 0;
	int rangeMax = 4;

	bool no_armor;
	if (level < 2) {
		no_armor = true;
	} else if (level <= 5) {
		no_armor = ran(5 - level + 1) != 0;
	} else {
		no_armor = false;
	}
	no_armor &= !overrideNoArmor;
	if (no_armor) {
		return NULL;
	} else {
		rangeMin = 1;
		rangeMax = (int)((level - 2)*0.5f) + 1;
		if (rangeMax<1) rangeMax = 1;
		if (rangeMax>5) rangeMax = 5;
	}
	Equipment *e = new Equipment();
	e->slot = ARMOR;
	e->type = rangeMin + ran(rangeMax - rangeMin + 1);
	int DR = 0;
	int slow = 100;
	switch (e->type) {
		case 0:
			break;
		case 1:
			DR = 0 + ran(2 + (level - 2) / 6);
			slow = 100;
			strcpy_s(e->name, "Cloth Armor");
			break;
		case 2:
			DR = 2 + ran(2 + (level - 2) / 4);
			slow = 90;
			strcpy_s(e->name, "Leather Armor");
			break;
		case 3:
			DR = 4 + ran(4 + (level - 2) / 3);
			slow = 80;
			strcpy_s(e->name, "Mail Armor");
			break;
		case 4:
			DR = 5 + ran(6 + (int)((level - 2) / 2.5f));
			slow = 75;
			strcpy_s(e->name, "Plate Armor");
			break;
		case 5:
			DR = 6 + ran(8 + (int)(level / 2.5f));
			slow = 65;
			strcpy_s(e->name, "Full Plate Armor");
			break;
	}

	if (DR != 0) {
		e->addBuff(new DRBuff(DR));
	}
	if (slow != 100) {
		e->addBuff(new MoveSpeedBuff(slow));
	}

	vector<int> weights(BUFF_CNT);
	int noBuffChance = 110 - level * 10;
	if (noBuffChance < 30) noBuffChance = 30;

	weights[NO_BUFF] = 6;


	weights[POISONED] = 0;
	weights[FIRE_DAMAGE] = 0;
	weights[ICE_DAMAGE] = 0;
	weights[LIGHTING_DAMAGE] = 0;

	weights[ATK_SPEED] = 1;
	weights[MV_SPEED] = 0;
	weights[DR_ADD] = 0;
	weights[ATK_DMG] = 1;
	weights[ATK_MUL] = 1;
	weights[SIGHT] = 1;

	weights[HP] = 10;
	weights[MP] = 6;
	weights[HP_REGEN] = 4;
	weights[MP_REGEN] = 6;

	weights[EVASION] = 2;
	weights[BLOCK] = 0;


	int maxBuffCount = ran(level / 3 + 1) + 1;
	if (maxBuffCount > 3) maxBuffCount = 3;

	addBuff(e, weights, level, maxBuffCount, noBuffChance);

	return e;
}

Equipment* getShield(int level) {
	int rangeMin = 0;
	int rangeMax = 4;

	bool no_armor;
	if (level < 2) {
		no_armor = true;
	} else if (level <= 5) {
		no_armor = ran(5 - level + 1) != 0;
	} else {
		no_armor = false;
	}

	if (no_armor) {
		return NULL;
	} else {
		rangeMin = 1;
		rangeMax = (int)((level - 2)*0.5f) + 1;
		if (rangeMax<1) rangeMax = 1;
		if (rangeMax>5) rangeMax = 5;
	}
	Equipment *e = new Equipment();
	e->slot = SHIELD;
	e->type = rangeMin + ran(rangeMax - rangeMin + 1);
	int DR = 0;
	int block = 100;
	switch (e->type) {
		case 0:
			break;
		case 1:
			DR = 0 + ran(1 + (level - 2) / 6);
			block = 100;
			strcpy_s(e->name, "Buckler");
			break;
		case 2:
			DR = 0 + ran(1 + (level - 2) / 4);
			block = 90;
			strcpy_s(e->name, "Targe Shield");
			break;
		case 3:
			DR = 1 + ran(2 + (level - 2) / 3);
			block = 90;
			strcpy_s(e->name, "Heater Shield");
			break;
		case 4:
			DR = 2 + ran(3 + (int)((level - 2) / 2.5f));
			block = 80;
			strcpy_s(e->name, "Kite Shield");
			break;
		case 5:
			DR = 3 + ran(3 + (int)(level / 2.5f));
			block = 70;
			strcpy_s(e->name, "Tower Shield");
			break;
	}

	if (DR != 0) {
		e->addBuff(new DRBuff(DR));
	}
	if (block != 100) {
		e->addBuff(new EvasionBuff(block, true));
	}

	vector<int> weights(BUFF_CNT);
	int noBuffChance = 110 - level * 10;
	if (noBuffChance < 30) noBuffChance = 30;

	weights[NO_BUFF] = 6;


	weights[POISONED] = 0;
	weights[FIRE_DAMAGE] = 0;
	weights[ICE_DAMAGE] = 0;
	weights[LIGHTING_DAMAGE] = 0;

	weights[ATK_SPEED] = 1;
	weights[MV_SPEED] = 0;
	weights[DR_ADD] = 0;
	weights[ATK_DMG] = 1;
	weights[ATK_MUL] = 1;
	weights[SIGHT] = 0;

	weights[HP] = 10;
	weights[MP] = 6;
	weights[HP_REGEN] = 4;
	weights[MP_REGEN] = 6;

	weights[EVASION] = 0;
	weights[BLOCK] = 0;


	int maxBuffCount = ran(level / 3 + 1) + 1;
	if (maxBuffCount > 3) maxBuffCount = 3;

	addBuff(e, weights, level, maxBuffCount, noBuffChance);

	return e;
}

Equipment* getHelm(int level) {
	int rangeMin = 0;
	int rangeMax = 4;

	bool no_armor;
	if (level < 3) {
		no_armor = true;
	} else if (level <= 6) {
		no_armor = ran(5 - level + 1) != 0;
	} else {
		no_armor = false;
	}

	if (no_armor) {
		return NULL;
	} else {
		rangeMin = 1;
		rangeMax = (int)((level - 4)*0.5f) + 1;
		if (rangeMax<1) rangeMax = 1;
		if (rangeMax>4) rangeMax = 4;
	}
	Equipment *e = new Equipment();
	e->slot = HELM;
	e->type = rangeMin + ran(rangeMax - rangeMin + 1);
	int DR = 0;
	switch (e->type) {
		case 0:
			break;
		case 1:
			DR = 0 + ran(1 + (level - 2) / 6);
			strcpy_s(e->name, "Hat");
			break;
		case 2:
			DR = 1 + ran(1 + (level - 2) / 5);
			strcpy_s(e->name, "Leather Hood");
			break;
		case 3:
			DR = 1 + ran(2 + (level - 2) / 4);
			strcpy_s(e->name, "Mail Helm");
			break;
		case 4:
			DR = 2 + ran(2 + (int)((level - 2) / 3));
			strcpy_s(e->name, "Plate Helm");
			break;
	}

	if (DR != 0) {
		e->addBuff(new DRBuff(DR));
	}


	vector<int> weights(BUFF_CNT);
	int noBuffChance = 110 - level * 10;
	if (noBuffChance < 30) noBuffChance = 30;

	weights[NO_BUFF] = 6;

	weights[POISONED] = 0;
	weights[FIRE_DAMAGE] = 0;
	weights[ICE_DAMAGE] = 0;
	weights[LIGHTING_DAMAGE] = 0;

	weights[ATK_SPEED] = 1;
	weights[MV_SPEED] = 0;
	weights[DR_ADD] = 0;
	weights[ATK_DMG] = 1;
	weights[ATK_MUL] = 1;
	weights[SIGHT] = 4;

	weights[HP] = 1;
	weights[MP] = 1;
	weights[HP_REGEN] = 2;
	weights[MP_REGEN] = 2;

	weights[EVASION] = 1;
	weights[BLOCK] = 0;


	int maxBuffCount = ran(level / 3 + 1) + 1;
	if (maxBuffCount > 3) maxBuffCount = 3;

	addBuff(e, weights, level, maxBuffCount, noBuffChance);

	return e;
}

Equipment* getBoots(int level) {
	int rangeMin = 0;
	int rangeMax = 4;

	bool no_armor;
	if (level < 3) {
		no_armor = true;
	} else if (level <= 6) {
		no_armor = ran(5 - level + 1) != 0;
	} else {
		no_armor = false;
	}

	if (no_armor) {
		return NULL;
	} else {
		rangeMin = 1;
		rangeMax = (int)((level - 4)*0.5f) + 1;
		if (rangeMax<1) rangeMax = 1;
		if (rangeMax>4) rangeMax = 4;
	}
	Equipment *e = new Equipment();
	e->slot = BOOTS;
	e->type = rangeMin + ran(rangeMax - rangeMin + 1);
	int DR = 0;
	switch (e->type) {
		case 0:
			break;
		case 1:
			DR = 0 + ran(1 + (level - 2) / 6);
			strcpy_s(e->name, "Shoes");
			break;
		case 2:
			DR = 1 + ran(2 + (level - 2) / 4);
			strcpy_s(e->name, "Leather Boots");
			break;
		case 3:
			DR = 1 + ran(4 + (level - 3) / 3);
			strcpy_s(e->name, "Mail Boots");
			break;
		case 4:
			DR = 2 + ran(6 + (int)((level - 2) / 3));
			strcpy_s(e->name, "Plate Boots");
			break;
	}

	if (DR != 0) {
		e->addBuff(new DRBuff(DR));
	}

	vector<int> weights(BUFF_CNT);
	int noBuffChance = 110 - level * 10;
	if (noBuffChance < 30) noBuffChance = 30;

	weights[NO_BUFF] = 6;

	weights[POISONED] = 0;
	weights[FIRE_DAMAGE] = 0;
	weights[ICE_DAMAGE] = 0;
	weights[LIGHTING_DAMAGE] = 0;

	weights[ATK_SPEED] = 0;
	weights[MV_SPEED] = 10;
	weights[DR_ADD] = 0;
	weights[ATK_DMG] = 1;
	weights[ATK_MUL] = 1;
	weights[SIGHT] = 0;

	weights[HP] = 1;
	weights[MP] = 1;
	weights[HP_REGEN] = 1;
	weights[MP_REGEN] = 1;

	weights[EVASION] = 4;
	weights[BLOCK] = 0;


	int maxBuffCount = ran(level / 3 + 1) + 1;
	if (maxBuffCount > 3) maxBuffCount = 3;

	addBuff(e, weights, level, maxBuffCount, noBuffChance);

	return e;
}

Equipment* getGloves(int level) {
	int rangeMin = 0;
	int rangeMax = 4;

	bool no_armor;
	if (level < 3) {
		no_armor = true;
	} else if (level <= 6) {
		no_armor = ran(5 - level + 1) != 0;
	} else {
		no_armor = false;
	}

	if (no_armor) {
		return NULL;
	} else {
		rangeMin = 1;
		rangeMax = (int)((level - 4)*0.5f) + 1;
		if (rangeMax<1) rangeMax = 1;
		if (rangeMax>4) rangeMax = 4;
	}
	Equipment *e = new Equipment();
	e->slot = GLOVES;
	e->type = rangeMin + ran(rangeMax - rangeMin + 1);
	int DR = 0;
	switch (e->type) {
		case 0:
			break;
		case 1:
			DR = 0 + ran(1 + (level - 2) / 6);
			strcpy_s(e->name, "Cloth Gloves");
			break;
		case 2:
			DR = 1 + ran(1 + (level - 2) / 4);
			strcpy_s(e->name, "Leather Gloves");
			break;
		case 3:
			DR = 1 + ran(2 + (level - 2) / 3);
			strcpy_s(e->name, "Chain Gloves");
			break;
		case 4:
			DR = 2 + ran(2 + (level - 2) / 3);
			strcpy_s(e->name, "Gauntlets");
			break;
	}

	if (DR != 0) {
		e->addBuff(new DRBuff(DR));
	}

	vector<int> weights(BUFF_CNT);
	int noBuffChance = 110 - level * 10;
	if (noBuffChance < 30) noBuffChance = 30;

	weights[NO_BUFF] = 6;

	weights[POISONED] = 0;
	weights[FIRE_DAMAGE] = 2;
	weights[ICE_DAMAGE] = 2;
	weights[LIGHTING_DAMAGE] = 2;

	weights[ATK_SPEED] = 10;
	weights[MV_SPEED] = 0;
	weights[DR_ADD] = 0;
	weights[ATK_DMG] = 4;
	weights[ATK_MUL] = 4;
	weights[SIGHT] = 0;

	weights[HP] = 1;
	weights[MP] = 1;
	weights[HP_REGEN] = 1;
	weights[MP_REGEN] = 1;

	weights[EVASION] = 0;
	weights[BLOCK] = 1;


	int maxBuffCount = ran(level / 3 + 1) + 1;
	if (maxBuffCount > 3) maxBuffCount = 3;

	addBuff(e, weights, level, maxBuffCount, noBuffChance);

	return e;
}

Equipment* getRing(int level) {
	int rangeMin = 0;
	int rangeMax = 4;

	bool no_armor;
	if (level < 3) {
		no_armor = true;
	} else if (level <= 6) {
		no_armor = ran(5 - level + 1) != 0;
	} else {
		no_armor = false;
	}

	if (no_armor) {
		return NULL;
	} else {
		rangeMin = 1;
		rangeMax = 8;
	}
	Equipment *e = new Equipment();
	e->slot = RING;
	e->type = rangeMin + ran(rangeMax - rangeMin + 1);
	switch (e->type) {
		case 0:
			break;
		case 1:
			strcpy_s(e->name, "Azure Ring");
			break;
		case 2:
			strcpy_s(e->name, "Saphire Ring");
			break;
		case 3:
			strcpy_s(e->name, "Diamond Ring");
			break;
		case 4:
			strcpy_s(e->name, "Ruby Ring");
			break;
		case 5:
			strcpy_s(e->name, "Opal Ring");
			break;
		case 6:
			strcpy_s(e->name, "Emerald Ring");
			break;
		case 7:
			strcpy_s(e->name, "Amber Ring");
			break;
		case 8:
			strcpy_s(e->name, "Bone Ring");
			break;
	}

	vector<int> weights(BUFF_CNT);
	int noBuffChance = 110 - level * 10;
	if (noBuffChance < 30) noBuffChance = 30;

	weights[NO_BUFF] = 1;

	weights[POISONED] = 0;
	weights[FIRE_DAMAGE] = 0;
	weights[ICE_DAMAGE] = 0;
	weights[LIGHTING_DAMAGE] = 0;

	weights[ATK_SPEED] = 1;
	weights[MV_SPEED] = 1;
	weights[DR_ADD] = 1;
	weights[ATK_DMG] = 1;
	weights[ATK_MUL] = 1;
	weights[SIGHT] = 1;

	weights[HP] = 1;
	weights[MP] = 1;
	weights[HP_REGEN] = 1;
	weights[MP_REGEN] = 1;

	weights[EVASION] = 1;
	weights[BLOCK] = 0;


	int maxBuffCount = ran(level / 3 + 1) + 1;
	if (maxBuffCount > 3) maxBuffCount = 3;

	addBuff(e, weights, level, maxBuffCount, noBuffChance);

	return e;
}

Equipment* getAmulet(int level) {
	int rangeMin = 0;
	int rangeMax = 4;

	bool no_armor;
	if (level < 3) {
		no_armor = true;
	} else if (level <= 6) {
		no_armor = ran(5 - level + 1) != 0;
	} else {
		no_armor = false;
	}

	if (no_armor) {
		return NULL;
	} else {
		rangeMin = 1;
		rangeMax = 8;
	}
	Equipment *e = new Equipment();
	e->slot = AMULET;
	e->type = rangeMin + ran(rangeMax - rangeMin + 1);
	switch (e->type) {
		case 0:
			break;
		case 1:
			strcpy_s(e->name, "Azure Amulet");
			break;
		case 2:
			strcpy_s(e->name, "Saphire Amulet");
			break;
		case 3:
			strcpy_s(e->name, "Diamond Amulet");
			break;
		case 4:
			strcpy_s(e->name, "Ruby Amulet");
			break;
		case 5:
			strcpy_s(e->name, "Opal Amulet");
			break;
		case 6:
			strcpy_s(e->name, "Emerald Amulet");
			break;
		case 7:
			strcpy_s(e->name, "Amber Amulet");
			break;
		case 8:
			strcpy_s(e->name, "Bone Amulet");
			break;
	}


	vector<int> weights(BUFF_CNT);
	int noBuffChance = 110 - level * 10;
	if (noBuffChance < 30) noBuffChance = 30;

	weights[NO_BUFF] = 1;

	weights[POISONED] = 0;
	weights[FIRE_DAMAGE] = 0;
	weights[ICE_DAMAGE] = 0;
	weights[LIGHTING_DAMAGE] = 0;

	weights[ATK_SPEED] = 1;
	weights[MV_SPEED] = 1;
	weights[DR_ADD] = 1;
	weights[ATK_DMG] = 1;
	weights[ATK_MUL] = 1;
	weights[SIGHT] = 1;

	weights[HP] = 1;
	weights[MP] = 1;
	weights[HP_REGEN] = 1;
	weights[MP_REGEN] = 1;

	weights[EVASION] = 1;
	weights[BLOCK] = 0;


	int maxBuffCount = ran(level / 3 + 1) + 1;
	if (maxBuffCount > 3) maxBuffCount = 3;

	addBuff(e, weights, level, maxBuffCount, noBuffChance);

	return e;
}

Consumable* getConsumable(int level, int force) {
	int rangeMin = 0;
	int rangeMax = 4;

	bool no_item;
	

	no_item = ran(level + 2) != 0;
	


	if (force != -1) no_item = false;

	if (no_item) {
		return NULL;
	} else {
		if (force != -1) {
			rangeMin = rangeMax = force;
		} else {
			rangeMin = HEAL_POT;
			rangeMax = CONS_MAX - 1;
		}
	}

	Consumable *e = new Consumable();
	e->slot = CONSUMABLE;
	e->type = rangeMin + ran(rangeMax - rangeMin + 1);
	e->count = 1;
	switch (e->type) {
		case 0:
			break;
		case HEAL_POT:
			strcpy_s(e->name, "Healing Potion");
			e->skill = new HealSkill(40, 80, 25, false);
			e->consumeType = POTION;
			break;
		case MANA_POT:
			strcpy_s(e->name, "Mana Potion");
			e->skill = new ManaSkill();
			e->consumeType = POTION;
			break;
		case REGEN_POT:
			strcpy_s(e->name, "Regenation Potion");
			e->skill = new RegenSkill();
			e->consumeType = POTION;
			break;
		case ENCH_WEAPON:
			strcpy_s(e->name, "Ench. Weapon Scroll");
			e->skill = new EnchantSkill(true);
			e->consumeType = SCROLL;
			break;
		case ENCH_ARMOR:
			strcpy_s(e->name, "Ench. Armor Scroll");
			e->skill = new EnchantSkill(false);
			e->count = rani(2, 5);
			e->consumeType = SCROLL;
			break;
		case LIGHTING_SCROLL:
			strcpy_s(e->name, "Book of Clouds");
			e->skill = new LightingSkill();
			e->count = rani(2, 5);
			e->consumeType = BOOK;
			break;
		case FIRE_SCROLL:
			strcpy_s(e->name, "Charred Book");
			e->skill = new FireBallSkill();
			e->count = rani(2, 5);
			e->consumeType = BOOK;
			break;
		case ICE_SCROLL:
			strcpy_s(e->name, "Book of Blizzard");
			e->skill = new IceBoltSkill();
			e->consumeType = BOOK;
			break;
		case CHARM_SCROLL:
			strcpy_s(e->name, "Scroll of Love");
			e->skill = new CharmSkill();
			e->consumeType = SCROLL;
			break;
		case BUTTERFLY_SCROLL:
			strcpy_s(e->name, "Scroll of Butterflies");
			e->skill = new SummonSkill(1);
			e->consumeType = SCROLL;
			break;
	}

	return e;
}

Creature* generateGoblin(int level, Pos pos) {
	Creature* c = new Creature();
	strcpy_s(c->name, "Goblin");
	c->level = level;
	c->hpMax = 20 + level * 10 + ran(4) * 5;
	c->hp = c->hpMax;
	c->mp = 0;
	c->minExp = 30 + level*10;
	c->maxExp = (int)(c->minExp*1.2f);
	c->sight = 6.0f;
	c->movePerTick = 6.0f;
	c->explores = false;
	c->wandersAround = true;
	c->type = GOBLIN;
	c->level = level;
	c->pixel.character = 'g';
	c->pixel.color = getColorIndex(0, 4, 2);
	c->tickToRegen = 100.0f;
	c->pos = pos;

	if (ran(2) == 0)c->equip(getWeapon(level));
	if (ran(3) == 0)c->equip(getArmor(level));
	if (ran(5) == 0)c->equip(getHelm(level - 1));
	if (ran(10) == 0)c->equip(getRing(level - 2));
	if (ran(5) == 0)c->equip(getShield(level - 1));
	if (ran(10) == 0)c->equip(getAmulet(level - 2));
	if (ran(6) == 0)c->equip(getGloves(level - 1));
	if (ran(6) == 0)c->equip(getBoots(level - 1));
	return c;
}


Creature* generateButterfly() {
	Creature* c = new Creature();
	strcpy_s(c->name, "Butterfly");
	c->level = 1;
	c->hpMax = 30;
	c->hp = c->hpMax;
	c->mp = 0;
	c->minExp = 0;
	c->maxExp = 0;
	c->sight = 6.0f;
	c->movePerTick = 2.0f;
	c->explores = false;
	c->wandersAround = true;
	c->type = ADVENTURER_ALLY;
	c->level = 1;
	c->pixel.character = 'b';
	c->chanceToHit = 300;
	c->cantAttack = true;
	int r = 0, g = 0, b = 0;
	int t = ran(3);
	if (t == 0) {
		r = 7;
		g = ran(8);
	} else if (t == 2) {
		g = 7;
		b = ran(8);
	} else {
		b = 7;
		r = ran(8);
	}
	c->pixel.color = getColorIndex(r, g, b);
	c->tickToRegen = 100000.0f;
	return c;
}