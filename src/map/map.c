/**
 * @file    map.c
 * @brief   地图模块实现（迁移自 defense.Inc/Map.c，已适配新 API）
 *          Map module implementation (migrated from defense.Inc/Map.c, adapted to new API).
 */

#include "map.h"

void map_init(Map* m) {
    int i, j;
    m->waypoint_count = 0;

    /* 初始化所有格子为草地 / Initialize all cells as grass */
    for (i = 0; i < MAP_ROWS; i++) {
        for (j = 0; j < MAP_COLS; j++) {
            m->grid[i][j] = CELL_GRASS;
        }
    }

    /* S 型泥土路径 / S-shaped dirt path */
    for (j = 0; j <= 12; j++)   m->grid[2][j]  = CELL_PATH;
    for (i = 3; i <= 8; i++)    m->grid[i][12]  = CELL_PATH;
    for (j = 11; j >= 3; j--)   m->grid[8][j]   = CELL_PATH;
    for (i = 9; i <= 11; i++)   m->grid[i][3]   = CELL_PATH;

    /* 路点（像素中心坐标）/ Waypoints (pixel center coords) */
    m->waypoints[m->waypoint_count++] = map_get_center(2,  0);
    m->waypoints[m->waypoint_count++] = map_get_center(2,  12);
    m->waypoints[m->waypoint_count++] = map_get_center(8,  12);
    m->waypoints[m->waypoint_count++] = map_get_center(8,  3);
    m->waypoints[m->waypoint_count++] = map_get_center(11, 3);
}

Point map_get_center(int row, int col) {
    Point p;
    p.x = col * CELL_SIZE + CELL_SIZE / 2;
    p.y = row * CELL_SIZE + CELL_SIZE / 2;
    return p;
}

void map_draw(const Map* m) {
    int i, j;
    for (i = 0; i < MAP_ROWS; i++) {
        for (j = 0; j < MAP_COLS; j++) {
            int x = j * CELL_SIZE;
            int y = i * CELL_SIZE;

            if (m->grid[i][j] == CELL_PATH) {
                /* 黑夜泥土路 / Night dirt path */
                DrawRectangle(x, y, CELL_SIZE, CELL_SIZE,
                              (Color){50, 40, 30, 255});
            } else {
                /* 黑夜草地 / Night grass */
                DrawRectangle(x, y, CELL_SIZE, CELL_SIZE,
                              (Color){15, 35, 20, 255});
                DrawRectangleLines(x, y, CELL_SIZE, CELL_SIZE,
                                   (Color){20, 50, 30, 255});
            }
        }
    }
}

const Point* map_get_waypoints(const Map* m) {
    return m->waypoints;
}

int map_get_waypoint_count(const Map* m) {
    return m->waypoint_count;
}

int map_can_place_tower(const Map* m, int row, int col) {
    if (row < 0 || row >= MAP_ROWS || col < 0 || col >= MAP_COLS) return 0;
    return m->grid[row][col] == CELL_GRASS;
}

void map_place_tower(Map* m, int row, int col) {
    if (row >= 0 && row < MAP_ROWS && col >= 0 && col < MAP_COLS) {
        m->grid[row][col] = CELL_TOWER;
    }
}
