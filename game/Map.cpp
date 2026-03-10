#include "Map.h"

void Map_init(Map* m) {
	int i, j;
	m->waypointCount = 0;
	
	for(i = 0; i < ROWS; i++) {
		for(j = 0; j < COLS; j++) m->grid[i][j] = 0;
	}
	
	for(j = 0; j <= 12; j++) m->grid[2][j] = 1;
	for(i = 3; i <= 8; i++)  m->grid[i][12] = 1;
	for(j = 11; j >= 3; j--) m->grid[8][j] = 1;
	for(i = 9; i <= 11; i++) m->grid[i][3] = 1;
	
	// 手动添加路点到数组
	m->waypoints[m->waypointCount++] = Map_getCenter(2, 0);  
	m->waypoints[m->waypointCount++] = Map_getCenter(2, 12); 
	m->waypoints[m->waypointCount++] = Map_getCenter(8, 12); 
	m->waypoints[m->waypointCount++] = Map_getCenter(8, 3);  
	m->waypoints[m->waypointCount++] = Map_getCenter(11, 3); 
}

Point Map_getCenter(int row, int col) {
	Point p;
	p.x = col * CELL_SIZE + CELL_SIZE / 2;
	p.y = row * CELL_SIZE + CELL_SIZE / 2;
	return p;
}

void Map_draw(Map* m) {
	int i, j;
	for(i = 0; i < ROWS; i++) {
		for(j = 0; j < COLS; j++) {
			int x = j * CELL_SIZE;
			int y = i * CELL_SIZE;
			
			if(m->grid[i][j] == 1) {
				setfillcolor(EGERGB(50, 40, 30));
				bar(x, y, x + CELL_SIZE, y + CELL_SIZE);
			} else {
				setfillcolor(EGERGB(15, 35, 20));
				bar(x, y, x + CELL_SIZE, y + CELL_SIZE);
				setcolor(EGERGB(20, 50, 30));
				rectangle(x, y, x + CELL_SIZE, y + CELL_SIZE);
			}
		}
	}
}
