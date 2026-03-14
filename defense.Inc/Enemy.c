#include "Enemy.h"
#include <math.h>

void Enemy_init(Enemy* e, Point startPos, int id) {
	e->id = id;
	e->x = startPos.x;
	e->y = startPos.y;
	
	e->baseSpeed = 60.0;     
	e->currentSpeed = e->baseSpeed; 
	
	e->maxHp = 100;              
	e->hp = e->maxHp;
	e->targetWaypointIndex = 1;  
	e->active = 1;
	e->reachedBase = 0;
	
	// 初始化所有异常状态为空
	e->hitFlashTimer = 0.0; 
	e->stunTimer = 0.0;
	e->slowTimer = 0.0;
	e->slowRatio = 1.0;
	e->dotTimer = 0.0;
	e->dotTickTimer = 0.0;
	e->dotDamage = 0;
	
	e->currentFrame = 0;
	e->framesCounter = 0;
	e->framesSpeed = 15; 
	e->frameRec = (Rectangle){ 0.0f, 0.0f, SKELETON_FRAME_WIDTH, SKELETON_FRAME_HEIGHT };
}

void Enemy_update(Enemy* e, double deltaTime, Point* waypoints, int waypointCount) {
	float dx, dy, dist;
	double moveDist;
	
	if (!e->active) return;
	
	// --- 1. 处理异常状态倒计时 ---
	if (e->hitFlashTimer > 0) e->hitFlashTimer -= deltaTime;
	
	// 处理眩晕
	if (e->stunTimer > 0) {
		e->stunTimer -= deltaTime;
		return; // 眩晕状态下直接 return，不执行移动和动画！
	}
	
	// 处理减速
	if (e->slowTimer > 0) {
		e->slowTimer -= deltaTime;
		e->currentSpeed = e->baseSpeed * e->slowRatio;
	} else {
		e->currentSpeed = e->baseSpeed; // 恢复正常速度
	}
	
	// 处理持续伤害 (DoT)
	if (e->dotTimer > 0) {
		e->dotTimer -= deltaTime;
		e->dotTickTimer -= deltaTime;
		if (e->dotTickTimer <= 0) {
			e->hp -= e->dotDamage; 
			e->hitFlashTimer = 0.1; 
			e->dotTickTimer = 1.0; 
		}
	}
	
	// --- 2. 移动逻辑 ---
	if (e->targetWaypointIndex < waypointCount) {
		Point target = waypoints[e->targetWaypointIndex];
		dx = (float)target.x - e->x;
		dy = (float)target.y - e->y;
		dist = sqrt(dx*dx + dy*dy);
		
		if (dist < 2.0) {
			e->targetWaypointIndex++;
		} else {
			moveDist = e->currentSpeed * deltaTime; 
			// 【修复】：把 distance 改成了 dist
			if (moveDist > dist) moveDist = dist; 
			e->x += (dx / dist) * moveDist;
			e->y += (dy / dist) * moveDist;
		}
	} else {
		e->active = 0;
		e->reachedBase = 1;
	}
	
	// --- 3. 动画更新逻辑 ---
	e->framesCounter++; 
	if (e->framesCounter >= e->framesSpeed) {
		e->framesCounter = 0; 
		e->currentFrame++;     
		if (e->currentFrame >= SKELETON_WALK_FRAMES) e->currentFrame = 0; 
		e->frameRec.x = (float)e->currentFrame * SKELETON_FRAME_WIDTH;
	}
}

void Enemy_draw(Enemy* e, Texture2D spriteSheet) {
	int barWidth;
	int currentBar;
	Color drawColor;
	Vector2 origin;
	Rectangle destRec;
	
	if (!e->active) return;
	
	// 如果中毒变绿，如果烧伤变橙，如果是受击变红...这里先保留受击变红
	drawColor = (e->hitFlashTimer > 0) ? RED : WHITE;
	
	// 如果被减速了，可以给个蓝色调
	if (e->slowTimer > 0 && e->hitFlashTimer <= 0) drawColor = SKYBLUE;
	
	origin = (Vector2){ SKELETON_FRAME_WIDTH / 2.0f, SKELETON_FRAME_HEIGHT / 2.0f };
	destRec = (Rectangle){ (float)e->x, (float)e->y, SKELETON_FRAME_WIDTH, SKELETON_FRAME_HEIGHT };
	
	DrawTexturePro(spriteSheet, e->frameRec, destRec, origin, 0.0f, drawColor);
	
	barWidth = 30;
	currentBar = (int)((double)e->hp / e->maxHp * barWidth);
	DrawRectangle((int)e->x - 15, (int)e->y + 20, barWidth, 4, (Color){100, 0, 0, 255}); 
	DrawRectangle((int)e->x - 15, (int)e->y + 20, currentBar, 4, (Color){0, 200, 0, 255}); 
}
