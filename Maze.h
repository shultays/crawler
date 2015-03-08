#pragma once
#include "GameWindow.h"

class Maze {
public:

	int changeDirChance;
	int threeWayPathChance;
	int fourWayPathChance;
	int stopChance;


	char arr[MAZE_W / 3][MAZE_H / 3];

	int room[MAZE_W][MAZE_H];
	int walls[MAZE_W][MAZE_H];

	int roomCount;
	vector<Pos> roomPos;

	GameWindow<MAZE_W, MAZE_H> *window;

	Maze() {

		changeDirChance = 10; //1
		threeWayPathChance = 3; //2
		fourWayPathChance = 5; //3
		stopChance = 2;  //4
	}

	int getRandomAction(int noActionChance, float changeDirMult, float threeWayMult, float fourWayMult, float stopMult) {
		int modifiedChangeDirChance = (int)(changeDirChance*changeDirMult);
		int modifiedThreeWayPathChance = (int)(threeWayPathChance*threeWayMult);
		int modofiedFourWayPathChance = (int)(fourWayMult*fourWayPathChance);
		int modifiedStopChance = (int)(stopChance*stopMult);

		int totalChance = noActionChance + modifiedChangeDirChance + modifiedThreeWayPathChance + modofiedFourWayPathChance + modifiedStopChance;
		if (totalChance == 0) {
			return 4;
		}
		int c = ran(totalChance);

		if (c < noActionChance) {
			return 0;
		}
		c -= noActionChance;


		if (c < modifiedChangeDirChance) {
			return 1;
		}
		c -= modifiedChangeDirChance;
		if (c < modifiedThreeWayPathChance) {
			return 2;
		}
		c -= modifiedThreeWayPathChance;
		if (c < modofiedFourWayPathChance) {
			return 3;
		}
		c -= modofiedFourWayPathChance;
		if (c < modifiedStopChance) {
			return 4;
		}
		c -= modifiedStopChance;

		return 4;
	}

	int isPosValid3(Pos pos) {
		if (pos.x < 0) return 0;
		if (pos.y < 0) return 0;
		if (pos.x >= MAZE_W) return 0;
		if (pos.y >= 3 * MAZE_H / 3) return 0;
		return 1;
	}

	int isPosValid(Pos pos) {
		if (pos.x < 0) return 0;
		if (pos.y < 0) return 0;
		if (pos.x >= MAZE_W / 3) return 0;
		if (pos.y >= MAZE_H / 3) return 0;
		return 1;
	}

	int isNodeEmpty(Pos pos) {
		if (!isPosValid(pos)) return 0;
		return arr[pos.x][pos.y] == 0;
	}
	void write() {

		FILE *f;

		fopen_s(&f, "out3.txt", "w");


		for (int i = 0; i < MAZE_W / 3; i++) {
			for (int k = 0; k < 3; k++) {
				for (int j = 0; j < MAZE_H / 3; j++) {
					fprintf(f, "%c%c%c", walls[i * 3 + k][j * 3], walls[i * 3 + k][j * 3 + 1], walls[i * 3 + k][j * 3 + 2]);
				}
				fprintf(f, "\n");
			}

		}


		fclose(f);

		FILE *f2;
		fopen_s(&f2, "out2.txt", "wb");
		int dat[] = {
			0x20,
			0x8294e2,
			0x8094e2,
			0x9894e2,
			0x8294e2,
			0x8294e2,
			0x9094e2,
			0xa494e2,
			0x8094e2,
			0x9494e2,
			0x8094e2,
			0xb494e2,
			0x8c94e2,
			0x9c94e2,
			0xac94e2,
			0xbc94e2,
		};

		int t = 0xbfbbef;

		fwrite(&t, 3, 1, f2);
		for (int i = 0; i < MAZE_W / 3; i++) {
			for (int j = 0; j < MAZE_H / 3; j++) {

				fwrite(&dat[arr[i][j]], 1, arr[i][j] == 0 ? 1 : 3, f2);



			}
			int t = 0xa0d;
			fwrite(&t, 1, 2, f2);
		}
		fclose(f2);
	}

