#include "Tower.h"
#include "GameManager.h" // 引入为了使用结构体
#include <math.h>

Tower::Tower(int startX, int startY) {
	x = startX;
	y = startY;
	range = 150;            
	damage = 35;            
	cooldown = 1.0;         
	currentCooldown = 0.0;  
}

void Tower::update(double deltaTime, std::vector<Enemy>& enemies, const std::vector<Point>& waypoints, 
				   std::vector<VisualProjectile>& projectiles, std::vector<FloatingText>& floatTexts) {
	if (currentCooldown > 0) currentCooldown -= deltaTime;
	
	if (currentCooldown <= 0) {
		int bestEnemyIdx = -1;
		double maxProgress = -1.0; 
		
		for (size_t i = 0; i < enemies.size(); i++) {
			if (enemies[i].active) {
				double dx = enemies[i].x - x;
				double dy = enemies[i].y - y;
				double distanceToTower = sqrt(dx * dx + dy * dy);
				
				if (distanceToTower <= range) {
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
		
		// 找到目标后
		if (bestEnemyIdx != -1) {
			enemies[bestEnemyIdx].hp -= damage;
			enemies[bestEnemyIdx].hitFlashTimer = 0.1; // 触发白光闪烁
			currentCooldown = cooldown;
			
			// 【特效】生成空中飞行的抛物线石块 (飞行0.2秒)
			VisualProjectile proj = {(double)x, (double)y, enemies[bestEnemyIdx].x, enemies[bestEnemyIdx].y, 0.2, 0.2};
			projectiles.push_back(proj);
			
			// 【特效】生成飘字
			FloatingText ft = {enemies[bestEnemyIdx].x, enemies[bestEnemyIdx].y - 20, damage, 0.8};
			floatTexts.push_back(ft);
		}
	}
}

void Tower::draw() {
	// 粗糙的石堆色
	setfillcolor(EGERGB(100, 100, 100));
	bar(x - 20, y - 20, x + 20, y + 20);
	// 塔顶的深色射击孔
	setfillcolor(EGERGB(40, 40, 40));
	fillcircle(x, y, 8);
}
