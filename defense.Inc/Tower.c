#include "Tower.h"
#include <math.h>

void Tower_init(Tower* t, int startX, int startY, int era, int type) {
	t->x = startX;
	t->y = startY;
	t->era = era;
	t->type = type;
	t->levelA = 0;
	t->levelB = 0;
	
	t->currentCooldown = 0.0;
	t->timer1 = 0.0;
	t->timer2 = 0.0;
	t->state = 0;
	t->targetId = -1;
	
	// 根据塔的类型，赋予初始属性
	switch(type) {
		case TOWER_SLING: // 投石索
		t->range = 150;            
		t->damage = 35;            
		t->cooldown = 1.0; 
		t->ammo = 12; // 设定12发弹药限制！
		break;
		case TOWER_TOTEM: // 图腾
		t->range = 200;
		t->damage = 0; // 纯控制，无直接伤害
		t->cooldown = 7.0; // 7秒一次怒吼
		break;
	default:
		t->range = 100;
		t->damage = 10;
		t->cooldown = 1.0;
		break;
	}
}

void Tower_update(Tower* t, double deltaTime,
				  Enemy* enemies, int enemyCount, 
				  Point* waypoints, 
				  VisualProjectile* projs, int* projCount, 
				  FloatingText* fTexts, int* fTextCount) {
	
	int i, bestEnemyIdx;
	// 【修复】：删除了多余的 maxProgress 变量
	double dx, dy, distanceToTower; 
	
	if (t->currentCooldown > 0) t->currentCooldown -= deltaTime;
	
	// --- 巨型状态机：根据塔的类型执行完全不同的逻辑 ---
	switch(t->type) {
		
		case TOWER_SLING: 
		if (t->currentCooldown <= 0 && t->ammo > 0) {
			bestEnemyIdx = -1; 
			for (i = 0; i < enemyCount; i++) {
				if (enemies[i].active) {
					dx = enemies[i].x - t->x; dy = enemies[i].y - t->y;
					distanceToTower = sqrt(dx * dx + dy * dy);
					if (distanceToTower <= t->range) {
						bestEnemyIdx = i;
						break; // 找到一个就打
					}
				}
			}
			
			if (bestEnemyIdx != -1) {
				t->ammo--; // 扣除弹药
				
				// 命中敌人！
				enemies[bestEnemyIdx].hp -= t->damage;
				enemies[bestEnemyIdx].hitFlashTimer = 0.1; 
				t->currentCooldown = t->cooldown;
				
				projs[*projCount].startX = (double)t->x; projs[*projCount].startY = (double)t->y;
				projs[*projCount].targetX = enemies[bestEnemyIdx].x; projs[*projCount].targetY = enemies[bestEnemyIdx].y;
				projs[*projCount].life = 0.2; projs[*projCount].maxLife = 0.2;
				(*projCount)++;
				
				fTexts[*fTextCount].x = enemies[bestEnemyIdx].x; fTexts[*fTextCount].y = enemies[bestEnemyIdx].y - 20;
				fTexts[*fTextCount].damage = t->damage; fTexts[*fTextCount].life = 0.8;
				(*fTextCount)++;
			}
		}
		break;
		
	case TOWER_TOTEM:
		// 图腾逻辑（预留位置）
		break;
	}
}

void Tower_draw(Tower* t) {
	// 根据时代和类型画不同的外形
	if (t->era == 0) {
		// 石器时代
		if (t->type == TOWER_SLING) {
			DrawRectangle(t->x - 14, t->y - 14, 28, 28, DARKBROWN); // 木头底座
			DrawCircle(t->x, t->y, 6, GRAY); // 石头
			// 没弹药了变灰色
			if (t->ammo <= 0) DrawText("Empty", t->x - 15, t->y - 20, 10, RED);
		} else if (t->type == TOWER_TOTEM) {
			DrawRectangle(t->x - 10, t->y - 20, 20, 40, MAROON); // 木雕
		} else {
			DrawRectangle(t->x - 14, t->y - 14, 28, 28, LIGHTGRAY); 
		}
	} else {
		// 古代和未来...
		DrawRectangle(t->x - 14, t->y - 14, 28, 28, MAGENTA); 
	}
}