	void genRand(int n) {
		memset(arr, 0, sizeof(arr));
		memset(room, 0, sizeof(room));
		memset(walls, 0, sizeof(walls));
		roomPos.clear();
		genRooms();
		genPaths(n);
		genWalls();
		connectRooms();
		fixSides();
		checkUnreached();
	}

	void fixSides() {
		printw("fixSides..."); refresh();
		for (int i = 0; i < MAZE_W; i++) {
			walls[i][0] = 1;
			walls[i][MAZE_H - 1] = 1;
		}

		for (int j = 0; j < MAZE_H; j++) {
			walls[0][j] = 1;
			walls[MAZE_W - 1][j] = 1;
		}

		printw("  done\n"); refresh();
	}
	void checkUnreached() {
		printw("checkUnreached..."); refresh();
		queue<Pos> queue;
		bool reached[MAZE_W][MAZE_H];
		memset(reached, false, sizeof(reached));
		queue.emplace(Pos(roomPos[0].x, roomPos[0].y));
		while (queue.size() > 0) {
			Pos pos = queue.front();
			queue.pop();
			reached[pos.x][pos.y] = 1;

			if (walls[pos.x][pos.y] == 0) {
				for (int i = 0; i < 4; i++) {
					Pos nPos = getPosForDir(pos, i);
					if (isPosValid3(nPos) && reached[nPos.x][nPos.y] == 0) {
						reached[nPos.x][nPos.y] = 1;
						queue.emplace(nPos);
					}
				}
			}
		}

		for (int i = 0; i < MAZE_W; i++) {
			for (int j = 0; j < MAZE_H; j++) {
				if (reached[i][j] == false && walls[i][j] == 0) {
					walls[i][j] = 1;
				}
			}
		}

		memset(reached, 0, sizeof(reached));

		for (int i = 0; i < MAZE_W; i++) {
			for (int j = 0; j < MAZE_H; j++) {
				for (int a = -1; a <= 1; a++) {
					for (int b = -1; b <= 1; b++) {
						if (isPosValid3(Pos(i + a, j + b)) && walls[i + a][j + b] == 0) {
							reached[i][j] = true;
						}
					}
				}
			}
		}


		for (int i = 0; i < MAZE_W; i++) {
			for (int j = 0; j < MAZE_H; j++) {
				if (reached[i][j] == false && walls[i][j] == 1) {
					walls[i][j] = 2;
				}
			}
		}
		printw(" done\n"); refresh();
	}

	class CompareNode {
		bool reverse;
	public:
		CompareNode(const bool& revparam = true) {
			reverse = revparam;
		}
		bool operator() (const Node& lhs, const Node& rhs) const {
			if (reverse) return (lhs.path > rhs.path);
			else return (lhs.path < rhs.path);
		}
	};

	void connectTwoRooms(int i, int j) {
		printw("\tconnecting %d %d ...", i, j); refresh();
		Pos startPos = Pos(roomPos[i].x, roomPos[i].y);

		Pos foundPos;
		unordered_map<int, float> weights;
		weights[0] = 1.0f;
		weights[1] = 20.0f;
		weights[2] = 20.0f;
		vector<Pos> path;
		doSearch<MAZE_W, MAZE_H>(startPos, walls, weights, 10000000, room, j, foundPos, path, true);

		for (unsigned i = 0; i < path.size(); i++) {
			Pos pos = path[i];
			walls[foundPos.x][foundPos.y] = 0;
			walls[pos.x][pos.y] = 0;
		}

		printw(" done\n"); refresh();
	}


