#include "Tools.h"
#include "Maze.h"
#include "Skill.h"

char buff[512];

unordered_map<int, float> moveWeights;
unordered_map<int, float> addWeights;
unordered_map<int, float> seeWeights;

char buffNamers[BUFF_CNT][32];
char firstBuffNamers[BUFF_CNT][32];
int eqipmentIcons[EQ_MAX];
char eqipmentNames[EQ_MAX][32];

extern Maze maze;

int ran(int n) {
	if (n <= 1) return 0;
	return rand() % n;
}
float ranf(float a, float b) {
	return a + (b - a)*(ran(10000) / 10000.0f);
}

int rani(int a, int b) {
	return a + ran(b - a + 1);
}

void initTools() {
	seeWeights.clear();
	seeWeights[0] = 1.0f;
	seeWeights[1] = -1.0f;
	seeWeights[2] = -1.0f;

	seeWeights[ADVENTURER] = 0.0f;
	seeWeights[ADVENTURER_ALLY] = 0.0f;
	seeWeights[GOBLIN] = 0.0f;

	addWeights.clear();
	addWeights[0] = 1.0f;
	addWeights[1] = -1.0f;
	addWeights[2] = -1.0f;

	addWeights[ADVENTURER] = 1.0f;
	addWeights[ADVENTURER_ALLY] = 1.0f;
	addWeights[GOBLIN] = 1.0f;

	moveWeights.clear();
	moveWeights[0] = 1.0f;
	moveWeights[1] = -1.0f;
	moveWeights[2] = -1.0f;


	moveWeights[ADVENTURER] = -1.0f;
	moveWeights[ADVENTURER_ALLY] = -1.0f;
	moveWeights[GOBLIN] = -1.0f;
	eqipmentIcons[WEAPON] = ACS_DARROW;
	eqipmentIcons[ARMOR] = ACS_DIAMOND;
	eqipmentIcons[SHIELD] = 248;
	eqipmentIcons[HELM] = ACS_PI;
	eqipmentIcons[GLOVES] = 171;
	eqipmentIcons[BOOTS] = ACS_LLCORNER;
	eqipmentIcons[RING] = 240;
	eqipmentIcons[AMULET] = 164;
	eqipmentIcons[CONSUMABLE] = ';';

	strcpy_s(eqipmentNames[WEAPON], "WPN");
	strcpy_s(eqipmentNames[ARMOR], "ARMR");
	strcpy_s(eqipmentNames[SHIELD], "SHLD");
	strcpy_s(eqipmentNames[HELM], "HELM");
	strcpy_s(eqipmentNames[GLOVES], "GLVS");
	strcpy_s(eqipmentNames[BOOTS], "GRVS");
	strcpy_s(eqipmentNames[RING], "RING");
	strcpy_s(eqipmentNames[AMULET], "AMUL");
	strcpy_s(eqipmentNames[CONSUMABLE], "CONS");

	for (int i = 0; i < BUFF_CNT; i++) {
		strcpy_s(buffNamers[i], "%s");
		strcpy_s(firstBuffNamers[i], "%s");
	}

	strcpy_s(buffNamers[ATK_SPEED], "%s of Speed");
	strcpy_s(firstBuffNamers[ATK_SPEED], "Windfury %s");

	strcpy_s(buffNamers[MV_SPEED], "%s of Speed");
	strcpy_s(firstBuffNamers[MV_SPEED], "Journeyman's %s");

	strcpy_s(buffNamers[DR_ADD], "%s of Resistance");
	strcpy_s(firstBuffNamers[DR_ADD], "Goldskin %s");

	strcpy_s(buffNamers[ATK_DMG], "%s of Strength");
	strcpy_s(firstBuffNamers[ATK_DMG], "Giant's %s");

	strcpy_s(buffNamers[ATK_MUL], "%s of Strength");
	strcpy_s(firstBuffNamers[ATK_MUL], "Giant's %s");

	strcpy_s(buffNamers[SIGHT], "%s of Light");
	strcpy_s(firstBuffNamers[SIGHT], "Hawk Eye's %s");

	strcpy_s(buffNamers[POISONED], "%s of Poison");
	strcpy_s(firstBuffNamers[POISONED], "Venomous %s");

	strcpy_s(buffNamers[FIRE_DAMAGE], "%s of Fire");
	strcpy_s(firstBuffNamers[FIRE_DAMAGE], "Ember %s");

	strcpy_s(buffNamers[LIGHTING_DAMAGE], "%s of Lighting");
	strcpy_s(firstBuffNamers[LIGHTING_DAMAGE], "Thunder %s");

	strcpy_s(buffNamers[ICE_DAMAGE], "%s of Ice");
	strcpy_s(firstBuffNamers[ICE_DAMAGE], "Freezing %s");


	strcpy_s(buffNamers[EVASION], "%s of Evasion");
	strcpy_s(firstBuffNamers[EVASION], "Blurred %s");

	strcpy_s(buffNamers[BLOCK], "%s of Block");
	strcpy_s(firstBuffNamers[BLOCK], "Grand %s");

	strcpy_s(buffNamers[HP], "%s of Health");
	strcpy_s(firstBuffNamers[HP], "Bear's %s");

	strcpy_s(buffNamers[MP], "%s of Mana");
	strcpy_s(firstBuffNamers[MP], "Archmages's %s");

	strcpy_s(buffNamers[HP_REGEN], "%s of Healing");
	strcpy_s(firstBuffNamers[HP_REGEN], "Regenerative %s");

	strcpy_s(buffNamers[MP_REGEN], "%s of Spirit");
	strcpy_s(firstBuffNamers[MP_REGEN], "Cerimonial %s");

};

