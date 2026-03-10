#include "Enemy.h"
#include <math.h>

Enemy::Enemy(Point startPos) {
	x = startPos.x;
	y = startPos.y;
	speed = 60.0;             
	maxHp = 100;              
	hp = maxHp;
	targetWaypointIndex = 1;  
	active = true;
	reachedBase = false;
	hitFlashTimer = 0.0; // 初始不闪烁
}

void Enemy::update(double deltaTime, const std::vector<Point>& waypoints) {
	if (!active) return;
	
	// 更新闪烁倒计时
	if (hitFlashTimer > 0) hitFlashTimer -= deltaTime;
	
	if (targetWaypointIndex < (int)waypoints.size()) {
		Point target = waypoints[targetWaypointIndex];
		double dx = target.x - x;
		double dy = target.y - y;
		double distance = sqrt(dx * dx + dy * dy);
		
		if (distance < 2.0) {
			targetWaypointIndex++;
		} else {
			double moveDist = speed * deltaTime;
			if (moveDist > distance) moveDist = distance; 
			x += (dx / distance) * moveDist;
			y += (dy / distance) * moveDist;
		}
	} else {
		active = false;
		reachedBase = true;
	}
}

void Enemy::draw() {
	if (!active) return;
	
	// 【特效】受击闪烁白光，否则是暗红色野兽
	if (hitFlashTimer > 0) {
		setfillcolor(EGERGB(255, 255, 255)); // 纯白闪烁
	} else {
		setfillcolor(EGERGB(150, 30, 30));   // 正常暗红
	}
	fillcircle((int)x, (int)y, 15);
	
	// 绘制血条
	int barWidth = 30;
	int currentBar = (int)((double)hp / maxHp * barWidth);
	setfillcolor(EGERGB(100, 0, 0));
	bar((int)x - 15, (int)y - 25, (int)x + 15, (int)y - 20);
	setfillcolor(EGERGB(0, 200, 0));
	bar((int)x - 15, (int)y - 25, (int)x - 15 + currentBar, (int)y - 20);
}