	void connectRooms() {
		printw("connectRooms...\n"); refresh();
		for (int b = roomCount - 1; b > 0; b--) {
			int a = ran(b);

			connectTwoRooms(a, b);
		}
		/*
		for (int i = 0; i < roomCount * 4; i++) {
		int a = ran(roomCount);
		int b;
		do {
		b = ran(roomCount);
		} while (b == a);
		connectTwoRooms(a, b);
		}*/
		printw(" done\n"); refresh();
	}

	void genRooms() {
		printw("genRooms..."); refresh();
		roomCount = 0;
		for (int a = 0; a < MAZE_W / 3 * MAZE_H / 3 / 20 + 2; a++) {
			int w = 3 + ran(5);
			int h = 3 + ran(5);
			int x = ran(MAZE_W / 3 - w);
			int y = ran(MAZE_H / 3 - h);

			int canAdd = 1;

			for (int i = x; i < x + w; i++) {
				for (int j = y; j < y + h; j++) {
					if (arr[i][j] != 0) canAdd = 0;
				}
			}

			if (canAdd) {
				roomCount++;
				roomPos.push_back(Pos(x * 3 + w * 3 / 2, y * 3 + h * 3 / 2));
				for (int i = x; i < x + w; i++) {
					for (int j = y; j < y + h; j++) {
						arr[i][j] = 0x10;
						if (j == y)
							arr[i][j] |= B_UP;
						if (i == x)
							arr[i][j] |= B_LEFT;
						if (j == y + h - 1)
							arr[i][j] |= B_DOWN;
						if (i == x + w - 1)
							arr[i][j] |= B_RIGHT;
						if (arr[i][j] == 0x10) {
							for (int a = 0; a < 3; a++) {
								for (int b = 0; b < 3; b++) {
									room[i * 3 + a][j * 3 + b] = roomCount;
								}
							}
						}
					}
				}
			}
		}
		printw(" done\n"); refresh();
	}

	void genWalls() {
		printw("genWalls..."); refresh();
		for (int i = 0; i < MAZE_W / 3; i++) {
			for (int j = 0; j < MAZE_H / 3; j++) {

				int t = arr[i][j];

				if (t == -1) {

					walls[i * 3][j * 3] = 1;
					walls[i * 3 + 2][j * 3] = 1;
					walls[i * 3 + 2][j * 3 + 2] = 1;
					walls[i * 3][j * 3 + 2] = 1;


					walls[i * 3 + 1][j * 3] = 1;
					walls[i * 3][j * 3 + 1] = 1;
					walls[i * 3 + 1][j * 3 + 2] = 1;
					walls[i * 3 + 2][j * 3 + 1] = 1;

					walls[i * 3 + 1][j * 3 + 1] = 1;
				} else if (t == 0) {


				} else if (t < 16) {
					walls[i * 3][j * 3] = 1;
					walls[i * 3 + 2][j * 3] = 1;
					walls[i * 3 + 2][j * 3 + 2] = 1;
					walls[i * 3][j * 3 + 2] = 1;


					walls[i * 3][j * 3 + 1] = t & B_UP ? 0 : 1;
					walls[i * 3 + 1][j * 3] = t & B_LEFT ? 0 : 1;
					walls[i * 3 + 2][j * 3 + 1] = t & B_DOWN ? 0 : 1;
					walls[i * 3 + 1][j * 3 + 2] = t & B_RIGHT ? 0 : 1;

				} else if (t & 16) {
					t = t & 15;

					if (t == 0) {

					} else {
						walls[i * 3][j * 3] = t & (B_UP | B_LEFT) ? 1 : 0;
						walls[i * 3 + 2][j * 3] = t & (B_UP | B_RIGHT) ? 1 : 0;
						walls[i * 3 + 2][j * 3 + 2] = t & (B_DOWN | B_RIGHT) ? 1 : 0;
						walls[i * 3][j * 3 + 2] = t & (B_DOWN | B_LEFT) ? 1 : 0;


						walls[i * 3 + 1][j * 3] = t & B_UP ? 1 : 0;
						walls[i * 3][j * 3 + 1] = t & B_LEFT ? 1 : 0;
						walls[i * 3 + 1][j * 3 + 2] = t & B_DOWN ? 1 : 0;
						walls[i * 3 + 2][j * 3 + 1] = t & B_RIGHT ? 1 : 0;

					}
				}
			}
		}


		for (int i = 0; i < MAZE_W / 3; i++) {
			for (int j = 0; j < MAZE_H / 3; j++) {

				int t = arr[i][j];

				if (t == -1) {
				} else if (t == 0) {
				} else if (t < 16) {
				} else if (t & 16) {
					t = t & 15;

					if (t == 0) {

					} else {
						Pos p;
						p.x = i * 3 + 1;
						p.y = j * 3 + 1;
						if (t == B_LEFT && ran(5) == 0) {
							Pos nPos = p;
							nPos.x -= 3;
							if (isPosValid3(nPos) && walls[nPos.x][nPos.y] == 0) {
								walls[p.x - 1][p.y] = 0;
								walls[p.x - 2][p.y] = 0;
							}
						}

						if (t == B_UP && ran(5) == 0) {
							Pos nPos = p;
							nPos.y -= 3;
							if (isPosValid3(nPos) && walls[nPos.x][nPos.y] == 0) {
								walls[p.x][p.y - 1] = 0;
								walls[p.x][p.y - 2] = 0;
							}
						}

						if (t == B_RIGHT && ran(5) == 0) {
							Pos nPos = p;
							nPos.x += 3;
							if (isPosValid3(nPos) && walls[nPos.x][nPos.y] == 0) {
								walls[p.x + 1][p.y] = 0;
								walls[p.x + 2][p.y] = 0;
							}
						}


						if (t == B_DOWN && ran(5) == 0) {
							Pos nPos = p;
							nPos.y += 3;
							if (isPosValid3(nPos) && walls[nPos.x][nPos.y] == 0) {
								walls[p.x][p.y + 1] = 0;
								walls[p.x][p.y + 2] = 0;
							}
						}
					}
				}
			}
		}
		printw(" done\n"); refresh();
	}

