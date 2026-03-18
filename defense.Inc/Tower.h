#ifndef TOWER_H
#define TOWER_H

#include "raylib.h"
#include "Enemy.h"

enum TowerType {
	TOWER_NONE = 0,
	TOWER_SLING = 1,
	TOWER_BONESPEAR,
	TOWER_TOTEM,
	TOWER_FIRE
};

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
	int isHit;
	int damage;
	int targetId;
} VisualProjectile;

typedef struct {
	int x, y;
	
	int era;
	int type;
	int levelA;
	int levelB;
	
	int range;
	int damage;
	double cooldown;
	double currentCooldown;
	
	int ammo;
	int state;
	double timer1;
	double timer2;
	int targetId;
	int pairedIndex;
	
	// --- 【全局 Buff 状态系统】 ---
	double buffTimer;      
	double buffSpeedRatio; 
	
	// --- 【新增：序列帧动画系统】 ---
	Rectangle frameRec;     // 动画取景框 (用来裁剪出精灵图上当前需要显示的那一帧)
	int currentFrame;       // 当前播放到了第几帧 (0, 1, 2, 3)
	int framesCounter;      // 计时器：用来控制动画播放速度
	int framesSpeed;        // 动画播放速度：数字越大，动画越慢 (15代表每15个游戏帧切换一次图片)
	int maxFrames;          // 这张贴图总共有多少帧 (本例中为4)
	
} Tower;

void Tower_init(Tower* t, int startX, int startY, int era, int type);

void Tower_update(Tower* t, double deltaTime, 
				  Enemy* enemies, int enemyCount, 
				  Tower* allTowers, int towerCount, 
				  Point* waypoints, 
				  VisualProjectile* projs, int* projCount, 
				  FloatingText* fTexts, int* fTextCount,
				  int* playerWood, int* playerStone);

// --- 【修改】：接收塔的贴图作为参数进行绘制 ---
void Tower_draw(Tower* t, Texture2D slingTex, Texture2D spearTex);

#endif
