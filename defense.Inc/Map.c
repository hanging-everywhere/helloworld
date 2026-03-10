#include "Map.h"

void Map_init(Map* m) {
	int i, j;
	m->waypointCount = 0;
	
	// 初始化所有格子为空地草地 (0)
	for(i = 0; i < ROWS; i++) {
		for(j = 0; j < COLS; j++) m->grid[i][j] = 0;
	}
	
	// 绘制经典的 S 型泥土路线 (1)
	for(j = 0; j <= 12; j++) m->grid[2][j] = 1;
	for(i = 3; i <= 8; i++)  m->grid[i][12] = 1;
	for(j = 11; j >= 3; j--) m->grid[8][j] = 1;
	for(i = 9; i <= 11; i++) m->grid[i][3] = 1;
	
	// 添加路点
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
				// 黑夜下的泥土路 (深褐色)
				DrawRectangle(x, y, CELL_SIZE, CELL_SIZE, (Color){50, 40, 30, 255});
			} else {
				// 黑夜下的草地 (极暗的墨绿色)
				DrawRectangle(x, y, CELL_SIZE, CELL_SIZE, (Color){15, 35, 20, 255});
				// 微弱的网格线
				DrawRectangleLines(x, y, CELL_SIZE, CELL_SIZE, (Color){20, 50, 30, 255});
			}
		}
	}
}
