/**
 * @file    map.c
 * @brief   地图模块实现（迁移自 prototype/v2/Map.c，已适配新 API）
 *          Map module implementation (migrated from prototype/v2/Map.c, adapted to new API).
 * @version 2.0
 */

#include "map.h"

/* ============================================================
* 内部：关卡1地形数据（三个关卡暂时共用同一布局）
* Internal: Level 1 terrain data (all 3 levels share the same layout for now)
* ============================================================ */
/* ============================================================
* Level 1 (Stone Age) map data
* ============================================================ */
static void load_level1_data(Map* m) {
	int i, j;
	
	/* ---- Waypoints ---- */
	m->waypoints[m->waypoint_count++] = map_get_center(3,  6);
	m->waypoints[m->waypoint_count++] = map_get_center(4,  7);
	m->waypoints[m->waypoint_count++] = map_get_center(11, 7);
	m->waypoints[m->waypoint_count++] = map_get_center(11, 10);
	m->waypoints[m->waypoint_count++] = map_get_center(7,  15);
	m->waypoints[m->waypoint_count++] = map_get_center(7,  19);
	m->waypoints[m->waypoint_count++] = map_get_center(11, 24);
	m->waypoints[m->waypoint_count++] = map_get_center(13, 24);
	m->waypoints[m->waypoint_count++] = map_get_center(16, 21);
	m->waypoints[m->waypoint_count++] = map_get_center(16, 16);
	m->waypoints[m->waypoint_count++] = map_get_center(21, 16);
	
	/* ---- Path cells ---- */
	m->grid[3][6] = CELL_TYPE_PATH;
	m->grid[4][7] = CELL_TYPE_PATH;
	for (i = 4; i <= 11; i++) m->grid[i][7]  = CELL_TYPE_PATH;
	for (j = 7; j <= 10; j++) m->grid[11][j] = CELL_TYPE_PATH;
	m->grid[11][10] = CELL_TYPE_PATH;  m->grid[10][11] = CELL_TYPE_PATH;
	m->grid[9][12]  = CELL_TYPE_PATH;  m->grid[9][13]  = CELL_TYPE_PATH;
	m->grid[8][14]  = CELL_TYPE_PATH;  m->grid[7][15]  = CELL_TYPE_PATH;
	for (j = 15; j <= 19; j++) m->grid[7][j]  = CELL_TYPE_PATH;
	m->grid[7][19]  = CELL_TYPE_PATH;  m->grid[8][20]  = CELL_TYPE_PATH;
	m->grid[9][21]  = CELL_TYPE_PATH;  m->grid[9][22]  = CELL_TYPE_PATH;
	m->grid[10][23] = CELL_TYPE_PATH;  m->grid[11][24] = CELL_TYPE_PATH;
	for (i = 11; i <= 13; i++) m->grid[i][24] = CELL_TYPE_PATH;
	m->grid[13][24] = CELL_TYPE_PATH;  m->grid[14][23] = CELL_TYPE_PATH;
	m->grid[15][22] = CELL_TYPE_PATH;  m->grid[16][21] = CELL_TYPE_PATH;
	for (j = 16; j <= 21; j++) m->grid[16][j] = CELL_TYPE_PATH;
	for (i = 16; i <= 21; i++) m->grid[i][16] = CELL_TYPE_PATH;
	
	/* ---- Buildable cells ---- */
	for (i = 5; i <= 10; i++) { m->grid[i][4]=CELL_TYPE_BUILDABLE; m->grid[i][5]=CELL_TYPE_BUILDABLE; m->grid[i][6]=CELL_TYPE_BUILDABLE; }
	for (i = 5; i <= 10; i++) { m->grid[i][8]=CELL_TYPE_BUILDABLE; m->grid[i][9]=CELL_TYPE_BUILDABLE; }
	for (j = 14; j <= 20; j++) { m->grid[5][j]=CELL_TYPE_BUILDABLE; m->grid[6][j]=CELL_TYPE_BUILDABLE; }
	for (j = 16; j <= 19; j++) { m->grid[8][j]=CELL_TYPE_BUILDABLE; m->grid[9][j]=CELL_TYPE_BUILDABLE; }
	for (i = 11; i <= 13; i++) { m->grid[i][25]=CELL_TYPE_BUILDABLE; m->grid[i][26]=CELL_TYPE_BUILDABLE; }
	for (j = 17; j <= 21; j++) { m->grid[14][j]=CELL_TYPE_BUILDABLE; m->grid[15][j]=CELL_TYPE_BUILDABLE; }
	for (i = 17; i <= 20; i++) { m->grid[i][14]=CELL_TYPE_BUILDABLE; m->grid[i][15]=CELL_TYPE_BUILDABLE; }
	for (i = 17; i <= 20; i++) { m->grid[i][17]=CELL_TYPE_BUILDABLE; m->grid[i][18]=CELL_TYPE_BUILDABLE; }
}

