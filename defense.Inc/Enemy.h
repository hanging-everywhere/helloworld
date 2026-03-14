#ifndef ENEMY_H
#define ENEMY_H

#include "raylib.h"
#include "Map.h"

#define SKELETON_FRAME_WIDTH 32   
#define SKELETON_FRAME_HEIGHT 32  
#define SKELETON_WALK_FRAMES 3    

typedef struct {
	int id;                  // 【新增】唯一标识符（方便激光塔、因果塔持续锁定）
	double x, y;             
	
	double baseSpeed;        // 【新增】基础速度
	double currentSpeed;     // 【新增】实际速度（受减速/加速影响）
	
	int maxHp;               
	int hp;                  
	int targetWaypointIndex; 
	int active;             
	int reachedBase;        
	
	// --- 动画视觉相关 ---
	double hitFlashTimer;    // 受击闪红
	Rectangle frameRec;     
	int currentFrame;       
	int framesCounter;      
	int framesSpeed;        
	
	// --- 【核心新增：异常状态系统 (Debuffs)】 ---
	double stunTimer;        // 眩晕倒计时：大于0时无法移动
	double slowTimer;        // 减速倒计时：大于0时触发减速
	double slowRatio;        // 减速系数：比如 0.5 就是减速 50%
	
	double dotTimer;         // 持续伤害(DoT)倒计时：比如流血、燃烧
	double dotTickTimer;     // 持续伤害的“跳字”间隔 (通常每秒跳一次)
	int dotDamage;           // 每次跳字的伤害值
	
} Enemy;

void Enemy_init(Enemy* e, Point startPos, int id);
void Enemy_update(Enemy* e, double deltaTime, Point* waypoints, int waypointCount);
void Enemy_draw(Enemy* e, Texture2D spriteSheet); 

#endif
