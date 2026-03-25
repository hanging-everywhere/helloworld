#ifndef ENEMY_H
#define ENEMY_H

#include "raylib.h"
#include "Map.h"

#define SKELETON_FRAME_WIDTH  32
#define SKELETON_FRAME_HEIGHT 32
#define SKELETON_WALK_FRAMES   3

typedef struct {
	int    id;
	double x, y;
	
	double baseSpeed;
	double currentSpeed;
	
	int    maxHp;
	int    hp;
	int    targetWaypointIndex;
	int    active;
	int    reachedBase;
	
	// 动画
	double    hitFlashTimer;
	Rectangle frameRec;
	int       currentFrame, framesCounter, framesSpeed;
	int       isInvisible;
	
	// 异常状态
	double stunTimer;
	double slowTimer;
	double slowRatio;
	double dotTimer, dotTickTimer;
	int    dotDamage;
	
	// 传送门：记录已被传送次数（基础最多1次，B1升级后最多2次）
	int    portalCount;
	
	// 时间塔 A2：位置快照（每3秒更新一次，命中后回溯到此处）
	double savedX, savedY;
	int    savedWaypointIndex;
	double posRecordTimer;
	
	// 时间塔 B1：因果律标记
	int    causeMarkActive;
	double causeTimer;
	int    causeDamageAccum;
	
} Enemy;

void Enemy_init(Enemy* e, Point startPos, int id);
void Enemy_update(Enemy* e, double deltaTime, Point* waypoints, int waypointCount);
void Enemy_draw(Enemy* e, Texture2D spriteSheet);

#endif