/* ============================================================
* Level 2 (Ancient Age) map data — edit waypoints/path/buildable as needed
* ============================================================ */
static void load_level2_data(Map* m) {
	int i, j;
	
	/* ---- Waypoints (edit these) ---- */
	m->waypoints[m->waypoint_count++] = map_get_center(3,  6);
	m->waypoints[m->waypoint_count++] = map_get_center(4,  7);
	m->waypoints[m->waypoint_count++] = map_get_center(11, 7);
	m->waypoints[m->waypoint_count++] = map_get_center(11, 10);
	m->waypoints[m->waypoint_count++] = map_get_center(7,  15);
	m->waypoints[m->waypoint_count++] = map_get_center(7,  19);
	m->waypoints[m->waypoint_count++] = map_get_center(11, 24);
	m->waypoints[m->waypoint_count++] = map_get_center(13, 24);
	m->waypoints[m->waypoint_count++] = map_get_center(16, 21);
	m->waypoints[m->waypoint_count++] = map_get_center(16, 16);
	m->waypoints[m->waypoint_count++] = map_get_center(21, 16);
	
	/* ---- Path cells (edit to match your waypoints) ---- */
	m->grid[3][6] = CELL_TYPE_PATH;
	m->grid[4][7] = CELL_TYPE_PATH;
	for (i = 4; i <= 11; i++) m->grid[i][7]  = CELL_TYPE_PATH;
	for (j = 7; j <= 10; j++) m->grid[11][j] = CELL_TYPE_PATH;
	m->grid[11][10] = CELL_TYPE_PATH;  m->grid[10][11] = CELL_TYPE_PATH;
	m->grid[9][12]  = CELL_TYPE_PATH;  m->grid[9][13]  = CELL_TYPE_PATH;
	m->grid[8][14]  = CELL_TYPE_PATH;  m->grid[7][15]  = CELL_TYPE_PATH;
	for (j = 15; j <= 19; j++) m->grid[7][j]  = CELL_TYPE_PATH;
	m->grid[7][19]  = CELL_TYPE_PATH;  m->grid[8][20]  = CELL_TYPE_PATH;
	m->grid[9][21]  = CELL_TYPE_PATH;  m->grid[9][22]  = CELL_TYPE_PATH;
	m->grid[10][23] = CELL_TYPE_PATH;  m->grid[11][24] = CELL_TYPE_PATH;
	for (i = 11; i <= 13; i++) m->grid[i][24] = CELL_TYPE_PATH;
	m->grid[13][24] = CELL_TYPE_PATH;  m->grid[14][23] = CELL_TYPE_PATH;
	m->grid[15][22] = CELL_TYPE_PATH;  m->grid[16][21] = CELL_TYPE_PATH;
	for (j = 16; j <= 21; j++) m->grid[16][j] = CELL_TYPE_PATH;
	for (i = 16; i <= 21; i++) m->grid[i][16] = CELL_TYPE_PATH;
	
	/* ---- Buildable cells (edit as needed) ---- */
	for (i = 5; i <= 10; i++) { m->grid[i][4]=CELL_TYPE_BUILDABLE; m->grid[i][5]=CELL_TYPE_BUILDABLE; m->grid[i][6]=CELL_TYPE_BUILDABLE; }
	for (i = 5; i <= 10; i++) { m->grid[i][8]=CELL_TYPE_BUILDABLE; m->grid[i][9]=CELL_TYPE_BUILDABLE; }
	for (j = 14; j <= 20; j++) { m->grid[5][j]=CELL_TYPE_BUILDABLE; m->grid[6][j]=CELL_TYPE_BUILDABLE; }
	for (j = 16; j <= 19; j++) { m->grid[8][j]=CELL_TYPE_BUILDABLE; m->grid[9][j]=CELL_TYPE_BUILDABLE; }
	for (i = 11; i <= 13; i++) { m->grid[i][25]=CELL_TYPE_BUILDABLE; m->grid[i][26]=CELL_TYPE_BUILDABLE; }
	for (j = 17; j <= 21; j++) { m->grid[14][j]=CELL_TYPE_BUILDABLE; m->grid[15][j]=CELL_TYPE_BUILDABLE; }
	for (i = 17; i <= 20; i++) { m->grid[i][14]=CELL_TYPE_BUILDABLE; m->grid[i][15]=CELL_TYPE_BUILDABLE; }
	for (i = 17; i <= 20; i++) { m->grid[i][17]=CELL_TYPE_BUILDABLE; m->grid[i][18]=CELL_TYPE_BUILDABLE; }
}

