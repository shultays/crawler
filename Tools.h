#pragma once

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#define WINDOWS
#else
#define LINUX
#endif

#include<unordered_map>
#include<string.h>
#include<stack>
#include<vector>
#include<queue>

#ifdef LINUX
#include <unistd.h>
#endif
#ifdef WINDOWS
#include <windows.h>
#endif
#undef MOUSE_MOVED
#include<curses.h>

void msleep(int sleepMS) {
#ifdef LINUX
	usleep(sleepMs * 1000);
#endif
#ifdef WINDOWS
	Sleep(sleepMS);
#endif
}

using namespace std;

#define MAZE_W 75
#define MAZE_H 75

#define UP 0
#define LEFT 1
#define DOWN 2
#define RIGHT 3

#define UP_LEFT 4
#define UP_RIGHT 5
#define DOWN_RIGHT 6
#define DOWN_LEFT 7

#define MAX_MESSAGE 100

unordered_map<int, float> moveWeights;
unordered_map<int, float> addWeights;
unordered_map<int, float> seeWeights;

#define B_UP 1
#define B_LEFT 2
#define B_DOWN 4
#define B_RIGHT 8

#define DIAG_MUL 1.41421356237f

int ran(int n) {
	return rand() % n;
}

#define ADVENTURER 11
#define ADVENTURER_ALLY 12

#define GOBLIN 101

float ranf(float a, float b) {
	return a + (b - a)*(ran(10000) / 10000.0f);
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
}

class Pos {
public:
	int x, y;
	Pos() {
		x = y = -1;
	}
	Pos(int x, int y) {
		this->x = x;
		this->y = y;
	}


	bool operator == (const Pos& other) const {
		return (x == other.x && y == other.y);
	}
	bool operator != (const Pos& other) const {
		return (x != other.x || y != other.y);
	}
};


class Pixel {
public:
	int character;
	int color;
};


extern int messagePos;
extern char messages[MAX_MESSAGE][128];

class Node {
public:
	Pos pos;
	int dir;
	float path;


	Node(int x, int y) {
		pos.x = x;
		pos.y = y;
		dir = 0;
		path = 0;
	}
	Node(int x, int y, int dir) {
		pos.x = x;
		pos.y = y;
		this->dir = dir;
		this->path = 1.0f;
	}

};

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
class CompareNode {
public:
	CompareNode(const bool& revparam = true) {}
	bool operator() (const Node& lhs, const Node& rhs) const {
		return (lhs.path > rhs.path);
	}
};

template<int W, int H>
bool doSearchExplore(Pos startPos, int arr[W][H], unordered_map<int, float> weights, float max_weight, int targetArr[W][H], int targetVal, Pos&endPos, vector<Pos> &path, vector<Pos>& exploredPos, bool ignoreDiag = false) {
	int explored[W][H];
	exploredPos.clear();
	path.clear();
	std::priority_queue<Node, std::vector<Node>, CompareNode> queue;
	memset(explored, -1, sizeof(explored));

	queue.emplace(Node(startPos.x, startPos.y));
	exploredPos.push_back(startPos);
	bool found = false;
	while (queue.size()) {
		Node n = queue.top();
		queue.pop();
		if (targetVal >= 0 && targetArr[n.pos.x][n.pos.y] == targetVal) {
			endPos = n.pos;
			found = true;
			break;
		}

		for (int i = 0; i < (ignoreDiag ? 4 : 8); i++) {
			Pos nPos = getPosForDir(n.pos, i);
			if (nPos.x < 0 || nPos.y < 0 || nPos.x >= W || nPos.y >= H) continue;
			if (explored[nPos.x][nPos.y] == -1) {
				float weight = weights[arr[nPos.x][nPos.y]];
				if (i >= 4) {
					weight *= DIAG_MUL;
				}
				if (weight >= 0 && n.path + weight <= max_weight) {
					explored[nPos.x][nPos.y] = i;
					Node newNode = Node(nPos.x, nPos.y);
					newNode.path = n.path + weight;
					queue.emplace(newNode);
				}
				exploredPos.push_back(nPos);
			}
		}
	}
	if (found) {
		vector<Pos> revPath;
		Pos pos = endPos;
		while (pos.x != startPos.x || pos.y != startPos.y) {
			revPath.push_back(pos);
			int reverseDir = getReverseDir(explored[pos.x][pos.y]);
			pos = getPosForDir(pos, reverseDir);
		}

		for (int i = revPath.size() - 1; i >= 0; i--) {
			path.push_back(revPath[i]);
			if (i == 0) {
				int a = 5;
			}
		}
	}
	return found;
}

template<int W, int H>
bool doSearch(Pos startPos, int arr[W][H], unordered_map<int, float> weights, float max_weight, int targetArr[W][H], int targetVal, Pos &endPos, vector<Pos>& path, bool ignoreDiag = false) {
	vector<Pos> exploredPos;
	return doSearchExplore<W, H>(startPos, arr, weights, max_weight, targetArr, targetVal, endPos, path, exploredPos, ignoreDiag);
}

template<int W, int H>
bool findPath(Pos startPos, Pos endPos, int arr[W][H], unordered_map<int, float> weights, float max_weight, vector<Pos>& path, bool ignoreDiag = false) {
	if (startPos == endPos) {
		path.clear();
		path.push_back(endPos);
		return true;
	}

	vector<Pos> exploredPos;
	int oldVal = arr[endPos.x][endPos.y];
	arr[endPos.x][endPos.y] = 0;
	int targetArr[W][H];
	memset(targetArr, 0, sizeof(targetArr));
	targetArr[endPos.x][endPos.y] = 1;
	bool ret = doSearchExplore<W, H>(startPos, arr, weights, max_weight, targetArr, 1, endPos, path, exploredPos, ignoreDiag);

	arr[endPos.x][endPos.y] = oldVal;
	return ret;
}

template<int W, int H>
void doExplore(Pos startPos, int arr[W][H], unordered_map<int, float> weights, float max_weight, vector<Pos>& exploredPos, bool ignoreDiag = false) {
	Pos endPos;
	vector<Pos> path;
	int targetArr[W][H];
	doSearchExplore<W, H>(startPos, arr, weights, max_weight, targetArr, -1, endPos, path, exploredPos, ignoreDiag);
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
	messagePos++;
	if (messagePos >= MAX_MESSAGE) {
		messagePos -= MAX_MESSAGE;
	}
	strcpy_s(messages[messagePos], str);
}