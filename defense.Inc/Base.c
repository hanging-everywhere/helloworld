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
	
	b->freeBuildCount  = 0;
	b->trapShield      = 0;
	
	b->invincible      = 0;
	b->invincibleTimer = 0.0;
	b->invincibleUsed  = 0;
	
	b->satelliteCooldown = 2.0;
	b->satelliteTimer    = 2.0;
	
	switch (era) {
		case 0:  b->maxHp = 10; break;
		case 1:  b->maxHp = 15; break;
		default: b->maxHp = 20; break;
	}
	b->hp = b->maxHp;
}

/* ================================================================
Base_update
================================================================ */
void Base_update(Base* b, double deltaTime,
				 Enemy* enemies, int enemyCount,
				 FloatingText* fTexts, int* fTextCount,
				 VisualProjectile* projs, int* projCount) {
	int i;
	double dx, dy;
	
	b->animTimer += deltaTime;
	
	/* ---- 无敌计时（未来A1）---- */
	if (b->invincible) {
		b->invincibleTimer -= deltaTime;
		if (b->invincibleTimer <= 0) {
			b->invincible = 0;
		}
	}
	
	/* ---- 石器A2 / 古代A1：自带火攻 / 火炮 ---- */
	if (b->attackDamage > 0 && b->attackRange > 0) {
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
					projs[*projCount].isCrow   = 0;
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
	
	/* ---- 未来A2：卫星轨道炮（全图最近敌人）---- */
	if (b->era == 2 && b->levelA >= 2) {
		b->satelliteTimer -= deltaTime;
		if (b->satelliteTimer <= 0) {
			b->satelliteTimer = b->satelliteCooldown;
			/* 找全图存活敌人中hp最高者（模拟卫星瞄准最强目标）*/
			int bestIdx = -1;
			int bestHp  = -1;
			for (i = 0; i < enemyCount; i++) {
				if (!enemies[i].active || enemies[i].hp <= 0) continue;
				if (enemies[i].hp > bestHp) { bestHp = enemies[i].hp; bestIdx = i; }
			}
			if (bestIdx >= 0) {
				enemies[bestIdx].hp -= 70;
				enemies[bestIdx].hitFlashTimer = 0.2;
				if (*fTextCount < 300) {
					fTexts[*fTextCount].x      = enemies[bestIdx].x;
					fTexts[*fTextCount].y      = enemies[bestIdx].y - 20;
					fTexts[*fTextCount].damage = 70;
					fTexts[*fTextCount].life   = 0.8;
					(*fTextCount)++;
				}
				/* 视觉：从画面顶部打下来的激光（用 proj 表示）*/
				if (*projCount < 300) {
					projs[*projCount].startX   = enemies[bestIdx].x;
					projs[*projCount].startY   = 0;
					projs[*projCount].targetX  = enemies[bestIdx].x;
					projs[*projCount].targetY  = enemies[bestIdx].y;
					projs[*projCount].targetId = enemies[bestIdx].id;
					projs[*projCount].damage   = 0; // 伤害已结算
					projs[*projCount].life     = 0.25;
					projs[*projCount].maxLife  = 0.25;
					projs[*projCount].isHit    = 0;
					projs[*projCount].isCrow   = 0;
					(*projCount)++;
				}
			}
		}
	}
}

/* ================================================================
Base_draw
================================================================ */
void Base_draw(Base* b) {
	double t = b->animTimer;
	int pulse = (int)(sin(t * 8.0) * 6);
	
	if (b->era == 0) {
		/* ---- 石器时代：火神像，以水绕之 ---- */
		DrawCircleLines(b->x, b->y, 38 + pulse/2, (Color){40, 120, 200, 180});
		DrawCircleLines(b->x, b->y, 44,            (Color){60, 160, 240, 100});
		DrawCircle(b->x, b->y, 26, (Color){80, 50, 20, 255});
		DrawCircle(b->x, b->y, 20, (Color){110, 70, 30, 255});
		DrawRectangle(b->x - 5, b->y - 18, 10, 20, (Color){150, 40, 10, 255});
		DrawCircle(b->x, b->y - 22, 7, (Color){255, 140, 0, 255});
		DrawCircle(b->x, b->y - 26, 4 + pulse/3, (Color){255, 220, 50, 255});
		if (b->levelA >= 2) {
			int k;
			for (k = 0; k < 4; k++) {
				double angle = t * 1.5 + k * 3.14159 / 2.0;
				int fx = b->x + (int)(cos(angle) * 32);
				int fy = b->y + (int)(sin(angle) * 32);
				DrawCircle(fx, fy, 4 + (int)(sin(t * 10 + k) * 2), ORANGE);
			}
		}
		
	} else if (b->era == 1) {
		/* ---- 古代文明：中式机关亭，四周城墙 ---- */
		/* 四角城墙 */
		int wallOff = 32;
		DrawRectangle(b->x - wallOff - 6, b->y - wallOff - 6, 12, 12, (Color){100, 90, 70, 255});
		DrawRectangle(b->x + wallOff - 6, b->y - wallOff - 6, 12, 12, (Color){100, 90, 70, 255});
		DrawRectangle(b->x - wallOff - 6, b->y + wallOff - 6, 12, 12, (Color){100, 90, 70, 255});
		DrawRectangle(b->x + wallOff - 6, b->y + wallOff - 6, 12, 12, (Color){100, 90, 70, 255});
		/* 四面城墙连线 */
		DrawLine(b->x - wallOff, b->y - wallOff, b->x + wallOff, b->y - wallOff, (Color){80, 70, 55, 255});
		DrawLine(b->x - wallOff, b->y + wallOff, b->x + wallOff, b->y + wallOff, (Color){80, 70, 55, 255});
		DrawLine(b->x - wallOff, b->y - wallOff, b->x - wallOff, b->y + wallOff, (Color){80, 70, 55, 255});
		DrawLine(b->x + wallOff, b->y - wallOff, b->x + wallOff, b->y + wallOff, (Color){80, 70, 55, 255});
		/* 亭台底座 */
		DrawRectangle(b->x - 18, b->y - 14, 36, 28, (Color){140, 110, 60, 255});
		/* 屋顶（梯形）*/
		DrawTriangle(
					 (Vector2){b->x,       b->y - 28},
					 (Vector2){b->x - 22, b->y - 14},
					 (Vector2){b->x + 22, b->y - 14},
					 (Color){180, 50, 30, 255}
					 );
		/* 机关齿轮（A1激活后旋转）*/
		if (b->levelA >= 1) {
			int k;
			float spin = (float)(t * 80.0);
			for (k = 0; k < 8; k++) {
				float angle = spin + k * 45.0f;
				int gx = b->x + (int)(cos(angle * 3.14159f/180) * 14);
				int gy = b->y + (int)(sin(angle * 3.14159f/180) * 14);
				DrawCircle(gx, gy, 3, (Color){220, 180, 50, 255});
			}
			/* 显示剩余免费建造次数 */
			if (b->freeBuildCount > 0) {
				DrawText(TextFormat("FREE x%d", b->freeBuildCount), b->x - 22, b->y + wallOff + 10, 10, GOLD);
			}
		}
		/* A2：陷阱盾 */
		if (b->levelA >= 2 && b->trapShield > 0) {
			int k;
			for (k = 0; k < b->trapShield; k++) {
				float angle = (float)(k * 120.0 * 3.14159 / 180.0);
				int sx = b->x + (int)(cos(angle) * 28);
				int sy = b->y + (int)(sin(angle) * 28);
				DrawRectangle(sx - 4, sy - 4, 8, 8, (Color){200, 180, 40, 220});
				DrawRectangleLines(sx - 4, sy - 4, 8, 8, GOLD);
			}
		}
		
	} else {
		/* ---- 科幻未来：悬浮能量球，防御力场 ---- */
		/* 外层力场环 */
		Color fieldColor = b->invincible ?
		(Color){50, 255, 255, 200} : (Color){30, 120, 255, 150};
		DrawCircleLines(b->x, b->y, 44 + pulse/2, fieldColor);
		DrawCircleLines(b->x, b->y, 40,            Fade(fieldColor, 0.5f));
		/* 旋转防御环 */
		{
			int k;
			for (k = 0; k < 3; k++) {
				double angle = t * 1.2 + k * 3.14159 * 2 / 3;
				int rx = b->x + (int)(cos(angle) * 34);
				int ry = b->y + (int)(sin(angle) * 34);
				DrawCircle(rx, ry, 5, (Color){80, 200, 255, 220});
			}
		}
		/* 核心能量球 */
		DrawCircle(b->x, b->y, 20, (Color){10, 20, 60, 255});
		DrawCircle(b->x, b->y, 14, (Color){30, 100, 220, 255});
		DrawCircle(b->x, b->y, 8,  (Color){150, 220, 255, 255});
		/* 无敌特效（A1） */
		if (b->invincible) {
			DrawCircle(b->x, b->y, 44, Fade(SKYBLUE, 0.25f));
			DrawCircleLines(b->x, b->y, 48, SKYBLUE);
			DrawText(TextFormat("SHIELD %.1fs", b->invincibleTimer),
					 b->x - 28, b->y - 60, 10, SKYBLUE);
		}
		/* 卫星轨道（A2）*/
		if (b->levelA >= 2) {
			double satAngle = t * 0.8;
			int sx = b->x + (int)(cos(satAngle) * 55);
			int sy = b->y + (int)(sin(satAngle) * 55);
			DrawCircleLines(b->x, b->y, 55, Fade(YELLOW, 0.3f));
			DrawCircle(sx, sy, 5, YELLOW);
			DrawCircle(sx, sy, 3, WHITE);
		}
	}
	
	/* ---- HP 血条 ---- */
	int barW = 60;
	int barX = b->x - barW / 2;
	int barY = b->y + 52;
	int fillW = (b->maxHp > 0) ? (int)((double)b->hp / b->maxHp * barW) : 0;
	if (fillW < 0) fillW = 0;
	DrawRectangle(barX, barY, barW, 6, (Color){80, 0, 0, 200});
	DrawRectangle(barX, barY, fillW, 6, (Color){0, 200, 60, 220});
	DrawRectangleLines(barX, barY, barW, 6, (Color){150, 150, 150, 200});
	DrawText(TextFormat("%d/%d", b->hp, b->maxHp), barX, barY + 8, 10, LIGHTGRAY);
}

/* ================================================================
Base_upgrade
================================================================ */
int Base_upgrade(Base* b, int path, int* wood, int* stone, int* metal, int* material) {
	if (path != 0) return 2; // 目前只有A路
	
	/* ---- 石器时代 ---- */
	if (b->era == 0) {
		if (b->levelA == 0) {
			if (*wood < 60) return 1;
			*wood -= 60;
			b->levelA = 1;
			return 0;
		} else if (b->levelA == 1) {
			if (*wood < 100) return 1;
			*wood -= 100;
			b->levelA = 2;
			b->attackDamage   = 25;
			b->attackRange    = 160;
			b->attackCooldown = 2.0;
			b->attackTimer    = 2.0;
			return 0;
		}
		return 2;
	}
	
	/* ---- 古代文明 ---- */
	if (b->era == 1) {
		if (b->levelA == 0) {
			/* A1：获得3次免费建造机会 + 自带50伤害/2秒火炮 */
			if (*metal < 150) return 1;
			*metal -= 150;
			b->levelA = 1;
			b->freeBuildCount = 3;
			b->attackDamage   = 50;
			b->attackRange    = 200;
			b->attackCooldown = 2.0;
			b->attackTimer    = 2.0;
			return 0;
		} else if (b->levelA == 1) {
			/* A2：机关塔前出现3个陷阱盾，可抵挡3次入侵 */
			if (*metal < 250) return 1;
			*metal -= 250;
			b->levelA = 2;
			b->trapShield = 3;
			return 0;
		}
		return 2;
	}
	
	/* ---- 科幻未来 ---- */
	if (b->era == 2) {
		if (b->levelA == 0) {
			/* A1：最后一滴血受攻击时获得5秒无敌（只触发一次）*/
			if (*material < 300) return 1;
			*material -= 300;
			b->levelA = 1;
			b->invincibleUsed = 0;
			return 0;
		} else if (b->levelA == 1) {
			/* A2：释放卫星轨道炮，每2秒对全图hp最高敌人造成70伤害 */
			if (*material < 500) return 1;
			*material -= 500;
			b->levelA = 2;
			b->satelliteCooldown = 2.0;
			b->satelliteTimer    = 2.0;
			return 0;
		}
		return 2;
	}
	
	return 2;
}

/* ================================================================
Base_getUpgradeLabel
================================================================ */
void Base_getUpgradeLabel(Base* b, int path, const char** outLabel, int* outAffordable,
						  int wood, int stone, int metal, int material) {
	*outLabel      = "";
	*outAffordable = 0;
	
	if (path != 0) {
		*outLabel = "(Not available)";
		return;
	}
	
	/* 石器时代 */
	if (b->era == 0) {
		if (b->levelA == 0) {
			*outLabel      = "A1: Prod+50%  -60W";
			*outAffordable = (wood >= 60);
		} else if (b->levelA == 1) {
			*outLabel      = "A2: FireAtk25 -100W";
			*outAffordable = (wood >= 100);
		} else {
			*outLabel = "A: MAX";
		}
		return;
	}
	
	/* 古代文明 */
	if (b->era == 1) {
		if (b->levelA == 0) {
			*outLabel      = "A1: FreeBuild x3  -150M";
			*outAffordable = (metal >= 150);
		} else if (b->levelA == 1) {
			*outLabel      = "A2: TrapShield x3  -250M";
			*outAffordable = (metal >= 250);
		} else {
			*outLabel = "A: MAX";
		}
		return;
	}
	
	/* 科幻未来 */
	if (b->era == 2) {
		if (b->levelA == 0) {
			*outLabel      = "A1: LastStand 5s  -300";
			*outAffordable = (material >= 300);
		} else if (b->levelA == 1) {
			*outLabel      = "A2: Satellite 70dmg  -500";
			*outAffordable = (material >= 500);
		} else {
			*outLabel = "A: MAX";
		}
		return;
	}
}

/* ================================================================
辅助函数
================================================================ */
int Base_consumeFreeBuild(Base* b) {
	if (b->era == 1 && b->levelA >= 1 && b->freeBuildCount > 0) {
		b->freeBuildCount--;
		return 1;
	}
	return 0;
}

int Base_consumeTrapShield(Base* b) {
	if (b->era == 1 && b->levelA >= 2 && b->trapShield > 0) {
		b->trapShield--;
		return 1; // 伤害被陷阱拦截
	}
	return 0;
}

int Base_tryInvincible(Base* b) {
	if (b->era == 2 && b->levelA >= 1 && !b->invincibleUsed && b->hp <= 1) {
		b->invincible      = 1;
		b->invincibleTimer = 5.0;
		b->invincibleUsed  = 1;
		return 1;
	}
	return 0;
}
