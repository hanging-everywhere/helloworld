#ifndef TOWER_H
#define TOWER_H

#include "raylib.h"
#include "Enemy.h"

enum TowerType {
	TOWER_NONE = 0,
	// 石器时代
	TOWER_SLING=1, TOWER_BONESPEAR=2, TOWER_TOTEM=3, TOWER_FIRE=4,
	// 古代文明
	TOWER_CROSSBOW=5, TOWER_MINE=6, TOWER_CAULDRON=7, TOWER_FIRECROW=8,
	// 科幻未来
	TOWER_PULSE=9, TOWER_GRAVITY=10, TOWER_PORTAL=11, TOWER_TIMECLOCK=12, TOWER_LASER=13
};

typedef struct {
	double x, y;
	int    damage;
	double life;
} FloatingText;

typedef struct {
	double startX, startY;
	double targetX, targetY;
	double life, maxLife;
	int    isHit, damage, targetId;
	int    isCrow;  // 1=飞鸦弹（抛物线+落地燃烧）
} VisualProjectile;

// 燃烧区域（飞鸦落地用）
typedef struct {
	double x, y;
	double life;       // 剩余燃烧时间（4秒）
	double tickTimer;  // 每秒跳一次
	int    damage;     // 每秒伤害
	int    active;
} BurnZone;

typedef struct {
	int x, y;
	int era, type;
	int levelA, levelB;
	int range, damage;
	double cooldown, currentCooldown;
	int    ammo, state;
	double timer1, timer2;
	int    targetId;
	int    pairedIndex;
	double buffTimer, buffSpeedRatio;
	Rectangle frameRec;
	int currentFrame, framesCounter, framesSpeed, maxFrames;
	
	// 诸葛连弩：暴击计数（A路专精）
	int hitCountOnTarget, lastTargetId;
	
	// 震天雷：自动填充计时（B路）
	double reloadTimer;
	
	// 炼丹炉：炸药配方剩余次数 + 伤害倍率（B路）
	int    explosiveCharges;
	double explosiveDmgMult;
	
	// 神火飞鸦：飞鸦独立冷却
	double crowCooldown, crowCurrentCooldown;
	
	// 脉冲塔：手动蓄力
	double chargeTime;
	int    isCharging;
	
	// 重力塔：A2引爆就绪
	int gravityExplodeReady;
	
	// 传送门：角色(0=A门/1=B门)，已传送计数，坍缩冷却
	int    portalRole;
	int    portalTeleCount;
	double portalCollapseCD;
	
	// 时间塔：时间波持续计时
	double timeWaveTimer;
	
	// 激光塔：光束持续时间，储能数，超级脉冲冷却
	double laserBeamTimer;
	int    laserCharges;
	double laserPulseCooldown;
	
} Tower;

void Tower_init(Tower* t, int startX, int startY, int era, int type);

void Tower_update(Tower* t, double deltaTime,
				  Enemy* enemies, int enemyCount,
				  Tower* allTowers, int towerCount,
				  Point* waypoints,
				  VisualProjectile* projs, int* projCount,
				  FloatingText* fTexts, int* fTextCount,
				  int* playerWood, int* playerStone,
				  int* playerMetal, int* playerMaterial,
				  BurnZone* burnZones, int* burnZoneCount);

// towerTextures[0..12] 对应 TOWER_SLING..TOWER_LASER
void Tower_draw(Tower* t, Texture2D* towerTextures);
void Tower_drawLaserBeam(Tower* t, Enemy* enemies, int enemyCount);

#endif
