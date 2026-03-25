#include "Enemy.h"
#include <math.h>

void Enemy_init(Enemy* e, Point startPos, int id) {
	e->id = id;
	e->x  = startPos.x;
	e->y  = startPos.y;
	
	e->baseSpeed    = 60.0;
	e->currentSpeed = e->baseSpeed;
	
	e->maxHp = 100;
	e->hp    = e->maxHp;
	e->targetWaypointIndex = 1;
	e->active      = 1;
	e->reachedBase = 0;
	
	e->hitFlashTimer = 0.0;
	e->stunTimer     = 0.0;
	e->slowTimer     = 0.0;
	e->slowRatio     = 1.0;
	e->dotTimer      = 0.0;
	e->dotTickTimer  = 0.0;
	e->dotDamage     = 0;
	e->isInvisible   = 0;
	
	e->currentFrame  = 0;
	e->framesCounter = 0;
	e->framesSpeed   = 15;
	e->frameRec = (Rectangle){ 0.0f, 0.0f, SKELETON_FRAME_WIDTH, SKELETON_FRAME_HEIGHT };
	
	// 传送门
	e->portalCount = 0;
	
	// 时间塔 A2 快照（初始化为出生点）
	e->savedX               = startPos.x;
	e->savedY               = startPos.y;
	e->savedWaypointIndex   = 1;
	e->posRecordTimer       = 0.0;
	
	// 时间塔 B1 因果律
	e->causeMarkActive  = 0;
	e->causeTimer       = 0.0;
	e->causeDamageAccum = 0;
}

void Enemy_update(Enemy* e, double deltaTime, Point* waypoints, int waypointCount) {
	float  dx, dy, dist;
	double moveDist;
	
	if (!e->active) return;
	
	// --- 1. 异常状态倒计时 ---
	if (e->hitFlashTimer > 0) e->hitFlashTimer -= deltaTime;
	
	// 因果律：标记倒计时，到期复现伤害
	if (e->causeMarkActive) {
		e->causeTimer -= deltaTime;
		if (e->causeTimer <= 0) {
			e->causeMarkActive  = 0;
			e->hp              -= e->causeDamageAccum;
			e->causeDamageAccum = 0;
			e->hitFlashTimer    = 0.15;
		}
	}
	
	// 眩晕
	if (e->stunTimer > 0) {
		e->stunTimer -= deltaTime;
		return;
	}
	
	// 减速
	if (e->slowTimer > 0) {
		e->slowTimer    -= deltaTime;
		e->currentSpeed  = e->baseSpeed * e->slowRatio;
	} else {
		e->currentSpeed = e->baseSpeed;
	}
	
	// DoT
	if (e->dotTimer > 0) {
		e->dotTimer     -= deltaTime;
		e->dotTickTimer -= deltaTime;
		if (e->dotTickTimer <= 0) {
			// 因果律累计
			if (e->causeMarkActive) e->causeDamageAccum += e->dotDamage;
			e->hp           -= e->dotDamage;
			e->hitFlashTimer = 0.1;
			e->dotTickTimer  = 1.0;
		}
	}
	
	// --- 2. 位置快照更新（时间塔 A2 用）---
	e->posRecordTimer += deltaTime;
	if (e->posRecordTimer >= 3.0) {
		e->posRecordTimer       = 0.0;
		e->savedX               = e->x;
		e->savedY               = e->y;
		e->savedWaypointIndex   = e->targetWaypointIndex;
	}
	
	// --- 3. 移动 ---
	if (e->targetWaypointIndex < waypointCount) {
		Point target = waypoints[e->targetWaypointIndex];
		dx   = (float)target.x - e->x;
		dy   = (float)target.y - e->y;
		dist = sqrt(dx*dx + dy*dy);
		if (dist < 2.0f) {
			e->targetWaypointIndex++;
		} else {
			moveDist = e->currentSpeed * deltaTime;
			if (moveDist > dist) moveDist = dist;
			e->x += (dx / dist) * moveDist;
			e->y += (dy / dist) * moveDist;
		}
	} else {
		e->active      = 0;
		e->reachedBase = 1;
	}
	
	// --- 4. 动画 ---
	e->framesCounter++;
	if (e->framesCounter >= e->framesSpeed) {
		e->framesCounter = 0;
		e->currentFrame++;
		if (e->currentFrame >= SKELETON_WALK_FRAMES) e->currentFrame = 0;
		e->frameRec.x = (float)e->currentFrame * SKELETON_FRAME_WIDTH;
	}
}

void Enemy_draw(Enemy* e, Texture2D spriteSheet) {
	int     barWidth, currentBar;
	Color   drawColor;
	Vector2 origin;
	Rectangle destRec;
	
	if (!e->active) return;
	
	drawColor = (e->hitFlashTimer > 0) ? RED : WHITE;
	if (e->slowTimer     > 0 && e->hitFlashTimer <= 0) drawColor = SKYBLUE;
	if (e->causeMarkActive && e->hitFlashTimer  <= 0)  drawColor = PURPLE; // 因果标记紫色
	
	origin  = (Vector2){ SKELETON_FRAME_WIDTH / 2.0f, SKELETON_FRAME_HEIGHT / 2.0f };
	destRec = (Rectangle){ (float)e->x, (float)e->y, SKELETON_FRAME_WIDTH, SKELETON_FRAME_HEIGHT };
	DrawTexturePro(spriteSheet, e->frameRec, destRec, origin, 0.0f, drawColor);
	
	barWidth   = 30;
	currentBar = (e->maxHp > 0) ? (int)((double)e->hp / e->maxHp * barWidth) : 0;
	if (currentBar < 0) currentBar = 0;
	DrawRectangle((int)e->x - 15, (int)e->y + 20, barWidth,    4, (Color){100,0,0,255});
	DrawRectangle((int)e->x - 15, (int)e->y + 20, currentBar,  4, (Color){0,200,0,255});
	
	if (e->stunTimer      > 0) DrawText("Zzz",  (int)e->x - 12, (int)e->y - 35, 14, YELLOW);
	if (e->causeMarkActive)    DrawText("FATE", (int)e->x - 14, (int)e->y - 48, 10, PURPLE);
}
