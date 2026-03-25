#include "Map.h"

static void Map_loadLevel1Data(Map* m) {
	int i, j;
	
	/* ---- 路点 ---- */
	m->waypoints[m->waypointCount++] = Map_getCenter(3,  6);
	m->waypoints[m->waypointCount++] = Map_getCenter(4,  7);
	m->waypoints[m->waypointCount++] = Map_getCenter(11, 7);
	m->waypoints[m->waypointCount++] = Map_getCenter(11, 10);
	m->waypoints[m->waypointCount++] = Map_getCenter(7,  15);
	m->waypoints[m->waypointCount++] = Map_getCenter(7,  19);
	m->waypoints[m->waypointCount++] = Map_getCenter(11, 24);
	m->waypoints[m->waypointCount++] = Map_getCenter(13, 24);
	m->waypoints[m->waypointCount++] = Map_getCenter(16, 21);
	m->waypoints[m->waypointCount++] = Map_getCenter(16, 16);
	m->waypoints[m->waypointCount++] = Map_getCenter(21, 16);
	
	/* ================================================================
	TYPE_PATH：严格按 Bresenham 折线覆盖，每格对应敌人实际经过的格子
	================================================================ */
	
	/* 段0→1: (3,6)→(4,7) 斜线入场 */
	m->grid[3][6] = TYPE_PATH;
	m->grid[4][7] = TYPE_PATH;
	
	/* 段1→2: col7 垂直向下 row4→11（8格，最长直线——骨矛阵/震天雷核心区）*/
	for (i = 4; i <= 11; i++) m->grid[i][7] = TYPE_PATH;
	
	/* 段2→3: row11 水平向右 col7→10 */
	for (j = 7; j <= 10; j++) m->grid[11][j] = TYPE_PATH;
	
	/* 段3→4: (11,10)→(7,15) 斜线向右上 */
	/* Bresenham: dr=-4 dc=5 → 主水平，每1.25列升1行 */
	m->grid[11][10] = TYPE_PATH;
	m->grid[10][11] = TYPE_PATH;
	m->grid[9][12]  = TYPE_PATH;
	m->grid[9][13]  = TYPE_PATH;
	m->grid[8][14]  = TYPE_PATH;
	m->grid[7][15]  = TYPE_PATH;
	
	/* 段4→5: row7 水平向右 col15→19（5格直线——骨矛阵/震天雷黄金区）*/
	for (j = 15; j <= 19; j++) m->grid[7][j] = TYPE_PATH;
	
	/* 段5→6: (7,19)→(11,24) 斜线向右下 */
	/* Bresenham: dr=4 dc=5 → 主水平 */
	m->grid[7][19]  = TYPE_PATH;
	m->grid[8][20]  = TYPE_PATH;
	m->grid[9][21]  = TYPE_PATH;
	m->grid[9][22]  = TYPE_PATH;
	m->grid[10][23] = TYPE_PATH;
	m->grid[11][24] = TYPE_PATH;
	
	/* 段6→7: col24 垂直向下 row11→13 */
	for (i = 11; i <= 13; i++) m->grid[i][24] = TYPE_PATH;
	
	/* 段7→8: (13,24)→(16,21) 斜线向左下 */
	/* Bresenham: dr=3 dc=-3 → 45度斜线 */
	m->grid[13][24] = TYPE_PATH;
	m->grid[14][23] = TYPE_PATH;
	m->grid[15][22] = TYPE_PATH;
	m->grid[16][21] = TYPE_PATH;
	
	/* 段8→9: row16 水平向左 col21→16（6格直线——震天雷/骨矛阵优质区）*/
	for (j = 16; j <= 21; j++) m->grid[16][j] = TYPE_PATH;
	
	/* 段9→10: col16 垂直向下 row16→21（6格直线——骨矛阵/震天雷优质区）*/
	for (i = 16; i <= 21; i++) m->grid[i][16] = TYPE_PATH;
	
	/* ================================================================
	TYPE_BUILDABLE：高地区域，紧贴长直线两侧，让塔覆盖最多路径
	================================================================
	
	布局策略：
	- col7 垂直段左侧 col5-6：居高临下，骨矛阵穿刺整列
	- col7 垂直段右侧 col8-9（row5-10）：侧射覆盖
	- row7 水平段上方 row5-6（col15-20）：骨矛阵横扫整行
	- row7 水平段下方 row8-9（col16-19）：双侧覆盖
	- row16 水平段上方 row14-15（col17-20）：覆盖长水平段
	- col16 垂直段左侧 col14-15（row17-20）：侧射
	- 传送门专属：col24 附近和 col7 附近各留路径格（已是 TYPE_PATH）
	================================================================ */
	
	/* col7 段左侧：row5-10, col4-6 */
	for (i = 5; i <= 10; i++) {
		m->grid[i][4] = TYPE_BUILDABLE;
		m->grid[i][5] = TYPE_BUILDABLE;
		m->grid[i][6] = TYPE_BUILDABLE;
	}
	
	/* col7 段右侧：row5-10, col8-9（不与斜线路径冲突）*/
	for (i = 5; i <= 10; i++) {
		m->grid[i][8] = TYPE_BUILDABLE;
		m->grid[i][9] = TYPE_BUILDABLE;
	}
	
	/* row7 段上方：row5-6, col14-20 */
	for (j = 14; j <= 20; j++) {
		m->grid[5][j] = TYPE_BUILDABLE;
		m->grid[6][j] = TYPE_BUILDABLE;
	}
	
	/* row7 段下方：row8-9, col16-19（避开斜线格）*/
	for (j = 16; j <= 19; j++) {
		m->grid[8][j] = TYPE_BUILDABLE;
		m->grid[9][j] = TYPE_BUILDABLE;
	}
	
	/* col24 段两侧：row11-13, col25-26（传送门/骨矛阵放置区）*/
	for (i = 11; i <= 13; i++) {
		m->grid[i][25] = TYPE_BUILDABLE;
		m->grid[i][26] = TYPE_BUILDABLE;
	}
	
	/* row16 段上方：row14-15, col17-21 */
	for (j = 17; j <= 21; j++) {
		m->grid[14][j] = TYPE_BUILDABLE;
		m->grid[15][j] = TYPE_BUILDABLE;
	}
	
	/* col16 段左侧：row17-21, col14-15 */
	for (i = 17; i <= 21; i++) {
		m->grid[i][14] = TYPE_BUILDABLE;
		m->grid[i][15] = TYPE_BUILDABLE;
	}
	
	/* col16 段右侧：row17-21, col17-18 */
	for (i = 17; i <= 21; i++) {
		m->grid[i][17] = TYPE_BUILDABLE;
		m->grid[i][18] = TYPE_BUILDABLE;
	}
	
	/* 额外：终点附近高地 row19-21, col13-14（基地周边防守）*/
	for (i = 19; i <= 21; i++) {
		m->grid[i][13] = TYPE_BUILDABLE;
	}
}

