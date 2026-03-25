#include "Tower.h"
#include <math.h>

// ============================================================
//  Tower_init
// ============================================================
void Tower_init(Tower* t, int startX, int startY, int era, int type) {
	t->x = startX; t->y = startY;
	t->era = era;  t->type = type;
	t->levelA = 0; t->levelB = 0;
	t->currentCooldown = 0.0;
	t->timer1 = 0.0; t->timer2 = 0.0;
	t->state = 0;  t->targetId = -1;
	t->buffTimer = 0.0; t->buffSpeedRatio = 1.0;
	t->pairedIndex = -1;
	t->currentFrame = 0; t->framesCounter = 0;
	t->framesSpeed = 15; t->maxFrames = 1;
	t->frameRec = (Rectangle){0,0,32,32};
	t->ammo = 0;
	
	// 专属字段默认值
	t->hitCountOnTarget = 0; t->lastTargetId = -1;
	t->reloadTimer = 0.0;
	t->explosiveCharges = 0; t->explosiveDmgMult = 1.0;
	t->crowCooldown = 8.0; t->crowCurrentCooldown = 0.0;
	t->chargeTime = 0.0; t->isCharging = 0;
	t->gravityExplodeReady = 0;
	t->portalRole = 0; t->portalTeleCount = 0; t->portalCollapseCD = 0.0;
	t->timeWaveTimer = 0.0;
	t->laserBeamTimer = 0.0; t->laserCharges = 0; t->laserPulseCooldown = 0.0;
	t->pairedIndex = 70; // 投石索命中率默认值复用
	
	switch (type) {
		// --- 石器时代 ---
	case TOWER_SLING:
		t->range=150; t->damage=20; t->cooldown=2.0; t->ammo=12;
		t->maxFrames=4; t->pairedIndex=70;
		break;
	case TOWER_BONESPEAR:
		t->range=40; t->damage=45; t->cooldown=0.5; t->ammo=3;
		t->timer1=3.0; t->state=0; t->maxFrames=4;
		break;
	case TOWER_TOTEM:
		t->range=180; t->damage=0; t->cooldown=7.0;
		break;
	case TOWER_FIRE:
		t->range=70; t->damage=15; t->cooldown=2.0;
		t->state=1;
		break;
		
		// --- 古代文明 ---
	case TOWER_CROSSBOW:
		t->range=256; t->damage=40; t->cooldown=3.0; // 3秒上弦
		t->state=0;   // 0=上弦中, 1=就绪
		t->timer1=3.0;
		break;
	case TOWER_MINE:
		t->range=48; t->damage=120; t->cooldown=0.0;
		t->ammo=1; t->state=0; // 0=激活, 1=引爆
		t->reloadTimer=0.0;
		break;
	case TOWER_CAULDRON:
		t->range=160; t->damage=5; t->cooldown=5.0; // 普通沸水5秒CD
		t->explosiveCharges=0; t->explosiveDmgMult=1.0;
		break;
	case TOWER_FIRECROW:
		t->range=448; t->damage=85; t->cooldown=5.0;  // 火枪
		t->crowCooldown=8.0; t->crowCurrentCooldown=0.0; // 飞鸦
		break;
		
		// --- 科幻未来 ---
	case TOWER_PULSE:
		t->range=300; t->damage=200; t->cooldown=3.0;
		break;
	case TOWER_GRAVITY:
		t->range=160; t->damage=0; t->cooldown=0.1; // 每帧刷减速
		t->gravityExplodeReady=0;
		break;
	case TOWER_PORTAL:
		t->range=32; t->damage=0; t->cooldown=0.0;
		t->pairedIndex=-1; t->state=0; // 0=等待配对, 1=激活
		t->portalRole=0; t->portalTeleCount=0; t->portalCollapseCD=0.0;
		break;
	case TOWER_TIMECLOCK:
		t->range=200; t->damage=0; t->cooldown=7.0;
		t->timeWaveTimer=0.0;
		break;
	case TOWER_LASER:
		t->range=700; t->damage=25; t->cooldown=0.1; // 持续照射用小CD
		t->targetId=-1; t->laserBeamTimer=0.0;
		t->laserCharges=0; t->laserPulseCooldown=0.0;
		break;
	default:
		t->range=100; t->damage=10; t->cooldown=1.0;
		break;
	}
}

// ============================================================
//  辅助：添加浮动伤害文字
// ============================================================
static void AddFloatText(FloatingText* fTexts, int* fTextCount, double x, double y, int dmg) {
	if (*fTextCount >= 300) return;
	fTexts[*fTextCount].x      = x;
	fTexts[*fTextCount].y      = y - 20;
	fTexts[*fTextCount].damage = dmg;
	fTexts[*fTextCount].life   = 0.8;
	(*fTextCount)++;
}

// 伤害敌人（同时累计因果律）
static void DamageEnemy(Enemy* e, int dmg, FloatingText* fTexts, int* fTextCount) {
	if (e->causeMarkActive) e->causeDamageAccum += dmg;
	e->hp            -= dmg;
	e->hitFlashTimer  = 0.1;
	AddFloatText(fTexts, fTextCount, e->x, e->y, dmg);
}

