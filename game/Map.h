#ifndef MAP_H
#define MAP_H
#include <graphics.h>
#include <vector>

const int CELL_SIZE = 64;  
const int ROWS = 12;       
const int COLS = 16;       

struct Point { int x, y; };

class Map {
public:
	int grid[ROWS][COLS];         
	std::vector<Point> waypoints; 
	
	Map();
	void init();
	void draw();
	Point getCenter(int row, int col);
};
#endif
