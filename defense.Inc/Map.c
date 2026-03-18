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
		
		// 2. 重新规划敌军路点 (Waypoints) - 完美适配居中后的新背景图
		m->waypoints[m->waypointCount++] = Map_getCenter(3, 6);   
		m->waypoints[m->waypointCount++] = Map_getCenter(4, 7);
		m->waypoints[m->waypointCount++] = Map_getCenter(11, 7);  
		m->waypoints[m->waypointCount++] = Map_getCenter(12, 9);  
		m->waypoints[m->waypointCount++] = Map_getCenter(11, 11);
		m->waypoints[m->waypointCount++] = Map_getCenter(7, 16);  
		m->waypoints[m->waypointCount++] = Map_getCenter(7, 20);  
		m->waypoints[m->waypointCount++] = Map_getCenter(11, 25); 
		m->waypoints[m->waypointCount++] = Map_getCenter(14, 25); 
		m->waypoints[m->waypointCount++] = Map_getCenter(17, 22); 
		m->waypoints[m->waypointCount++] = Map_getCenter(17, 17); 
		m->waypoints[m->waypointCount++] = Map_getCenter(21, 17); 
		
        // --- 3. 铺设道路网格 (TYPE_PATH, 允许造骨矛塔) ---
		for(i = 3; i <= 11; i++) {  m->grid[i][7] = TYPE_PATH; }
		for(j = 8; j <= 11; j++) { m->grid[11][j] = TYPE_PATH; m->grid[12][j] = TYPE_PATH; }
		m->grid[10][12] = TYPE_PATH; m->grid[10][13] = TYPE_PATH;
		m->grid[9][13] = TYPE_PATH; m->grid[9][14] = TYPE_PATH;
		m->grid[8][14] = TYPE_PATH; m->grid[8][15] = TYPE_PATH;
		for(j = 15; j <= 20; j++) { m->grid[7][j] = TYPE_PATH; }
		m->grid[8][21] = TYPE_PATH; m->grid[8][22] = TYPE_PATH;
		m->grid[9][22] = TYPE_PATH; m->grid[9][23] = TYPE_PATH;
		m->grid[10][23] = TYPE_PATH; m->grid[10][24] = TYPE_PATH;
		for(i = 11; i <= 14; i++) { m->grid[i][25] = TYPE_PATH; }
		m->grid[15][24] = TYPE_PATH; m->grid[15][23] = TYPE_PATH;
		m->grid[16][23] = TYPE_PATH; m->grid[16][22] = TYPE_PATH;
		for(j = 17; j <= 22; j++) {m->grid[17][j] = TYPE_PATH; }
		for(i = 18; i <= 21; i++) {m->grid[i][17] = TYPE_PATH; }
		
		// --- 4. 铺设高地网格 (TYPE_BUILDABLE, 允许造投石索/图腾) ---
		// 左侧出怪口外侧的草地
		for(i = 5; i <= 8; i++) { for(j = 3; j <= 4; j++) m->grid[i][j] = TYPE_BUILDABLE; }
		// 第一个U型弯内侧的高地
		for(i = 8; i <= 9; i++) { for(j = 9; j <= 10; j++) m->grid[i][j] = TYPE_BUILDABLE; }
		// 顶部直道上方的草地 (适合放图腾覆盖大面积)
		for(i = 4; i <= 5; i++) { for(j = 16; j <= 19; j++) m->grid[i][j] = TYPE_BUILDABLE; }
		// 顶部直道下方 (地图中央区域)
		for(i = 9; i <= 10; i++) { for(j = 17; j <= 19; j++) m->grid[i][j] = TYPE_BUILDABLE; }
		// 右侧大弯道外侧高地
		for(i = 9; i <= 12; i++) { for(j = 27; j <= 28; j++) m->grid[i][j] = TYPE_BUILDABLE; }
		// 底部直道下方 (大蘑菇房右下方区域)
		for(i = 19; i <= 20; i++) { for(j = 20; j <= 24; j++) m->grid[i][j] = TYPE_BUILDABLE; }
		// 基地左侧高地
		for(i = 18; i <= 20; i++) { for(j = 13; j <= 14; j++) m->grid[i][j] = TYPE_BUILDABLE; }   
		
		break;
		
		case 2: // 第二关：古代文明 (占位，以后再填)
		break;
		
		case 3: // 第三关：科幻未来 (占位，以后再填)
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
	
	// 计算居中偏移量 
	// 窗口是 1024x768，图片是 1024x576，所以水平偏移为0，垂直向下偏移 96 像素
	int offsetX = (1024 - m->bgImg.width) / 2;
	int offsetY = (768 - m->bgImg.height) / 2;
	
	// 绘制背景图时加上偏移量，使其居中
	DrawTexture(m->bgImg, offsetX, offsetY, WHITE);
	
	// 按下空格键显示调试网格 (帮你重新铺设防御塔位置的利器)
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