void Map_init(Map* m, int level) {
	int i, j;
	m->waypointCount = 0;
	for (i = 0; i < ROWS; i++)
		for (j = 0; j < COLS; j++)
			m->grid[i][j] = TYPE_OBSTACLE;
	switch (level) {
	case 1:
	case 2:
	case 3:
		Map_loadLevel1Data(m);
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
	int offsetX = (1024 - m->bgImg.width)  / 2;
	int offsetY = (768  - m->bgImg.height) / 2;
	DrawTexture(m->bgImg, offsetX, offsetY, WHITE);
	
	if (IsKeyDown(KEY_SPACE)) {
		for (i = 0; i < ROWS; i++) {
			for (j = 0; j < COLS; j++) {
				if      (m->grid[i][j] == TYPE_PATH)      DrawRectangle(j*CELL_SIZE, i*CELL_SIZE, CELL_SIZE, CELL_SIZE, Fade(DARKBROWN, 0.5f));
				else if (m->grid[i][j] == TYPE_BUILDABLE) DrawRectangle(j*CELL_SIZE, i*CELL_SIZE, CELL_SIZE, CELL_SIZE, Fade(DARKGREEN, 0.5f));
				else                                       DrawRectangle(j*CELL_SIZE, i*CELL_SIZE, CELL_SIZE, CELL_SIZE, Fade(DARKGRAY,  0.5f));
				DrawRectangleLines(j*CELL_SIZE, i*CELL_SIZE, CELL_SIZE, CELL_SIZE, Fade(BLACK, 0.2f));
			}
		}
		for (i = 0; i < m->waypointCount - 1; i++) {
			Vector2 p1 = {(float)m->waypoints[i].x,   (float)m->waypoints[i].y};
			Vector2 p2 = {(float)m->waypoints[i+1].x, (float)m->waypoints[i+1].y};
			DrawLineEx(p1, p2, 3.0f, RED);
			DrawCircleV(p1, 4, YELLOW);
		}
	}
}
