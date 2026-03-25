#ifndef MAP_H
#define MAP_H

#include "raylib.h"

// 保持 32x32 的精细网格划分
#define CELL_SIZE 32     
#define ROWS 24          
#define COLS 32          
#define MAX_WAYPOINTS 40 

// 地形属性定义
#define TYPE_PATH 0       
#define TYPE_BUILDABLE 1  
#define TYPE_OBSTACLE 2   

typedef struct { 
	int x, y; 
} Point;

//结构体包含某一游戏组分的全部信息
typedef struct {
	int grid[ROWS][COLS];         
	Point waypoints[MAX_WAYPOINTS]; 
	int waypointCount;              
	Texture2D bgImg;  
} Map;

void Map_init(Map* m,int level);
void Map_draw(Map* m);
Point Map_getCenter(int row, int col);

#endif
