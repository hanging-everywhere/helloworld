#ifndef MAP_H
#define MAP_H

#include <graphics.h>

#define CELL_SIZE 64
#define ROWS 12
#define COLS 16
#define MAX_WAYPOINTS 20

// 定义坐标结构体
typedef struct { 
	int x, y; 
} Point;

// 将类改为 C 语言结构体
typedef struct {
	int grid[ROWS][COLS];         
	Point waypoints[MAX_WAYPOINTS]; // 静态数组取代 vector
	int waypointCount;              // 记录路点数量
} Map;

// C 语言风格的函数声明，必须传入结构体指针
void Map_init(Map* m);
void Map_draw(Map* m);
Point Map_getCenter(int row, int col);

#endif
