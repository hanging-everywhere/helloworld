#ifndef TOWER_H
#define TOWER_H

#include "raylib.h"
#include "Enemy.h"

typedef struct {
	double x, y;
	int damage;
	double life;
} FloatingText;

typedef struct {
	double startX, startY;
	double targetX, targetY;
	double life;
	double maxLife;
} VisualProjectile;

typedef struct {
	int x, y;
	int range;
	int damage;
	double cooldown;
	double currentCooldown;
} Tower;

void Tower_init(Tower* t, int startX, int startY);
void Tower_update(Tower* t, double deltaTime, 
				  Enemy* enemies, int enemyCount, 
				  Point* waypoints, 
				  VisualProjectile* projs, int* projCount, 
				  FloatingText* fTexts, int* fTextCount);
void Tower_draw(Tower* t);

#endif