	void genPaths(int pathLen) {
		printw("genPaths..."); refresh();
		vector<Node> nodes;

		vector<Pos> allPos;
		for (int i = 0; i < MAZE_W / 3; i++) {
			for (int j = 0; j < MAZE_H / 3; j++) {
				Pos p;
				p.x = i;
				p.y = j;
				if (isNodeEmpty(p)) {
					allPos.push_back(p);

				}
			}
		}

		while (allPos.size() > 0) {
			Pos p = allPos[ran(allPos.size())];
			Node n = Node(p.x, p.y, ran(4));
			nodes.push_back(n);

			vector<Pos> added;
			while (nodes.size() > 0) {
				for (unsigned i = 0; i < nodes.size(); i++) {
					Node& n = nodes[i];

					for (unsigned j = 0; j < allPos.size(); j++) {
						if (allPos[j].x == n.pos.x && allPos[j].y == n.pos.y) {
							allPos[j] = allPos[allPos.size() - 1];
							allPos.pop_back();
							break;
						}
					}

					added.push_back(n.pos);
					float mult = pathLen - n.path;
					int noActionChance = (int)((pathLen - n.path) * 10);
					if (noActionChance < 0) noActionChance = 0;
					Pos nextPos = getPosForDir(n.pos, n.dir);

					int left = isNodeEmpty(getPosForDir(n.pos, n.dir + 1));
					int right = isNodeEmpty(getPosForDir(n.pos, n.dir - 1));
					int front = isNodeEmpty(getPosForDir(n.pos, n.dir));

					if (!front)
						noActionChance = 0;

					int nCount = left + right + front;


					float changeDirMult = nCount >= 1 ? 1.0f : 0.0f;
					float threeWayMult = nCount >= 2 ? 1.0f : 0.0f;
					float fourWayMult = nCount >= 3 ? 1.0f : 0.0f;
					float stopMult = nodes.size() > 1 ? 1.0f : 0.0f;

					printw("%d << %d %d << %d %f %f %f %f  << %d %d %d\n", n.dir, n.pos.x, n.pos.y, noActionChance, changeDirMult, threeWayMult, fourWayMult, stopMult, left, right, front);
					//write();
					if (n.pos.x == 12 && n.pos.y == 10) {
						int a = 5;
					}
					int nextAction = getRandomAction(noActionChance, changeDirMult, threeWayMult, fourWayMult, stopMult);

					int oldDir = getReverseDir(n.dir);

					if (arr[n.pos.x][n.pos.y] != 0) {
						nextAction = 4;
					} else {
						arr[n.pos.x][n.pos.y] |= 1 << oldDir;

					}

					if (nextAction == 0) {
						arr[n.pos.x][n.pos.y] |= 1 << n.dir;
						n.pos = getPosForDir(n.pos, n.dir);
						n.path++;
					} else if (nextAction == 1) {
						int z;
						if (left && right) {
							z = ran(2) == 0 ? -1 : 1;
						} else if (left) {
							z = +1;
						} else {
							z = -1;
						}

						n.dir = getSideDir(n.dir, z);
						arr[n.pos.x][n.pos.y] |= 1 << n.dir;
						n.pos = getPosForDir(n.pos, n.dir);
						n.path = 0;
					} else if (nextAction == 2) {

						int ns[4];
						int j = 0;
						if (left) ns[j++] = getSideDir(n.dir, +1);
						if (right) ns[j++] = getSideDir(n.dir, -1);
						if (front) ns[j++] = n.dir;
						int n1 = ran(j);
						int n2;
						do {
							n2 = ran(j);
						} while (n2 == n1);
						n1 = ns[n1];
						n2 = ns[n2];

						n.dir = n1;

						arr[n.pos.x][n.pos.y] |= 1 << n1;
						arr[n.pos.x][n.pos.y] |= 1 << n2;

						n.path = 0;

						Node newNode = Node(n.pos.x, n.pos.y, n2);
						n.pos = getPosForDir(n.pos, n.dir);
						newNode.pos = getPosForDir(newNode.pos, n2);
						nodes.push_back(newNode);
					} else if (nextAction == 3) {
						int n1 = getSideDir(n.dir, -1);
						int n2 = getSideDir(n.dir, +1);

						arr[n.pos.x][n.pos.y] |= 1 << n1;
						arr[n.pos.x][n.pos.y] |= 1 << n2;
						arr[n.pos.x][n.pos.y] |= 1 << n.dir;
						n.path = 0;

						n.pos = getPosForDir(n.pos, n.dir);

						Node newNode = Node(n.pos.x, n.pos.y, n1);
						Node newNode2 = Node(n.pos.x, n.pos.y, n2);
						newNode.pos = getPosForDir(newNode.pos, n1);
						newNode2.pos = getPosForDir(newNode2.pos, n2);
						nodes.push_back(newNode);
						nodes.push_back(newNode2);

					} else {
						if (nodes.size() > 0) {

							nodes[i] = nodes[nodes.size() - 1];
							nodes.pop_back();
							i--;
						}
					}
					//write();
				}
			}
			/*
			if (added.size() < 2) {
			for (unsigned i = 0; i < added.size(); i++) {
			arr[added[i].x][added[i].y] = 0xFF;
			}
			}*/
		}

		printw(" done\n"); refresh();
	}

	void setWindow(GameWindow<MAZE_W, MAZE_H> *window) {
		this->window = window;
	}

	void updateWindow() {
		for (int i = 0; i < MAZE_W; i++) {
			for (int j = 0; j < MAZE_H; j++) {
				window->data[i][j].character = walls[i][j] == 1 ? '*' : ' ';
				window->data[i][j].color = COLOR_WHITE;
			}
		}
	}
};