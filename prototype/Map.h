#ifndef MAP_H
#define MAP_H

#include "raylib.h"

#define CELL_SIZE 64
#define ROWS 12
#define COLS 16
#define MAX_WAYPOINTS 20

typedef struct { 
	int x, y; 
} Point;

typedef struct {
	int grid[ROWS][COLS];         
	Point waypoints[MAX_WAYPOINTS]; 
	int waypointCount;              
} Map;

void Map_init(Map* m);
void Map_draw(Map* m);
Point Map_getCenter(int row, int col);

#endif
