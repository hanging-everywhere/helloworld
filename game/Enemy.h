#ifndef ENEMY_H
#define ENEMY_H
#include <graphics.h>
#include <vector>
#include "Map.h"

class Enemy {
public:
	double x, y;             
	double speed;            
	int maxHp;               
	int hp;                  
	int targetWaypointIndex; 
	bool active;             
	bool reachedBase;        
	
	// 【新增】受击闪烁计时器
	double hitFlashTimer;    
	
	Enemy(Point startPos);
	void update(double deltaTime, const std::vector<Point>& waypoints);
	void draw();
};
#endif