// ============================================================
//  Tower_update
// ============================================================
void Tower_update(Tower* t, double deltaTime,
				  Enemy* enemies, int enemyCount,
				  Tower* allTowers, int towerCount,
				  Point* waypoints,
				  VisualProjectile* projs, int* projCount,
				  FloatingText* fTexts, int* fTextCount,
				  int* playerWood, int* playerStone,
				  int* playerMetal, int* playerMaterial,
				  BurnZone* burnZones, int* burnZoneCount) {
	int i, j;
	double dx, dy;
	
	// 序列帧动画
	if (t->maxFrames > 1) {
		t->framesCounter++;
		if (t->framesCounter >= t->framesSpeed) {
			t->framesCounter = 0;
			t->currentFrame  = (t->currentFrame + 1) % t->maxFrames;
			t->frameRec.x    = (float)t->currentFrame * t->frameRec.width;
		}
	}
	
	// Buff 处理
	if (t->buffTimer > 0) {
		t->buffTimer -= deltaTime;
		if (t->buffTimer <= 0) t->buffSpeedRatio = 1.0;
	}
	if (t->currentCooldown > 0)
		t->currentCooldown -= deltaTime * t->buffSpeedRatio;
	
	switch (t->type) {
		
		// ============================================================
		case TOWER_SLING: {
		if (t->currentCooldown > 0 || t->ammo <= 0) break;
		int stoneCost = (t->levelA >= 2) ? 10 : (t->levelB >= 1 ? 7 : 5);
		if (*playerStone < stoneCost) break;
		int best = -1;
		for (i = 0; i < enemyCount; i++) {
			if (!enemies[i].active || enemies[i].hp <= 0) continue;
			dx = enemies[i].x - t->x; dy = enemies[i].y - t->y;
			if (dx*dx + dy*dy <= (double)t->range*t->range) { best = i; break; }
		}
		if (best < 0) break;
		*playerStone -= stoneCost;
		t->ammo--;
		int hitChance = t->pairedIndex; // 存命中率阈值
		int isHit = (GetRandomValue(1,100) <= hitChance) ? 1 : 0;
		t->currentCooldown = t->cooldown;
		if (*projCount < 300) {
			projs[*projCount].startX  = t->x; projs[*projCount].startY = t->y;
			projs[*projCount].isHit   = isHit; projs[*projCount].isCrow = 0;
			if (isHit) {
				projs[*projCount].targetX  = enemies[best].x;
				projs[*projCount].targetY  = enemies[best].y;
				projs[*projCount].targetId = enemies[best].id;
				projs[*projCount].damage   = t->damage;
			} else {
				int tx = enemies[best].x + GetRandomValue(-35,35);
				int ty = enemies[best].y + GetRandomValue(-35,35);
				if (tx<10) tx=10; if (tx>1014) tx=1014;
				if (ty<10) ty=10; if (ty>758)  ty=758;
				projs[*projCount].targetX  = tx; projs[*projCount].targetY = ty;
				projs[*projCount].targetId = -1; projs[*projCount].damage  = 0;
				AddFloatText(fTexts, fTextCount, enemies[best].x, enemies[best].y, -1);
			}
			projs[*projCount].life = 0.2; projs[*projCount].maxLife = 0.2;
			(*projCount)++;
		}
		break;
	}
		
		// ============================================================
	case TOWER_BONESPEAR:
		if (t->ammo <= 0) break;
		if (t->state == 0) {
			t->timer1 -= deltaTime;
			if (t->timer1 <= 0) t->state = 1;
		} else if (t->state == 1 && t->currentCooldown <= 0) {
			int hitCount = (t->levelB >= 1) ? 3 : 1;
			int hit = 0;
			for (i = 0; i < enemyCount && hit < hitCount; i++) {
				if (!enemies[i].active || enemies[i].hp <= 0) continue;
				dx = enemies[i].x - t->x; dy = enemies[i].y - t->y;
				if (dx*dx + dy*dy > (double)t->range*t->range) continue;
				DamageEnemy(&enemies[i], t->damage, fTexts, fTextCount);
				if (t->levelB >= 1) enemies[i].isInvisible = 0;
				if (t->levelA >= 1) { enemies[i].slowTimer = 2.0; enemies[i].slowRatio = 0.8; }
				if (t->levelA >= 2) { enemies[i].dotTimer = 3.0; enemies[i].dotTickTimer = 1.0; enemies[i].dotDamage = 8; }
				hit++;
			}
			if (hit > 0) { t->ammo--; t->currentCooldown = t->cooldown; }
		}
		break;
		
		// ============================================================
	case TOWER_TOTEM:
		if (t->currentCooldown <= 0) {
			if (t->levelA >= 1) {
				for (i = 0; i < towerCount; i++) {
					if (allTowers[i].type == TOWER_TOTEM) continue;
					dx = allTowers[i].x - t->x; dy = allTowers[i].y - t->y;
					if (dx*dx + dy*dy <= (double)t->range*t->range) {
						allTowers[i].buffTimer = 3.0; allTowers[i].buffSpeedRatio = 1.2;
					}
				}
			} else {
				double stunDur = (t->levelB >= 1) ? 5.0 : 3.0;
				for (i = 0; i < enemyCount; i++) {
					if (!enemies[i].active || enemies[i].hp <= 0) continue;
					dx = enemies[i].x - t->x; dy = enemies[i].y - t->y;
					if (dx*dx + dy*dy <= (double)t->range*t->range) enemies[i].stunTimer = stunDur;
				}
			}
			if (t->levelB >= 2) {
				for (i = 0; i < enemyCount; i++) {
					if (!enemies[i].active || enemies[i].hp <= 0) continue;
					dx = enemies[i].x - t->x; dy = enemies[i].y - t->y;
					if (dx*dx + dy*dy <= (double)t->range*t->range)
						DamageEnemy(&enemies[i], 30, fTexts, fTextCount);
				}
			}
			AddFloatText(fTexts, fTextCount, t->x, t->y + 10, -3);
			t->timer2 = 0.5; t->currentCooldown = t->cooldown;
		}
		if (t->timer2 > 0) t->timer2 -= deltaTime;
		break;
		
		// ============================================================
	case TOWER_FIRE:
		if (t->levelA < 2) {
			t->timer1 += deltaTime;
			if (t->timer1 >= 1.0) {
				t->timer1 -= 1.0;
				if (*playerWood >= 1) { (*playerWood)--; if (t->state == 0) t->state = 1; }
				else t->state = 0;
			}
		} else { t->state = 2; }
		
		if ((t->state == 1 || t->state == 2) && t->currentCooldown <= 0) {
			int triggered = 0;
			for (i = 0; i < enemyCount; i++) {
				if (!enemies[i].active || enemies[i].hp <= 0) continue;
				dx = enemies[i].x - t->x; dy = enemies[i].y - t->y;
				if (dx*dx + dy*dy > (double)t->range*t->range) continue;
				DamageEnemy(&enemies[i], t->damage, fTexts, fTextCount);
				enemies[i].dotTimer=3.0; enemies[i].dotTickTimer=1.0; enemies[i].dotDamage=5;
				enemies[i].isInvisible = 0;
				if (t->levelA >= 1) {
					for (j = 0; j < enemyCount; j++) {
						if (j==i || !enemies[j].active) continue;
						double ddx=enemies[j].x-enemies[i].x, ddy=enemies[j].y-enemies[i].y;
						if (ddx*ddx+ddy*ddy <= 50*50) {
							enemies[j].dotTimer=3.0; enemies[j].dotTickTimer=1.0; enemies[j].dotDamage=5;
						}
					}
				}
				triggered = 1;
			}
			if (triggered) { t->currentCooldown = t->cooldown; t->timer2 = 0.3; }
		}
		if (t->timer2 > 0) t->timer2 -= deltaTime;
		break;
		
		// ============================================================
		case TOWER_CROSSBOW: {
			// 上弦阶段
			if (t->state == 0) {
				t->timer1 -= deltaTime;
				if (t->timer1 <= 0) { t->state = 1; t->timer1 = 0; }
				break;
			}
			if (t->currentCooldown > 0) break;
			// 检查资源
			int costWood = 6, costMetal = 10;
			if (t->levelB >= 1) { costWood *= 3; costMetal *= 3; }
			if (*playerWood < costWood || *playerMetal < costMetal) break;
			
			// 找目标
			int best = -1;
			for (i = 0; i < enemyCount; i++) {
				if (!enemies[i].active || enemies[i].hp <= 0) continue;
				dx = enemies[i].x - t->x; dy = enemies[i].y - t->y;
				if (dx*dx + dy*dy <= (double)t->range*t->range) { best = i; break; }
			}
			if (best < 0) break;
			*playerWood -= costWood; *playerMetal -= costMetal;
			
			if (t->levelA >= 1) {
				// A1：穿透，打射程内所有敌人
				for (i = 0; i < enemyCount; i++) {
					if (!enemies[i].active || enemies[i].hp <= 0) continue;
					dx = enemies[i].x - t->x; dy = enemies[i].y - t->y;
					if (dx*dx + dy*dy > (double)t->range*t->range) continue;
					DamageEnemy(&enemies[i], t->damage, fTexts, fTextCount);
					if (t->levelA >= 2) {
						// A2：留倒钩DoT
						enemies[i].dotTimer=3.0; enemies[i].dotTickTimer=1.0; enemies[i].dotDamage=10;
					}
				}
			} else if (t->levelB >= 1) {
				// B1：扇形散射3个最近目标
				int shotsFired = 0;
				for (i = 0; i < enemyCount && shotsFired < 3; i++) {
					if (!enemies[i].active || enemies[i].hp <= 0) continue;
					dx = enemies[i].x - t->x; dy = enemies[i].y - t->y;
					if (dx*dx + dy*dy > (double)t->range*t->range) continue;
					int dmg = t->damage;
					if (t->levelB >= 2) {
						// B2：连发5次，每次-5%
						for (j = 0; j < 5; j++) {
							int d = (int)(dmg * (1.0 - 0.05*j));
							DamageEnemy(&enemies[i], d, fTexts, fTextCount);
						}
					} else {
						DamageEnemy(&enemies[i], dmg, fTexts, fTextCount);
					}
					shotsFired++;
				}
			} else {
				// 基础单发
				DamageEnemy(&enemies[best], t->damage, fTexts, fTextCount);
			}
			
			// 视觉子弹
			if (*projCount < 300) {
				projs[*projCount].startX=t->x; projs[*projCount].startY=t->y;
				projs[*projCount].targetX=enemies[best].x; projs[*projCount].targetY=enemies[best].y;
				projs[*projCount].targetId=-1; projs[*projCount].damage=0; // 伤害已直接结算
				projs[*projCount].isHit=1; projs[*projCount].isCrow=0;
				projs[*projCount].life=0.15; projs[*projCount].maxLife=0.15;
				(*projCount)++;
			}
			t->currentCooldown = t->cooldown;
			t->state = 0; t->timer1 = 3.0; // 再次上弦
			break;
		}
		
		// ============================================================
	case TOWER_MINE:
		if (t->ammo <= 0) {
			// B1：自动填充
			if (t->levelB >= 1) {
				t->reloadTimer += deltaTime;
				double refillTime = 10.0;
				if (t->reloadTimer >= refillTime) {
					if (*playerMetal >= 50) { *playerMetal -= 50; t->ammo = 1; t->reloadTimer = 0; t->state = 0; }
				}
			}
			break;
		}
		// 检测敌人触发
		for (i = 0; i < enemyCount; i++) {
			if (!enemies[i].active || enemies[i].hp <= 0) continue;
			dx = enemies[i].x - t->x; dy = enemies[i].y - t->y;
			if (dx*dx + dy*dy > (double)t->range*t->range) continue;
			// 引爆
			double blastR = (t->levelA >= 1) ? t->range * 1.7 : t->range;
			for (j = 0; j < enemyCount; j++) {
				if (!enemies[j].active || enemies[j].hp <= 0) continue;
				double bx=enemies[j].x-t->x, by=enemies[j].y-t->y;
				if (bx*bx+by*by <= blastR*blastR)
					DamageEnemy(&enemies[j], t->damage, fTexts, fTextCount);
			}
			if (t->levelA >= 2) {
				// A2：二次爆炸（0.3秒后，用timer2标记）
				t->timer2 = 0.3;
			}
			// B2：连环爆——搜索附近其他地雷
			if (t->levelB >= 2) {
				for (j = 0; j < towerCount; j++) {
					if (allTowers[j].type != TOWER_MINE || &allTowers[j] == t) continue;
					double mx2=allTowers[j].x-t->x, my2=allTowers[j].y-t->y;
					if (mx2*mx2+my2*my2 <= 200*200 && allTowers[j].ammo > 0)
						allTowers[j].timer2 = 0.1; // 触发相邻地雷
				}
			}
			t->ammo = 0; t->state = 1;
			break;
		}
		// A2二次爆炸
		if (t->timer2 > 0) {
			t->timer2 -= deltaTime;
			if (t->timer2 <= 0) {
				double blastR = (t->levelA >= 1) ? t->range*1.7 : t->range;
				for (j = 0; j < enemyCount; j++) {
					if (!enemies[j].active || enemies[j].hp <= 0) continue;
					double bx=enemies[j].x-t->x, by=enemies[j].y-t->y;
					if (bx*bx+by*by <= blastR*blastR)
						DamageEnemy(&enemies[j], t->damage, fTexts, fTextCount);
				}
			}
		}
		break;
		
		// ============================================================
	case TOWER_CAULDRON:
		if (t->currentCooldown > 0) break;
		if (t->levelB >= 1 && t->explosiveCharges > 0) {
			// B1：炸药配方（有限次爆炸）
			for (i = 0; i < enemyCount; i++) {
				if (!enemies[i].active || enemies[i].hp <= 0) continue;
				dx = enemies[i].x - t->x; dy = enemies[i].y - t->y;
				if (dx*dx + dy*dy > (double)t->range*t->range) continue;
				int dmg = (int)(40 * t->explosiveDmgMult);
				DamageEnemy(&enemies[i], dmg, fTexts, fTextCount);
				if (t->levelB >= 2) {
					// B2：硝化—命中附加爆炸20%额外伤害
					DamageEnemy(&enemies[i], (int)(dmg*0.2), fTexts, fTextCount);
				}
			}
			t->explosiveDmgMult *= 1.4;
			t->explosiveCharges--;
			t->currentCooldown = t->cooldown;
		} else if (t->levelA >= 1) {
			// A1：剧毒—给范围内敌人上DoT
			for (i = 0; i < enemyCount; i++) {
				if (!enemies[i].active || enemies[i].hp <= 0) continue;
				dx = enemies[i].x - t->x; dy = enemies[i].y - t->y;
				if (dx*dx + dy*dy > (double)t->range*t->range) continue;
				enemies[i].dotTimer=5.0; enemies[i].dotTickTimer=1.0; enemies[i].dotDamage=8;
				if (t->levelA >= 2) {
					// A2：毒雾破隐身
					enemies[i].isInvisible = 0;
					DamageEnemy(&enemies[i], 20, fTexts, fTextCount);
				}
			}
			t->currentCooldown = t->cooldown;
		} else {
			// 基础：沸水低伤害
			for (i = 0; i < enemyCount; i++) {
				if (!enemies[i].active || enemies[i].hp <= 0) continue;
				dx = enemies[i].x - t->x; dy = enemies[i].y - t->y;
				if (dx*dx + dy*dy > (double)t->range*t->range) continue;
				DamageEnemy(&enemies[i], t->damage, fTexts, fTextCount);
			}
			t->currentCooldown = t->cooldown;
		}
		break;
		
		// ============================================================
		case TOWER_FIRECROW: {
			// --- 火枪 ---
			if (t->currentCooldown <= 0 && *playerMetal >= 10) {
				int best = -1;
				for (i = 0; i < enemyCount; i++) {
					if (!enemies[i].active || enemies[i].hp <= 0) continue;
					dx=enemies[i].x-t->x; dy=enemies[i].y-t->y;
					if (dx*dx+dy*dy <= (double)t->range*t->range) { best=i; break; }
				}
				if (best >= 0) {
					*playerMetal -= 10;
					DamageEnemy(&enemies[best], t->damage, fTexts, fTextCount);
					if (*projCount < 300) {
						projs[*projCount].startX=t->x; projs[*projCount].startY=t->y;
						projs[*projCount].targetX=enemies[best].x; projs[*projCount].targetY=enemies[best].y;
						projs[*projCount].targetId=-1; projs[*projCount].damage=0;
						projs[*projCount].isHit=1; projs[*projCount].isCrow=0;
						projs[*projCount].life=0.1; projs[*projCount].maxLife=0.1;
						(*projCount)++;
					}
					t->currentCooldown = (t->levelA >= 1) ? 3.0 : t->cooldown;
				}
			}
			// --- 飞鸦 ---
			if (t->crowCurrentCooldown > 0)
				t->crowCurrentCooldown -= deltaTime * t->buffSpeedRatio;
			if (t->crowCurrentCooldown <= 0 && *playerWood >= 30) {
				int crowRange = 320;
				int best = -1;
				for (i = 0; i < enemyCount; i++) {
					if (!enemies[i].active || enemies[i].hp <= 0) continue;
					dx=enemies[i].x-t->x; dy=enemies[i].y-t->y;
					if (dx*dx+dy*dy <= (double)crowRange*crowRange) { best=i; break; }
				}
				if (best >= 0) {
					*playerWood -= 30;
					// B1：飞行途中伤害50
					if (t->levelB >= 1) DamageEnemy(&enemies[best], 50, fTexts, fTextCount);
					if (*projCount < 300) {
						projs[*projCount].startX=t->x; projs[*projCount].startY=t->y;
						projs[*projCount].targetX=enemies[best].x; projs[*projCount].targetY=enemies[best].y;
						projs[*projCount].targetId=enemies[best].id; projs[*projCount].damage=70;
						projs[*projCount].isHit=1; projs[*projCount].isCrow=1; // 标记为飞鸦
						projs[*projCount].life=0.5; projs[*projCount].maxLife=0.5;
						(*projCount)++;
					}
					t->crowCurrentCooldown = (t->levelA >= 1) ? 3.0 : t->crowCooldown;
				}
			}
			break;
		}
		
		// ============================================================
		case TOWER_PULSE: {
			if (t->currentCooldown > 0) break;
			int best = -1;
			for (i = 0; i < enemyCount; i++) {
				if (!enemies[i].active || enemies[i].hp <= 0) continue;
				dx=enemies[i].x-t->x; dy=enemies[i].y-t->y;
				if (dx*dx+dy*dy <= (double)t->range*t->range) { best=i; break; }
			}
			if (best < 0) break;
			
			// B2：切割模式
			if (t->levelB >= 2) {
				for (i = 0; i < enemyCount; i++) {
					if (!enemies[i].active || enemies[i].hp <= 0) continue;
					dx=enemies[i].x-t->x; dy=enemies[i].y-t->y;
					if (dx*dx+dy*dy <= (double)t->range*t->range)
						DamageEnemy(&enemies[i], t->damage, fTexts, fTextCount);
				}
				t->currentCooldown = 15.0;
				break;
			}
			
			// 连锁跳跃
			int prevId = -1;
			int curIdx = best;
			double dmg = t->damage;
			for (j = 0; j < 4; j++) { // 最多跳4次
				if (curIdx < 0) break;
				DamageEnemy(&enemies[curIdx], (int)dmg, fTexts, fTextCount);
				if (t->levelA >= 2) enemies[curIdx].stunTimer = 1.0;
				if (*projCount < 300) {
					projs[*projCount].startX = (j==0) ? t->x : enemies[prevId >= 0 ? prevId : curIdx].x;
					projs[*projCount].startY = (j==0) ? t->y : enemies[prevId >= 0 ? prevId : curIdx].y;
					projs[*projCount].targetX=enemies[curIdx].x; projs[*projCount].targetY=enemies[curIdx].y;
					projs[*projCount].targetId=-1; projs[*projCount].damage=0;
					projs[*projCount].isHit=1; projs[*projCount].isCrow=0;
					projs[*projCount].life=0.08; projs[*projCount].maxLife=0.08;
					(*projCount)++;
				}
				prevId = curIdx;
				if (t->levelA < 1) dmg *= 0.5; // A1升级后不衰减
				// 找下一个跳跃目标
				curIdx = -1;
				for (i = 0; i < enemyCount; i++) {
					if (i == prevId || !enemies[i].active || enemies[i].hp <= 0) continue;
					double jx=enemies[i].x-enemies[prevId].x, jy=enemies[i].y-enemies[prevId].y;
					if (jx*jx+jy*jy <= 80*80) { curIdx=i; break; }
				}
			}
			t->currentCooldown = t->cooldown;
			break;
		}
		
		// ============================================================
		case TOWER_GRAVITY: {
			// 基础：每帧持续减速50%范围内敌人（无CD）
			double gRange = (t->levelA >= 1) ? t->range * 1.3 : t->range;
			for (i = 0; i < enemyCount; i++) {
				if (!enemies[i].active || enemies[i].hp <= 0) continue;
				dx=enemies[i].x-t->x; dy=enemies[i].y-t->y;
				if (dx*dx+dy*dy <= gRange*gRange) {
					enemies[i].slowTimer = 0.15;
					enemies[i].slowRatio = 0.5;
				}
			}
			
			// A2：手动引爆（CD 15秒，由 gravityExplodeReady 标记触发）
			if (t->gravityExplodeReady && t->levelA >= 2 && t->currentCooldown <= 0) {
				for (i = 0; i < enemyCount; i++) {
					if (!enemies[i].active || enemies[i].hp <= 0) continue;
					dx=enemies[i].x-t->x; dy=enemies[i].y-t->y;
					if (dx*dx+dy*dy <= gRange*gRange) {
						DamageEnemy(&enemies[i], 180, fTexts, fTextCount);
						enemies[i].stunTimer = 2.0;
					}
				}
				t->gravityExplodeReady = 0;
				t->currentCooldown = 15.0;
			}
			
			// B1：反重力，持续4秒悬浮（stunTimer），范围224px，CD 8秒
			// B2：黑洞吞噬，CD 20秒（chargeTime复用为B路CD计时器）
			if (t->levelB >= 1) {
				if (t->timer2 > 0) {
					// 反重力持续中：持续刷新stunTimer
					t->timer2 -= deltaTime;
					double bRange = 224.0; // 7格
					for (i = 0; i < enemyCount; i++) {
						if (!enemies[i].active || enemies[i].hp <= 0) continue;
						dx=enemies[i].x-t->x; dy=enemies[i].y-t->y;
						if (dx*dx+dy*dy <= bRange*bRange) {
							enemies[i].stunTimer = 0.15; // 持续刷新
							// B2：黑洞吞噬——timer1复用为黑洞CD，chargeTime复用为在场计时
							if (t->levelB >= 2 && t->chargeTime <= 0) {
								enemies[i].hp = 0;
								AddFloatText(fTexts, fTextCount, enemies[i].x, enemies[i].y, -4);
							}
						}
					}
				} else {
					// 反重力冷却中
					if (t->currentCooldown > 0) {
						t->currentCooldown -= deltaTime;
					} else {
						// CD完毕，重新激活4秒悬浮
						t->timer2 = 4.0;
						t->currentCooldown = (t->levelB >= 2) ? 20.0 : 8.0;
						// B2：黑洞本次吞噬CD（吞噬期间不重复触发）
						if (t->levelB >= 2) t->chargeTime = 4.0;
					}
				}
				// B2 chargeTime倒计时
				if (t->chargeTime > 0) t->chargeTime -= deltaTime;
			}
			
			if (t->currentCooldown > 0 && t->levelA >= 1 && t->levelB == 0)
				t->currentCooldown -= deltaTime;
			break;
		}
		
		// ============================================================
	case TOWER_PORTAL:
		if (t->state != 1 || t->pairedIndex < 0) break; // 未激活/未配对
		if (t->portalRole != 1) break; // 只有B门（role=1）检测敌人进入
		
		if (t->portalCollapseCD > 0) { t->portalCollapseCD -= deltaTime; break; }
		
		for (i = 0; i < enemyCount; i++) {
			if (!enemies[i].active || enemies[i].hp <= 0) continue;
			dx=enemies[i].x-t->x; dy=enemies[i].y-t->y;
			if (dx*dx+dy*dy > 32*32) continue;
			
			// 检查传送次数上限
			int maxTele = (allTowers[t->pairedIndex].levelB >= 1) ? 2 : 1;
			if (enemies[i].portalCount >= maxTele) continue;
			
			// 传送到A门位置
			Tower* gateA = &allTowers[t->pairedIndex];
			enemies[i].x = gateA->x;
			enemies[i].y = gateA->y;
			enemies[i].stunTimer = 1.0;
			enemies[i].portalCount++;
			
			// A1：空间切割伤害
			if (gateA->levelA >= 1)
				DamageEnemy(&enemies[i], 240, fTexts, fTextCount);
			
			// 计数
			t->portalTeleCount++;
			gateA->portalTeleCount++;
			
			// B2：15个敌人后坍缩
			if (allTowers[t->pairedIndex].levelB >= 2 && t->portalTeleCount >= 15) {
				for (j = 0; j < enemyCount; j++) {
					if (!enemies[j].active) continue;
					dx=enemies[j].x-t->x; dy=enemies[j].y-t->y;
					double colR = 300;
					if (dx*dx+dy*dy <= colR*colR) {
						enemies[j].hp = 0;
						AddFloatText(fTexts, fTextCount, enemies[j].x, enemies[j].y, -4);
					}
				}
				t->portalTeleCount = 0;
				gateA->portalTeleCount = 0;
				t->portalCollapseCD = 30.0;
			}
		}
		break;
		
		// ============================================================
	case TOWER_TIMECLOCK:
		if (t->currentCooldown <= 0) {
			// 时间波刚触发：第一帧处理 A2/B1（一次性效果）
			for (i = 0; i < enemyCount; i++) {
				if (!enemies[i].active || enemies[i].hp <= 0) continue;
				dx=enemies[i].x-t->x; dy=enemies[i].y-t->y;
				if (dx*dx+dy*dy > (double)t->range*t->range) continue;
				// A2：时间裂隙——仅在触发瞬间回溯（只执行一次）
				if (t->levelA >= 2) {
					enemies[i].x = enemies[i].savedX;
					enemies[i].y = enemies[i].savedY;
					enemies[i].targetWaypointIndex = enemies[i].savedWaypointIndex;
				}
				// B1：因果律标记（仅在触发时加标记）
				if (t->levelB >= 1 && !enemies[i].causeMarkActive) {
					enemies[i].causeMarkActive  = 1;
					enemies[i].causeTimer       = 3.0;
					enemies[i].causeDamageAccum = 0;
				}
			}
			t->timeWaveTimer   = 2.0;
			t->currentCooldown = t->cooldown;
		}
		// 时间波持续期：A1静止 / 基础减速（每帧刷新）
		if (t->timeWaveTimer > 0) {
			t->timeWaveTimer -= deltaTime;
			for (i = 0; i < enemyCount; i++) {
				if (!enemies[i].active || enemies[i].hp <= 0) continue;
				dx=enemies[i].x-t->x; dy=enemies[i].y-t->y;
				if (dx*dx+dy*dy > (double)t->range*t->range) continue;
				if (t->levelA >= 1) {
					// A1：持续静止（A2也保持静止）
					enemies[i].stunTimer = 0.15;
				} else {
					// 基础/B路：减速40%
					enemies[i].slowTimer = 0.15;
					enemies[i].slowRatio = 0.6;
				}
			}
		}
		break;
		
		// ============================================================
		case TOWER_LASER: {
			if (t->laserPulseCooldown > 0) { t->laserPulseCooldown -= deltaTime; break; }
			
			// B1：单发模式（1.5秒/发）
			if (t->levelB >= 1) {
				if (t->currentCooldown > 0) break;
				int best = -1;
				for (i = 0; i < enemyCount; i++) {
					if (!enemies[i].active || enemies[i].hp <= 0) continue;
					dx=enemies[i].x-t->x; dy=enemies[i].y-t->y;
					if (dx*dx+dy*dy <= (double)t->range*t->range) { best=i; break; }
				}
				if (best < 0) break;
				DamageEnemy(&enemies[best], 150, fTexts, fTextCount);
				// B2：切割链，跳3次
				if (t->levelB >= 2) {
					int prev = best;
					for (j = 0; j < 3; j++) {
						int next = -1;
						for (i = 0; i < enemyCount; i++) {
							if (i==prev || !enemies[i].active || enemies[i].hp<=0) continue;
							double jx=enemies[i].x-enemies[prev].x, jy=enemies[i].y-enemies[prev].y;
							if (jx*jx+jy*jy <= 100*100) { next=i; break; }
						}
						if (next < 0) break;
						DamageEnemy(&enemies[next], 150, fTexts, fTextCount);
						prev = next;
					}
				}
				t->currentCooldown = 1.5;
				break;
			}
			
			// A2：超级脉冲
			if (t->levelA >= 2 && t->laserCharges >= 2) {
				int best = -1;
				for (i = 0; i < enemyCount; i++) {
					if (!enemies[i].active || enemies[i].hp <= 0) continue;
					dx=enemies[i].x-t->x; dy=enemies[i].y-t->y;
					if (dx*dx+dy*dy <= (double)t->range*t->range) { best=i; break; }
				}
				if (best >= 0) {
					DamageEnemy(&enemies[best], 800, fTexts, fTextCount);
					t->laserCharges = 0;
					t->laserPulseCooldown = 6.0;
					t->laserBeamTimer = 0.3;
				}
				break;
			}
			
			// 基础/A1：持续照射递增伤害
			// targetId 锁定目标
			if (t->targetId >= 0) {
				// 验证目标仍有效
				int found = 0;
				for (i = 0; i < enemyCount; i++) {
					if (enemies[i].id == t->targetId && enemies[i].active && enemies[i].hp > 0) {
						dx=enemies[i].x-t->x; dy=enemies[i].y-t->y;
						if (dx*dx+dy*dy <= (double)t->range*t->range) { found=1; break; }
					}
				}
				if (!found) { t->targetId=-1; t->laserBeamTimer=0; t->timer1=0; }
			}
			// 寻找新目标
			if (t->targetId < 0) {
				for (i = 0; i < enemyCount; i++) {
					if (!enemies[i].active || enemies[i].hp <= 0) continue;
					dx=enemies[i].x-t->x; dy=enemies[i].y-t->y;
					if (dx*dx+dy*dy <= (double)t->range*t->range) {
						t->targetId = enemies[i].id;
						t->timer1   = 0; // 照射时长重置
						t->laserBeamTimer = (t->levelA >= 1) ? 4.0 : 6.0;
						break;
					}
				}
			}
			// 持续照射结算
			if (t->targetId >= 0) {
				t->currentCooldown -= deltaTime;
				if (t->currentCooldown <= 0) {
					t->currentCooldown = 1.0; // 每秒结算一次
					t->timer1 += 1.0;
					double dmgMult = 1.0;
					for (j = 1; j < (int)t->timer1; j++) dmgMult *= 2.0;
					int dmg = (int)(t->damage * dmgMult);
					for (i = 0; i < enemyCount; i++) {
						if (enemies[i].id != t->targetId) continue;
						DamageEnemy(&enemies[i], dmg, fTexts, fTextCount);
						break;
					}
					// A1：储能计数
					if (t->levelA >= 1 && t->laserCharges < 2) t->laserCharges++;
				}
				t->laserBeamTimer -= deltaTime;
				if (t->laserBeamTimer <= 0) {
					// 照射时间耗尽，冷却3秒
					t->targetId = -1; t->timer1 = 0;
					t->currentCooldown = 3.0;
					t->laserBeamTimer  = 0;
				}
			}
			break;
		}
		
	} // end switch
}

