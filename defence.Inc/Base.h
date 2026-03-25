#ifndef BASE_H
#define BASE_H

#include "raylib.h"
#include "Enemy.h"
#include "Tower.h"

typedef struct {
	int x, y;
	int era;
	int maxHp;
	int hp;
	
	int levelA;
	int levelB;
	
	// 石器/古代 A2：自带火攻 / 火炮
	double attackCooldown;
	double attackTimer;
	int    attackDamage;
	int    attackRange;
	
	// 古代 A1：免费建造次数
	int    freeBuildCount;   // 剩余免费建造次数（最多3）
	
	// 古代 A2：陷阱盾（抵挡入侵次数）
	int    trapShield;       // 剩余可抵挡次数（最多3）
	
	// 未来 A1：最后一滴血无敌
	int    invincible;       // 1=无敌中
	double invincibleTimer;  // 无敌剩余时间
	int    invincibleUsed;   // 已触发过（只触发一次）
	
	// 未来 A2：卫星轨道炮
	double satelliteCooldown;   // 卫星攻击间隔（固定2秒）
	double satelliteTimer;      // 当前计时
	
	double animTimer;
} Base;

void Base_init(Base* b, int x, int y, int era);

void Base_update(Base* b, double deltaTime,
				 Enemy* enemies, int enemyCount,
				 FloatingText* fTexts, int* fTextCount,
				 VisualProjectile* projs, int* projCount);

// 返回值：0=成功 1=资源不足 2=满级/不可用
// 同时处理三个时代
int Base_upgrade(Base* b, int path, int* wood, int* stone, int* metal, int* material);

void Base_getUpgradeLabel(Base* b, int path, const char** outLabel, int* outAffordable,
						  int wood, int stone, int metal, int material);

void Base_draw(Base* b);

// 古代A1：消耗一次免费建造机会（由GameManager调用）
// 返回1表示本次建造免费，0表示没有机会了
int Base_consumeFreeBuild(Base* b);

// 古代A2：受到一次入侵时调用，陷阱盾拦截返回1（伤害取消），否则返回0
int Base_consumeTrapShield(Base* b);

// 未来A1：受到攻击时调用，如果触发无敌返回1
int Base_tryInvincible(Base* b);

#endif
