#include "Map.h"

Map::Map() {
	for(int i = 0; i < ROWS; i++) {
		for(int j = 0; j < COLS; j++) grid[i][j] = 0;
	}
}

void Map::init() {
	for(int j = 0; j <= 12; j++) grid[2][j] = 1;
	for(int i = 3; i <= 8; i++)  grid[i][12] = 1;
	for(int j = 11; j >= 3; j--) grid[8][j] = 1;
	for(int i = 9; i <= 11; i++) grid[i][3] = 1;
	
	waypoints.push_back(getCenter(2, 0));  
	waypoints.push_back(getCenter(2, 12)); 
	waypoints.push_back(getCenter(8, 12)); 
	waypoints.push_back(getCenter(8, 3));  
	waypoints.push_back(getCenter(11, 3)); 
}

Point Map::getCenter(int row, int col) {
	Point p;
	p.x = col * CELL_SIZE + CELL_SIZE / 2;
	p.y = row * CELL_SIZE + CELL_SIZE / 2;
	return p;
}

void Map::draw() {
	for(int i = 0; i < ROWS; i++) {
		for(int j = 0; j < COLS; j++) {
			int x = j * CELL_SIZE;
			int y = i * CELL_SIZE;
			
			if(grid[i][j] == 1) {
				// 黑夜下的泥土路 (深褐色)
				setfillcolor(EGERGB(50, 40, 30));
				bar(x, y, x + CELL_SIZE, y + CELL_SIZE);
			} else {
				// 黑夜下的草地 (极暗的墨绿色)
				setfillcolor(EGERGB(15, 35, 20));
				bar(x, y, x + CELL_SIZE, y + CELL_SIZE);
				// 微弱的网格线
				setcolor(EGERGB(20, 50, 30));
				rectangle(x, y, x + CELL_SIZE, y + CELL_SIZE);
			}
		}
	}
}
