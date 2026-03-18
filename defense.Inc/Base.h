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
	
	int levelA;   // 石器A路等级（0/1/2）
	int levelB;   // 预留，暂不使用
	
	// 石器A2：自带火攻
	double attackCooldown;
	double attackTimer;
	int    attackDamage;
	int    attackRange;
	
	double animTimer;
} Base;

void Base_init(Base* b, int x, int y, int era);

void Base_update(Base* b, double deltaTime,
				 Enemy* enemies, int enemyCount,
				 FloatingText* fTexts, int* fTextCount,
				 VisualProjectile* projs, int* projCount);

void Base_draw(Base* b);

// 返回 0=成功 1=资源不足 2=满级/不可用
int Base_upgrade(Base* b, int path, int* wood, int* stone, int* metal, int* material);

void Base_getUpgradeLabel(Base* b, int path, const char** outLabel, int* outAffordable,
						  int wood, int stone, int metal, int material);

#endif
