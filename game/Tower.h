#ifndef TOWER_H
#define TOWER_H

#include <graphics.h>
#include "Enemy.h"

// 集中定义所有特效结构体
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

// 这里需要传入各个数组及其数量的指针
void Tower_init(Tower* t, int startX, int startY);
void Tower_update(Tower* t, double deltaTime, 
				  Enemy* enemies, int enemyCount, 
				  Point* waypoints, 
				  VisualProjectile* projs, int* projCount, 
				  FloatingText* fTexts, int* fTextCount);
void Tower_draw(Tower* t);

#endif
