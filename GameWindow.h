#pragma once

#include "Tools.h"

template <int W, int H>
class GameWindow {
public:
	char border[10];
	bool hasBorder;

	Pos windowSize;
	Pos windowPos;

	Pos mapCenter;

	Pixel data[H][W];
	GameWindow() {

		strcpy_s(border, "+-+| |+-+");
		hasBorder = true;
		for (int i = 0; i < H; i++) {
			for (int j = 0; j < W; j++) {
				data[i][j].character = ' ';
				data[i][j].color = 0;
			}
		}
	}

	void render() {
		int startX = 0;
		int startY = 0;
		int endX = windowSize.x - 1;
		int endY = windowSize.y - 1;

		int x = windowPos.x;
		int y = windowPos.y;
		if (hasBorder) {

			attrset(COLOR_PAIR(255));
			mvaddch(x + 0, y + 0, border[0]);
			for (int i = 1; i < endY - 1; i++) mvaddch(x + 0, y + i, border[1]);
			mvaddch(x + 0, y + endY - 1, border[2]);
			for (int i = 1; i < endX - 1; i++) mvaddch(x + i, y + 0, border[3]);
			for (int i = 1; i < endX - 1; i++) mvaddch(x + i, y + endY - 1, border[5]);
			mvaddch(x + endX - 1, y + 0, border[6]);
			for (int i = 1; i < endY - 1; i++)  mvaddch(x + endX - 1, y + i, border[7]);
			mvaddch(x + endX - 1, y + endY - 1, border[8]);


			startX++;
			startY++;
			endX -= 2;
			endY -= 2;
		}

		int rW = endX - startX + 1;
		int rH = endY - startY + 1;

		int bX = mapCenter.x - rW / 2;
		int bY = mapCenter.y - rH / 2;


		for (int j = startY; j <= endY; j++) {
			for (int i = startX; i <= endX; i++) {
				int px = bX + i;
				int py = bY + j;
				if (px < 0 || px >= W || py < 0 || py >= H) {
					mvaddch(x + i, y + j, ' ');
				} else {
					attrset(COLOR_PAIR(data[px][py].color));
					mvaddch(x + i, y + j, data[px][py].character);
				}
			}
		}
	}
};