#include "Tower.h"
#include <math.h>

void Tower_init(Tower* t, int startX, int startY) {
	t->x = startX;
	t->y = startY;
	t->range = 150;            
	t->damage = 35;            
	t->cooldown = 1.0;         
	t->currentCooldown = 0.0;  
}

void Tower_update(Tower* t, double deltaTime, 
				  Enemy* enemies, int enemyCount, 
				  Point* waypoints, 
				  VisualProjectile* projs, int* projCount, 
				  FloatingText* fTexts, int* fTextCount) {
	
	if (t->currentCooldown > 0) t->currentCooldown -= deltaTime;
	
	if (t->currentCooldown <= 0) {
		int bestEnemyIdx = -1;
		double maxProgress = -1.0; 
		int i;
		
		for (i = 0; i < enemyCount; i++) {
			if (enemies[i].active) {
				double dx = enemies[i].x - t->x;
				double dy = enemies[i].y - t->y;
				double distanceToTower = sqrt(dx * dx + dy * dy);
				
				if (distanceToTower <= t->range) {
					double targetDx = waypoints[enemies[i].targetWaypointIndex].x - enemies[i].x;
					double targetDy = waypoints[enemies[i].targetWaypointIndex].y - enemies[i].y;
					double distToNextWaypoint = sqrt(targetDx * targetDx + targetDy * targetDy);
					double progressScore = enemies[i].targetWaypointIndex * 10000.0 - distToNextWaypoint;
					
					if (progressScore > maxProgress) {
						maxProgress = progressScore;
						bestEnemyIdx = i;
					}
				}
			}
		}
		
		if (bestEnemyIdx != -1) {
			enemies[bestEnemyIdx].hp -= t->damage;
			enemies[bestEnemyIdx].hitFlashTimer = 0.1; 
			t->currentCooldown = t->cooldown;
			
			// 发射抛物线石块
			projs[*projCount].startX = (double)t->x;
			projs[*projCount].startY = (double)t->y;
			projs[*projCount].targetX = enemies[bestEnemyIdx].x;
			projs[*projCount].targetY = enemies[bestEnemyIdx].y;
			projs[*projCount].life = 0.2;
			projs[*projCount].maxLife = 0.2;
			(*projCount)++;
			
			// 生成伤害飘字
			fTexts[*fTextCount].x = enemies[bestEnemyIdx].x;
			fTexts[*fTextCount].y = enemies[bestEnemyIdx].y - 20;
			fTexts[*fTextCount].damage = t->damage;
			fTexts[*fTextCount].life = 0.8;
			(*fTextCount)++;
		}
	}
}

void Tower_draw(Tower* t) {
	// 粗糙的石堆色方形
	DrawRectangle(t->x - 20, t->y - 20, 40, 40, (Color){100, 100, 100, 255});
	// 塔顶深色射击孔
	DrawCircle(t->x, t->y, 8, (Color){40, 40, 40, 255});
}
