#ifndef ENEMY_H
#define ENEMY_H

#include "raylib.h"
#include "Map.h"

typedef struct {
	double x, y;             
	double speed;            
	int maxHp;               
	int hp;                  
	int targetWaypointIndex; 
	int active;             
	int reachedBase;        
	double hitFlashTimer;    
} Enemy;

void Enemy_init(Enemy* e, Point startPos);
void Enemy_update(Enemy* e, double deltaTime, Point* waypoints, int waypointCount);
void Enemy_draw(Enemy* e);

#endif
