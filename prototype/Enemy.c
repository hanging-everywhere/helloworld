#include "Enemy.h"
#include <math.h>

void Enemy_init(Enemy* e, Point startPos) {
	e->x = startPos.x;
	e->y = startPos.y;
	e->speed = 60.0;             
	e->maxHp = 100;              
	e->hp = e->maxHp;
	e->targetWaypointIndex = 1;  
	e->active = 1;
	e->reachedBase = 0;
	e->hitFlashTimer = 0.0; 
}

void Enemy_update(Enemy* e, double deltaTime, Point* waypoints, int waypointCount) {
	if (!e->active) return;
	
	if (e->hitFlashTimer > 0) e->hitFlashTimer -= deltaTime;
	
	if (e->targetWaypointIndex < waypointCount) {
		Point target = waypoints[e->targetWaypointIndex];
		double dx = target.x - e->x;
		double dy = target.y - e->y;
		double distance = sqrt(dx * dx + dy * dy);
		
		if (distance < 2.0) {
			e->targetWaypointIndex++;
		} else {
			double moveDist = e->speed * deltaTime;
			if (moveDist > distance) moveDist = distance; 
			e->x += (dx / distance) * moveDist;
			e->y += (dy / distance) * moveDist;
		}
	} else {
		e->active = 0;
		e->reachedBase = 1;
	}
}

void Enemy_draw(Enemy* e) {
	if (!e->active) return;
	
	// 受击闪烁白光，否则是暗红色
	Color enemyColor = (e->hitFlashTimer > 0) ? WHITE : (Color){150, 30, 30, 255};
	DrawCircle((int)e->x, (int)e->y, 15, enemyColor);
	
	// 绘制血条
	int barWidth = 30;
	int currentBar = (int)((double)e->hp / e->maxHp * barWidth);
	DrawRectangle((int)e->x - 15, (int)e->y - 25, barWidth, 5, (Color){100, 0, 0, 255}); // 底色
	DrawRectangle((int)e->x - 15, (int)e->y - 25, currentBar, 5, (Color){0, 200, 0, 255}); // 当前血量
}