/* ============================================================
* Level 3 (Future Age) map data — edit waypoints/path/buildable as needed
* ============================================================ */
static void load_level3_data(Map* m) {
	int i, j;
	
	/* ---- Waypoints (edit these) ---- */
	m->waypoints[m->waypoint_count++] = map_get_center(3,  6);
	m->waypoints[m->waypoint_count++] = map_get_center(4,  7);
	m->waypoints[m->waypoint_count++] = map_get_center(11, 7);
	m->waypoints[m->waypoint_count++] = map_get_center(11, 10);
	m->waypoints[m->waypoint_count++] = map_get_center(7,  15);
	m->waypoints[m->waypoint_count++] = map_get_center(7,  19);
	m->waypoints[m->waypoint_count++] = map_get_center(11, 24);
	m->waypoints[m->waypoint_count++] = map_get_center(13, 24);
	m->waypoints[m->waypoint_count++] = map_get_center(16, 21);
	m->waypoints[m->waypoint_count++] = map_get_center(16, 17);
	m->waypoints[m->waypoint_count++] = map_get_center(21, 17);
	
	/* ---- Path cells (edit to match your waypoints) ---- */
	m->grid[3][6] = CELL_TYPE_PATH;
	m->grid[4][7] = CELL_TYPE_PATH;
	for (i = 4; i <= 11; i++) m->grid[i][7]  = CELL_TYPE_PATH;
	for (j = 7; j <= 10; j++) m->grid[11][j] = CELL_TYPE_PATH;
	m->grid[11][10] = CELL_TYPE_PATH;  m->grid[10][11] = CELL_TYPE_PATH;
	m->grid[9][12]  = CELL_TYPE_PATH;  m->grid[9][13]  = CELL_TYPE_PATH;
	m->grid[8][14]  = CELL_TYPE_PATH;  m->grid[7][15]  = CELL_TYPE_PATH;
	for (j = 15; j <= 19; j++) m->grid[7][j]  = CELL_TYPE_PATH;
	m->grid[7][19]  = CELL_TYPE_PATH;  m->grid[8][20]  = CELL_TYPE_PATH;
	m->grid[9][21]  = CELL_TYPE_PATH;  m->grid[9][22]  = CELL_TYPE_PATH;
	m->grid[10][23] = CELL_TYPE_PATH;  m->grid[11][24] = CELL_TYPE_PATH;
	for (i = 11; i <= 13; i++) m->grid[i][24] = CELL_TYPE_PATH;
	m->grid[13][24] = CELL_TYPE_PATH;  m->grid[14][23] = CELL_TYPE_PATH;
	m->grid[15][22] = CELL_TYPE_PATH;  m->grid[16][21] = CELL_TYPE_PATH;
	for (j = 16; j <= 21; j++) m->grid[16][j] = CELL_TYPE_PATH;
	for (i = 16; i <= 21; i++) m->grid[i][16] = CELL_TYPE_PATH;
	
	/* ---- Buildable cells (edit as needed) ---- */
	for (i = 5; i <= 10; i++) { m->grid[i][4]=CELL_TYPE_BUILDABLE; m->grid[i][5]=CELL_TYPE_BUILDABLE; m->grid[i][6]=CELL_TYPE_BUILDABLE; }
	for (i = 5; i <= 10; i++) { m->grid[i][8]=CELL_TYPE_BUILDABLE; m->grid[i][9]=CELL_TYPE_BUILDABLE; }
	for (j = 14; j <= 20; j++) { m->grid[5][j]=CELL_TYPE_BUILDABLE; m->grid[6][j]=CELL_TYPE_BUILDABLE; }
	for (j = 16; j <= 19; j++) { m->grid[8][j]=CELL_TYPE_BUILDABLE; m->grid[9][j]=CELL_TYPE_BUILDABLE; }
	for (i = 11; i <= 13; i++) { m->grid[i][25]=CELL_TYPE_BUILDABLE; m->grid[i][26]=CELL_TYPE_BUILDABLE; }
	for (j = 17; j <= 21; j++) { m->grid[14][j]=CELL_TYPE_BUILDABLE; m->grid[15][j]=CELL_TYPE_BUILDABLE; }
	for (i = 17; i <= 20; i++) { m->grid[i][14]=CELL_TYPE_BUILDABLE; m->grid[i][15]=CELL_TYPE_BUILDABLE; }
	for (i = 17; i <= 20; i++) { m->grid[i][17]=CELL_TYPE_BUILDABLE; m->grid[i][18]=CELL_TYPE_BUILDABLE; }
}

