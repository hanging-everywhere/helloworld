#include "Base.h"
#include <math.h>

void Base_init(Base* b, int x, int y, int era) {
	b->x   = x;
	b->y   = y;
	b->era = era;
	
	b->levelA = 0;
	b->levelB = 0;
	b->animTimer = 0.0;
	
	b->attackDamage   = 0;
	b->attackRange    = 0;
	b->attackCooldown = 2.0;
	b->attackTimer    = 2.0;
	
	switch (era) {
		case 0:  b->maxHp = 10; break;
		case 1:  b->maxHp = 15; break;
		default: b->maxHp = 20; break;
	}
	b->hp = b->maxHp;
}

void Base_update(Base* b, double deltaTime,
				 Enemy* enemies, int enemyCount,
				 FloatingText* fTexts, int* fTextCount,
				 VisualProjectile* projs, int* projCount) {
	int i;
	double dx, dy;
	
	b->animTimer += deltaTime;
	
	// 石器A2：自带火攻
	if (b->era == 0 && b->attackDamage > 0 && b->attackRange > 0) {
		b->attackTimer -= deltaTime;
		if (b->attackTimer <= 0) {
			int bestIdx = -1;
			double bestDist = 1e18;
			for (i = 0; i < enemyCount; i++) {
				if (!enemies[i].active || enemies[i].hp <= 0) continue;
				dx = enemies[i].x - b->x;
				dy = enemies[i].y - b->y;
				double d2 = dx*dx + dy*dy;
				if (d2 <= (double)b->attackRange * b->attackRange && d2 < bestDist) {
					bestDist = d2; bestIdx = i;
				}
			}
			if (bestIdx >= 0) {
				enemies[bestIdx].hp -= b->attackDamage;
				enemies[bestIdx].hitFlashTimer = 0.1;
				if (*projCount < 300) {
					projs[*projCount].startX   = (double)b->x;
					projs[*projCount].startY   = (double)b->y;
					projs[*projCount].targetX  = enemies[bestIdx].x;
					projs[*projCount].targetY  = enemies[bestIdx].y;
					projs[*projCount].targetId = enemies[bestIdx].id;
					projs[*projCount].damage   = b->attackDamage;
					projs[*projCount].life     = 0.15;
					projs[*projCount].maxLife  = 0.15;
					projs[*projCount].isHit    = 1;
					(*projCount)++;
				}
				if (*fTextCount < 300) {
					fTexts[*fTextCount].x      = enemies[bestIdx].x;
					fTexts[*fTextCount].y      = enemies[bestIdx].y - 20;
					fTexts[*fTextCount].damage = b->attackDamage;
					fTexts[*fTextCount].life   = 0.8;
					(*fTextCount)++;
				}
			}
			b->attackTimer = b->attackCooldown;
		}
	}
}

void Base_draw(Base* b) {
	double t = b->animTimer;
	int pulse = (int)(sin(t * 8.0) * 6);
	
	if (b->era == 0) {
		// 石器时代：火神像，以水绕之
		
		// 外圈水波
		DrawCircleLines(b->x, b->y, 38 + pulse/2, (Color){40, 120, 200, 180});
		DrawCircleLines(b->x, b->y, 44,            (Color){60, 160, 240, 100});
		
		// 石台底座
		DrawCircle(b->x, b->y, 26, (Color){80, 50, 20, 255});
		DrawCircle(b->x, b->y, 20, (Color){110, 70, 30, 255});
		
		// 火神像身体
		DrawRectangle(b->x - 5, b->y - 18, 10, 20, (Color){150, 40, 10, 255});
		
		// 火焰头
		DrawCircle(b->x, b->y - 22, 7, (Color){255, 140, 0, 255});
		DrawCircle(b->x, b->y - 26, 4 + pulse/3, (Color){255, 220, 50, 255});
		
		// A2：四角旋转火球（自带火攻的视觉提示）
		if (b->levelA >= 2) {
			int k;
			for (k = 0; k < 4; k++) {
				double angle = t * 1.5 + k * 3.14159 / 2.0;
				int fx = b->x + (int)(cos(angle) * 32);
				int fy = b->y + (int)(sin(angle) * 32);
				DrawCircle(fx, fy, 4 + (int)(sin(t * 10 + k) * 2), ORANGE);
			}
		}
		
	} else {
		// 古代/未来：简单占位（后续版本再做）
		DrawCircle(b->x, b->y, 35 + pulse, (Color){255, 80, 0, 255});
		DrawCircle(b->x, b->y, 25 + pulse/2, (Color){255, 160, 0, 255});
		DrawCircle(b->x, b->y, 15, (Color){255, 230, 100, 255});
	}
	
	// HP 血条
	int barW = 60;
	int barX = b->x - barW / 2;
	int barY = b->y + 50;
	int fillW = (b->maxHp > 0) ? (int)((double)b->hp / b->maxHp * barW) : 0;
	DrawRectangle(barX, barY, barW, 6, (Color){80, 0, 0, 200});
	DrawRectangle(barX, barY, fillW, 6, (Color){0, 200, 60, 220});
	DrawRectangleLines(barX, barY, barW, 6, (Color){150, 150, 150, 200});
	DrawText(TextFormat("%d/%d", b->hp, b->maxHp), barX, barY + 8, 10, LIGHTGRAY);
}

int Base_upgrade(Base* b, int path, int* wood, int* stone, int* metal, int* material) {
	// 只有石器时代有升级路线
	if (b->era != 0) return 2;
	// 只有A路
	if (path != 0) return 2;
	
	if (b->levelA == 0) {
		// A1：资源产量+50%（由 GameManager 读 levelA 判断，这里只扣费升级）
		if (*wood < 60) return 1;
		*wood -= 60;
		b->levelA = 1;
		return 0;
	} else if (b->levelA == 1) {
		// A2：友塔攻速+30% + 自带火攻25伤害/2秒
		if (*wood < 100) return 1;
		*wood -= 100;
		b->levelA = 2;
		b->attackDamage   = 25;
		b->attackRange    = 160;
		b->attackCooldown = 2.0;
		b->attackTimer    = 2.0;
		return 0;
	}
	return 2; // 满级
}

void Base_getUpgradeLabel(Base* b, int path, const char** outLabel, int* outAffordable,
						  int wood, int stone, int metal, int material) {
	*outLabel    = "";
	*outAffordable = 0;
	
	if (b->era != 0 || path != 0) {
		*outLabel = "(Not available)";
		return;
	}
	
	if (b->levelA == 0) {
		*outLabel    = "A1: Prod+50%  -60 Wood";
		*outAffordable = (wood >= 60);
	} else if (b->levelA == 1) {
		*outLabel    = "A2: FireAtk   -100 Wood";
		*outAffordable = (wood >= 100);
	} else {
		*outLabel = "A: MAX";
	}
}