int dist(Pos& p1, Pos& p2) {
	return abs(p1.x - p2.x) + abs(p1.y - p2.y);
}

int getReverseDir(int dir) {
	if (dir < 4) {
		return (dir + 2) % 4;
	} else {
		int t = dir - 4;
		t = (dir + 2) % 4;
		return t + 4;
	}
}

int getSideDir(int dir, int side) {
	if (dir < 4) {
		int t = dir + side;
		if (dir < 0) dir += 4;
		return dir % 4;
	} else {
		int t = dir + side - 4;
		if (dir < 0) dir += 4;
		return (dir % 4) + 4;
	}
}

Pos getPosForDir(Pos i, int dir) {
	switch (dir) {
		case UP:
			i.x--;
			break;
		case LEFT:
			i.y--;
			break;
		case DOWN:
			i.x++;
			break;
		case RIGHT:
			i.y++;
			break;

		case UP_LEFT:
			i.x--;
			i.y--;
			break;
		case UP_RIGHT:
			i.x--;
			i.y++;
			break;
		case DOWN_LEFT:
			i.x++;
			i.y--;
			break;
		case DOWN_RIGHT:
			i.x++;
			i.y++;
			break;
	}
	return i;
}

int getColorIndex(int r, int g, int b) {
	return (b >> 1) | (g << 2) | (r << 5);
}

void pushMessage(char *str) {
	while (str) {

		int i = -1;
		int len = strlen(str);
		if (len > maze.window->windowSize.y - 3) {
			i = maze.window->windowSize.y - 3;
			while (str[i] != ' ' && str[i] != 0 && i >= 0)
				i--;
		}
		if (i == 0) break;
		messagePos++;
		if (messagePos >= MAX_MESSAGE) {
			messagePos -= MAX_MESSAGE;
		}
		if (i == -1)
			strcpy_s(messages[messagePos], str);
		else
			strncpy_s(messages[messagePos], str, i);

		if (i == -1) break;
		else str = str + i + 1;
	}
}

void msleep(int sleepMS) {
#ifdef LINUX
	usleep(sleepMs * 1000);
#endif
#ifdef WINDOWS
	Sleep(sleepMS);
#endif
}



unsigned mtime() {
#ifdef LINUX
	return GetTickCount(); //linux
#endif
#ifdef WINDOWS
	return (unsigned)GetTickCount();
#endif
}

Buff* getBuff(vector<int>& weights, int level) {
	int totalWeight = 0;
	for (unsigned i = 0; i < weights.size(); i++) {
		totalWeight += weights[i];
	}
	int buffType = -1;
	if (totalWeight != 0) {
		int r = ran(totalWeight);

		for (unsigned i = 0; i < weights.size(); i++) {
			if (r < weights[i]) {
				buffType = i;
				break;
			}
			r -= weights[i];
		}
	} else {
		buffType = rani(1, BUFF_CNT - 1);
	}

	if (buffType <= 0) return NULL;
	Buff* buff = NULL;
	int min, max;
	switch (buffType) {
		case ATK_SPEED:
			buff = new AtkSpeedBuff(100 + ran(level / 4 + 1) * 5 + (level / 6) * 5 + 5);
			break;
		case MV_SPEED:
			buff = new MoveSpeedBuff(100 + ran(level / 4 + 1) * 5 + (level / 6) * 5 + 5);
			break;
		case DR_ADD:
			buff = new DRBuff(1 + ran(level / 2 + 2));
			break;
		case ATK_DMG:
			buff = new ConstantDMGBoost(2 + ran(level / 4 + 1) + (level / 6));
			break;
		case ATK_MUL:
			buff = new DMGBultBuff(100 + ran(level / 4 + 1) * 5 + (level / 6) * 5 + 5);
			break;
		case SIGHT:
			buff = new SightBuff(100 + ran(level / 4 + 1) * 5 + (level / 6) * 5 + 10);
			break;
		case POISONED:
			buff = new PoisonBladeBuff(2 + ran(level / 2 + 2) + level / 4);
			break;
		case FIRE_DAMAGE:
			min = 2 + ran(level / 2 + 2) + level / 4;
			max = min + 1 + ran(level / 2 + 2) + level / 4;
			buff = new PlusDamageBuff(min, max, "Fire", getColorIndex(7, 0, 0));
			break;
		case LIGHTING_DAMAGE:
			min = 2 + ran(level / 2 + 2) + level / 4;
			max = min + 1 + ran(level / 2 + 2) + level / 4;
			buff = new PlusDamageBuff(min, max, "Lighting", getColorIndex(7, 7, 0));
			break;
		case ICE_DAMAGE:
			min = 2 + ran(level / 2 + 2) + level / 4;
			max = min + 1 + ran(level / 2 + 2) + level / 4;
			buff = new PlusDamageBuff(min, max, "Ice", getColorIndex(0, 7, 7));
			break;
		case EVASION:
			buff = new EvasionBuff(100 - ran(level / 4 + 1) * 5 - (level / 6) * 5 - 5, false);
			break;
		case BLOCK:
			buff = new EvasionBuff(100 - ran(level / 4 + 1) * 5 - (level / 6) * 5 - 5, true);
			break;
		case HP:
			buff = new HpBuff(10 + ran(level / 2 + 2) * 5);
			break;
		case MP:
			buff = new MpBuff(10 + ran(level / 2 + 2) * 5);
			break;
		case HP_REGEN:
			buff = new HpRegenBuff(100 + ran(level / 4 + 1) * 5 + (level / 6) * 5 + 5);
			break;
		case MP_REGEN:
			buff = new MpRegenBuff(100 + ran(level / 4 + 1) * 5 + (level / 6) * 5 + 5);
			break;

	}
	buff->buffIndex = buffType;
	return buff;
}