/* ============================================================
* map_init
* ============================================================ */
void map_init(Map* m, int level) {
	int i, j;
	m->waypoint_count = 0;
	m->bg_img.id = 0; /* 背景图由 game 模块设置 / Background set by game module */
	
	/* 初始化所有格子为障碍物 / Init all cells as obstacles */
	for (i = 0; i < MAP_ROWS; i++)
		for (j = 0; j < MAP_COLS; j++)
			m->grid[i][j] = CELL_TYPE_OBSTACLE;
	
	/* Load terrain data for the specified level */
	if      (level == 1) load_level1_data(m);
	else if (level == 2) load_level2_data(m);
	else                 load_level3_data(m);
}

/* ============================================================
* map_get_center
* ============================================================ */
Point map_get_center(int row, int col) {
	Point p;
	p.x = col * CELL_SIZE + CELL_SIZE / 2;
	p.y = row * CELL_SIZE + CELL_SIZE / 2;
	return p;
}

/* ============================================================
* map_draw
* ============================================================ */
void map_draw(const Map* m) {
	int i, j;
	
	/* 背景图（若已加载）/ Background image (if loaded) */
	if (m->bg_img.id != 0) {
		int ox = (1024 - m->bg_img.width)  / 2;
		int oy = (768  - m->bg_img.height) / 2;
		DrawTexture(m->bg_img, ox, oy, WHITE);
	}
	
	/* 按住 SPACE 显示调试网格与路点 / Hold SPACE for debug grid + waypoints */
	if (IsKeyDown(KEY_SPACE)) {
		for (i = 0; i < MAP_ROWS; i++) {
			for (j = 0; j < MAP_COLS; j++) {
				int x = j * CELL_SIZE, y = i * CELL_SIZE;
				if      (m->grid[i][j] == CELL_TYPE_PATH)
					DrawRectangle(x, y, CELL_SIZE, CELL_SIZE, Fade(DARKBROWN, 0.5f));
				else if (m->grid[i][j] == CELL_TYPE_BUILDABLE)
					DrawRectangle(x, y, CELL_SIZE, CELL_SIZE, Fade(DARKGREEN, 0.5f));
				else
					DrawRectangle(x, y, CELL_SIZE, CELL_SIZE, Fade(DARKGRAY, 0.5f));
				DrawRectangleLines(x, y, CELL_SIZE, CELL_SIZE, Fade(BLACK, 0.2f));
			}
		}
		for (i = 0; i < m->waypoint_count - 1; i++) {
			Vector2 p1 = {(float)m->waypoints[i].x,   (float)m->waypoints[i].y};
			Vector2 p2 = {(float)m->waypoints[i+1].x, (float)m->waypoints[i+1].y};
			DrawLineEx(p1, p2, 3.0f, RED);
			DrawCircleV(p1, 4.0f, YELLOW);
		}
	}
}

/* ============================================================
* 获取器 / Getters
* ============================================================ */

const Point* map_get_waypoints(const Map* m) { return m->waypoints; }
int          map_get_waypoint_count(const Map* m) { return m->waypoint_count; }

int map_can_place_tower(const Map* m, int row, int col) {
	if (row < 0 || row >= MAP_ROWS || col < 0 || col >= MAP_COLS) return 0;
	return m->grid[row][col] == CELL_TYPE_BUILDABLE;
}

int map_is_path(const Map* m, int row, int col) {
	if (row < 0 || row >= MAP_ROWS || col < 0 || col >= MAP_COLS) return 0;
	return m->grid[row][col] == CELL_TYPE_PATH;
}

void map_place_tower(Map* m, int row, int col) {
	if (row >= 0 && row < MAP_ROWS && col >= 0 && col < MAP_COLS)
		m->grid[row][col] = CELL_TYPE_OBSTACLE;
}

void map_restore_cell(Map* m, int row, int col) {
	if (row >= 0 && row < MAP_ROWS && col >= 0 && col < MAP_COLS)
		m->grid[row][col] = CELL_TYPE_BUILDABLE;
}
