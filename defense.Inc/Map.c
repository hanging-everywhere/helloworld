#include "Map.h"

void Map_init(Map* m, int level) {
	int i, j;
	m->waypointCount = 0;
	
	// 初始化所有格子为障碍物 (TYPE_OBSTACLE)
	for(i = 0; i < ROWS; i++) {
		for(j = 0; j < COLS; j++) m->grid[i][j] = TYPE_OBSTACLE; 
	}
	
	// 根据传入的关卡编号加载不同的地图
	switch (level) {
		case 1: // 第一关：石器时代
		// 1. 加载第一关背景图
		m->bgImg = LoadTexture("background(1).png"); 
		
		// 2. 第一关的土路 (TYPE_PATH)
		for(i = 0; i <= 11; i++) { m->grid[i][6] = TYPE_PATH; m->grid[i][7] = TYPE_PATH; }
		for(j = 8; j <= 10; j++) { m->grid[11][j] = TYPE_PATH; m->grid[10][j] = TYPE_PATH; }
		m->grid[9][11] = TYPE_PATH; m->grid[9][12] = TYPE_PATH;
		m->grid[8][13] = TYPE_PATH; m->grid[7][14] = TYPE_PATH;
		for(j = 15; j <= 25; j++) { m->grid[6][j] = TYPE_PATH; m->grid[7][j] = TYPE_PATH; }
		for(i = 8; i <= 15; i++) { m->grid[i][24] = TYPE_PATH; m->grid[i][25] = TYPE_PATH; }
		for(j = 19; j <= 23; j++) { m->grid[14][j] = TYPE_PATH; m->grid[15][j] = TYPE_PATH; }
		for(i = 16; i < ROWS; i++) { m->grid[i][18] = TYPE_PATH; m->grid[i][19] = TYPE_PATH; }
		
		// 3. 第一关的大片草地平台 (TYPE_BUILDABLE)
		for(i = 2; i <= 10; i++) { for(j = 1; j <= 4; j++) m->grid[i][j] = TYPE_BUILDABLE; }
		for(i = 13; i <= 16; i++) { for(j = 2; j <= 5; j++) m->grid[i][j] = TYPE_BUILDABLE; }
		for(i = 12; i <= 17; i++) { for(j = 9; j <= 15; j++) m->grid[i][j] = TYPE_BUILDABLE; }
		for(i = 1; i <= 4; i++) { for(j = 11; j <= 18; j++) m->grid[i][j] = TYPE_BUILDABLE; }
		for(i = 9; i <= 13; i++) { for(j = 27; j <= 30; j++) m->grid[i][j] = TYPE_BUILDABLE; }
		for(i = 17; i <= 20; i++) { for(j = 22; j <= 28; j++) m->grid[i][j] = TYPE_BUILDABLE; }
		
		// 4. 第一关的敌军路点 (Waypoints)
		m->waypoints[m->waypointCount++] = Map_getCenter(0, 6);   
		m->waypoints[m->waypointCount++] = Map_getCenter(11, 6);  
		m->waypoints[m->waypointCount++] = Map_getCenter(11, 9);  
		m->waypoints[m->waypointCount++] = Map_getCenter(6, 15);  
		m->waypoints[m->waypointCount++] = Map_getCenter(6, 25);  
		m->waypoints[m->waypointCount++] = Map_getCenter(15, 25); 
		m->waypoints[m->waypointCount++] = Map_getCenter(15, 18); 
		m->waypoints[m->waypointCount++] = Map_getCenter(23, 18); 
		break;
		
		case 2: // 第二关：古代文明 (占位，以后再填)
		// m->bgImg = LoadTexture("background(2).jpg"); 
		break;
		
		case 3: // 第三关：科幻未来 (占位，以后再填)
		// m->bgImg = LoadTexture("background(3).jpg"); 
		break;
	}
}

Point Map_getCenter(int row, int col) {
	Point p;
	p.x = col * CELL_SIZE + CELL_SIZE / 2;
	p.y = row * CELL_SIZE + CELL_SIZE / 2; 
	return p;
}

void Map_draw(Map* m) {
	int i, j;
	
	// 绘制背景图
	DrawTexture(m->bgImg, 0, 0, WHITE);
	
	// 按下空格键显示调试网格
	if (IsKeyDown(KEY_SPACE)) {
		for(i = 0; i < ROWS; i++) {
			for(j = 0; j < COLS; j++) {
				if (m->grid[i][j] == TYPE_PATH) {
					DrawRectangle(j * CELL_SIZE, i * CELL_SIZE, CELL_SIZE, CELL_SIZE, Fade(DARKBROWN, 0.5f));
				} else if (m->grid[i][j] == TYPE_BUILDABLE) {
					DrawRectangle(j * CELL_SIZE, i * CELL_SIZE, CELL_SIZE, CELL_SIZE, Fade(DARKGREEN, 0.5f));
				} else {
					DrawRectangle(j * CELL_SIZE, i * CELL_SIZE, CELL_SIZE, CELL_SIZE, Fade(DARKGRAY, 0.5f));
				}
				DrawRectangleLines(j * CELL_SIZE, i * CELL_SIZE, CELL_SIZE, CELL_SIZE, Fade(BLACK, 0.2f));
			}
		}
		
		for (i = 0; i < m->waypointCount - 1; i++) {
			Vector2 p1 = {(float)m->waypoints[i].x, (float)m->waypoints[i].y};
			Vector2 p2 = {(float)m->waypoints[i+1].x, (float)m->waypoints[i+1].y};
			DrawLineEx(p1, p2, 3.0f, RED);
			DrawCircleV(p1, 4, YELLOW);
		}
	}
}