// ============================================================
//  Tower_draw
// ============================================================
void Tower_draw(Tower* t, Texture2D* tex) {
	// tex 索引：TOWER_SLING=1..TOWER_LASER=13，用 t->type 直接索引
	// 每张贴图横排4帧，每帧32x32
	Rectangle destRec = { (float)(t->x - 16), (float)(t->y - 16), 32.0f, 32.0f };
	Vector2   origin  = { 0.0f, 0.0f };
	Rectangle srcRec  = { (float)(t->currentFrame * 32), 0.0f, 32.0f, 32.0f };
	
	// ---- 用贴图绘制塔体 ----
	if (t->type >= TOWER_SLING && t->type <= TOWER_LASER) {
		Texture2D tx = tex[t->type]; // 直接用枚举值索引
		Color tint = WHITE;
		// 骨矛阵耗尽时变半透明
		if (t->type == TOWER_BONESPEAR && t->ammo <= 0) tint = Fade(DARKGRAY, 0.45f);
		// 骨矛阵准备中变暗
		if (t->type == TOWER_BONESPEAR && t->ammo > 0 && t->state == 0) tint = Fade(WHITE, 0.55f);
		// 传送门未激活时半透明
		if (t->type == TOWER_PORTAL && t->state == 0) tint = Fade(WHITE, 0.5f);
		// 火堆熄灭时变暗
		if (t->type == TOWER_FIRE && t->state == 0) tint = Fade(WHITE, 0.35f);
		DrawTexturePro(tx, srcRec, destRec, origin, 0.0f, tint);
	}
	
	// ---- 叠加状态文字与特效 ----
	// 升级标签
	if (t->levelA > 0) DrawText("A", t->x+10, t->y-20, 10, GREEN);
	if (t->levelB > 0) DrawText("B", t->x+18, t->y-20, 10, SKYBLUE);
	
	// 各塔专属状态显示
	if (t->type == TOWER_SLING && t->ammo <= 0)
		DrawText("Empty", t->x-15, t->y-22, 10, RED);
	
	if (t->type == TOWER_BONESPEAR && t->ammo <= 0)
		DrawText("Rebuild", t->x-16, t->y-22, 10, (Color){200,150,100,255});
	
	if (t->type == TOWER_FIRE && t->state == 0)
		DrawText("No Wood", t->x-18, t->y-22, 10, GRAY);
	
	if (t->type == TOWER_CROSSBOW && t->state == 0) {
		double prog = (t->timer1 > 0) ? 1.0 - (t->timer1/3.0) : 1.0;
		DrawRectangle(t->x-12, t->y+16, 24, 3, DARKGRAY);
		DrawRectangle(t->x-12, t->y+16, (int)(24*prog), 3, GOLD);
	}
	
	if (t->type == TOWER_MINE) {
		if (t->ammo <= 0 && t->levelB >= 1 && t->reloadTimer > 0) {
			double prog = t->reloadTimer / 10.0;
			DrawRectangle(t->x-12, t->y+16, 24, 3, DARKGRAY);
			DrawRectangle(t->x-12, t->y+16, (int)(24*prog), 3, ORANGE);
		}
		if (t->ammo <= 0 && t->timer2 > 0) {
			float r = (float)(t->range * 1.5 * (1.0 - t->timer2/0.3));
			DrawCircleLines(t->x, t->y, r, Fade(ORANGE, (float)(t->timer2/0.3)));
		}
	}
	
	if (t->type == TOWER_CAULDRON && t->levelB >= 1)
		DrawText(TextFormat("%d", t->explosiveCharges), t->x-4, t->y-22, 12, ORANGE);
	
	if (t->type == TOWER_FIRECROW && t->crowCurrentCooldown > 0) {
		double prog = 1.0 - (t->crowCurrentCooldown / t->crowCooldown);
		DrawRectangle(t->x-12, t->y+16, 24, 3, DARKGRAY);
		DrawRectangle(t->x-12, t->y+16, (int)(24*prog), 3, ORANGE);
	}
	
	if (t->type == TOWER_PULSE && t->currentCooldown > 0) {
		double prog = 1.0-(t->currentCooldown/t->cooldown);
		DrawCircleLines(t->x, t->y, (int)(8+prog*12), Fade(SKYBLUE, (float)prog));
	}
	
	if (t->type == TOWER_GRAVITY) {
		double wave = fmod(GetTime()*1.5, 1.0);
		DrawCircleLines(t->x, t->y, (int)(t->range*wave), Fade(PURPLE, (float)(1.0-wave)));
	}
	
	if (t->type == TOWER_PORTAL) {
		DrawText(t->portalRole==0 ? "A" : "B", t->x-4, t->y-6, 12, WHITE);
		if (t->state == 0) DrawText("Wait", t->x-14, t->y+20, 10, PURPLE);
		if (t->portalCollapseCD > 0)
			DrawText(TextFormat("CD%.0f", t->portalCollapseCD), t->x-12, t->y-22, 9, RED);
	}
	
	if (t->type == TOWER_TIMECLOCK) {
		if (t->timeWaveTimer > 0) {
			double prog = 1.0-(t->timeWaveTimer/2.0);
			DrawCircleLines(t->x, t->y, (int)(t->range*prog), Fade(SKYBLUE,(float)(1.0-prog)));
		}
		if (t->currentCooldown > 0) {
			DrawRectangle(t->x-12, t->y+18, 24, 3, DARKGRAY);
			DrawRectangle(t->x-12, t->y+18, (int)(24*(1.0-t->currentCooldown/t->cooldown)), 3, SKYBLUE);
		}
	}
	
	if (t->type == TOWER_LASER) {
		if (t->laserBeamTimer > 0) {
			float inten = (float)(t->laserBeamTimer/6.0);
			DrawCircle(t->x, t->y-10, (int)(5+t->laserBeamTimer), Fade(RED, inten));
		}
		if (t->levelA >= 1)
			DrawText(TextFormat("[%d]", t->laserCharges), t->x-8, t->y+14, 10, GOLD);
	}
	
	// Buff 光环
	if (t->buffTimer > 0) {
		DrawCircleLines(t->x, t->y-25, 4, ORANGE);
		DrawCircleLines(t->x, t->y-25, 6, Fade(ORANGE, 0.5f));
	}
}


// ============================================================
//  激光塔光束（单独绘制，覆盖在所有塔上层）
// ============================================================
void Tower_drawLaserBeam(Tower* t, Enemy* enemies, int enemyCount) {
	if (t->type != TOWER_LASER || t->targetId < 0) return;
	int i;
	for (i = 0; i < enemyCount; i++) {
		if (enemies[i].id != t->targetId || !enemies[i].active) continue;
		// 激光束：深红细线+外发光
		DrawLineEx((Vector2){(float)t->x, (float)(t->y-16)},
				   (Vector2){(float)enemies[i].x, (float)enemies[i].y},
				   3.0f, Fade((Color){220,30,30,255}, 0.9f));
		DrawLineEx((Vector2){(float)t->x, (float)(t->y-16)},
				   (Vector2){(float)enemies[i].x, (float)enemies[i].y},
				   1.0f, (Color){255,180,180,255});
		break;
	}
}
