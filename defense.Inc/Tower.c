#include "Tower.h"
#include <math.h>

void Tower_init(Tower* t, int startX, int startY, int era, int type) {
	t->x = startX; t->y = startY;
	t->era = era; t->type = type;
	t->levelA = 0; t->levelB = 0;
	t->currentCooldown = 0.0;
	t->timer1 = 0.0; t->timer2 = 0.0;
	t->state = 0; t->targetId = -1;
	t->buffTimer = 0.0;
	t->buffSpeedRatio = 1.0;
	t->pairedIndex = 70; // 默认命中率70%（投石索用）
	
	t->currentFrame  = 0;
	t->framesCounter = 0;
	t->framesSpeed   = 15; 
	t->frameRec      = (Rectangle){ 0.0f, 0.0f, 32.0f, 32.0f }; 
	
	switch(type) {
	case TOWER_SLING:
		t->range = 150; t->damage = 20; t->cooldown = 2.0; t->ammo = 12;
		t->maxFrames = 4;
		t->pairedIndex = 70; // 命中率阈值
		break;
	case TOWER_BONESPEAR:
		t->range = 40; t->damage = 45; t->cooldown = 0.5; t->ammo = 3;
		t->timer1 = 3.0; t->state = 0;
		t->maxFrames = 4;
		break;
	case TOWER_TOTEM:
		t->range = 180; t->damage = 0; t->cooldown = 7.0;
		t->maxFrames = 1;
		break;
	case TOWER_FIRE:
		t->range = 70; t->damage = 15; t->cooldown = 2.0;
		t->maxFrames = 1;
		t->state = 1;
		t->timer1 = 0.0;
		t->timer2 = 0.0;
		break;
	default:
		t->range = 100; t->damage = 10; t->cooldown = 1.0;
		t->maxFrames = 1;
		break;
	}
}

void Tower_update(Tower* t, double deltaTime, 
				  Enemy* enemies, int enemyCount, 
				  Tower* allTowers, int towerCount, 
				  Point* waypoints, 
				  VisualProjectile* projs, int* projCount, 
				  FloatingText* fTexts, int* fTextCount,
				  int* playerWood, int* playerStone) {
	int i, bestEnemyIdx;
	double dx, dy;
	int isHit;
	
	// 序列帧动画
	if (t->maxFrames > 1) { 
		t->framesCounter++; 
		if (t->framesCounter >= t->framesSpeed) {
			t->framesCounter = 0; 
			t->currentFrame++;
			if (t->currentFrame >= t->maxFrames) t->currentFrame = 0; 
			t->frameRec.x = (float)t->currentFrame * t->frameRec.width;
		}
	}
	
	// Buff 处理
	if (t->buffTimer > 0) {
		t->buffTimer -= deltaTime;
		if (t->buffTimer <= 0) t->buffSpeedRatio = 1.0;
	}
	if (t->currentCooldown > 0) t->currentCooldown -= (deltaTime * t->buffSpeedRatio);
	
	switch(t->type) {
		// ============================================================
	case TOWER_SLING:
		// 需要石材开火
		if (t->currentCooldown <= 0 && t->ammo > 0 && (*playerStone) >= 5) {
			// 升级A2：每发消耗10石材
			int stoneCost = (t->levelA >= 2) ? 10 : 5;
			if ((*playerStone) < stoneCost) break;
			
			bestEnemyIdx = -1;
			for (i = 0; i < enemyCount; i++) {
				if (enemies[i].active && enemies[i].hp > 0) {
					dx = enemies[i].x - t->x; dy = enemies[i].y - t->y;
					if (dx*dx + dy*dy <= t->range * t->range) { bestEnemyIdx = i; break; }
				}
			}
			if (bestEnemyIdx != -1) {
				(*playerStone) -= stoneCost;
				t->ammo--;
				// 命中率：pairedIndex 存储阈值（70/85/100）
				isHit = (GetRandomValue(1, 100) <= t->pairedIndex) ? 1 : 0;
				t->currentCooldown = t->cooldown;
				
				projs[*projCount].startX = (double)t->x; projs[*projCount].startY = (double)t->y;
				projs[*projCount].isHit  = isHit;
				if (isHit) {
					projs[*projCount].targetX  = enemies[bestEnemyIdx].x;
					projs[*projCount].targetY  = enemies[bestEnemyIdx].y;
					projs[*projCount].targetId = enemies[bestEnemyIdx].id;
					projs[*projCount].damage   = t->damage;
				} else {
					int tx = enemies[bestEnemyIdx].x + GetRandomValue(-35, 35);
					int ty = enemies[bestEnemyIdx].y + GetRandomValue(-35, 35);
					if (tx < 10) tx = 10; if (tx > 1014) tx = 1014;
					if (ty < 10) ty = 10; if (ty > 758)  ty = 758;
					projs[*projCount].targetX  = tx; projs[*projCount].targetY = ty;
					projs[*projCount].targetId = -1; projs[*projCount].damage  = 0;
					if (*fTextCount < 300) {
						fTexts[*fTextCount].x = enemies[bestEnemyIdx].x;
						fTexts[*fTextCount].y = enemies[bestEnemyIdx].y - 20;
						fTexts[*fTextCount].damage = -1; fTexts[*fTextCount].life = 0.8;
						(*fTextCount)++;
					}
				}
				projs[*projCount].life = 0.2; projs[*projCount].maxLife = 0.2;
				(*projCount)++;
			}
		}
		break;
		
		// ============================================================
	case TOWER_BONESPEAR:
		if (t->ammo <= 0) break;
		if (t->state == 0) {
			t->timer1 -= deltaTime;
			if (t->timer1 <= 0) t->state = 1;
		} else if (t->state == 1 && t->currentCooldown <= 0) {
			for (i = 0; i < enemyCount; i++) {
				if (enemies[i].active && enemies[i].hp > 0) {
					dx = enemies[i].x - t->x; dy = enemies[i].y - t->y;
					if (dx*dx + dy*dy <= t->range * t->range) {
						// B1：三连爆（在范围内最多打3个）
						int hitCount = (t->levelB >= 1) ? 3 : 1;
						int j, hit = 0;
						for (j = i; j < enemyCount && hit < hitCount; j++) {
							if (!enemies[j].active || enemies[j].hp <= 0) continue;
							dx = enemies[j].x - t->x; dy = enemies[j].y - t->y;
							if (dx*dx + dy*dy > t->range * t->range) continue;
							
							enemies[j].hp -= t->damage;
							enemies[j].hitFlashTimer = 0.1;
							
							// B1：破隐身
							if (t->levelB >= 1) enemies[j].isInvisible = 0;
							
							// A1：减速20%
							if (t->levelA >= 1) {
								enemies[j].slowTimer = 2.0;
								enemies[j].slowRatio = 0.8;
							}
							// A2：附加流血DoT
							if (t->levelA >= 2) {
								enemies[j].dotTimer    = 3.0;
								enemies[j].dotTickTimer = 1.0;
								enemies[j].dotDamage   = 8;
							}
							
							if (*fTextCount < 300) {
								fTexts[*fTextCount].x = enemies[j].x; fTexts[*fTextCount].y = enemies[j].y - 20;
								fTexts[*fTextCount].damage = t->damage; fTexts[*fTextCount].life = 0.8;
								(*fTextCount)++;
							}
							hit++;
						}
						t->ammo--;
						t->currentCooldown = t->cooldown;
						break;
					}
				}
			}
		}
		break;
		
		// ============================================================
	case TOWER_TOTEM:
		if (t->currentCooldown <= 0) {
			if (t->levelA >= 1) {
				// A1：嗜血图腾——不再眩晕，改为纯鼓舞（友塔攻速+20%，持续3秒）
				for (i = 0; i < towerCount; i++) {
					if (allTowers[i].type != TOWER_TOTEM) {
						dx = allTowers[i].x - t->x; dy = allTowers[i].y - t->y;
						if (dx*dx + dy*dy <= t->range * t->range) {
							allTowers[i].buffTimer      = 3.0;
							allTowers[i].buffSpeedRatio = 1.2;
						}
					}
				}
			} else {
				// 基础版：只眩晕敌人，无友塔buff
				// B1：眩晕从3秒延长到5秒
				double stunDuration = (t->levelB >= 1) ? 5.0 : 3.0;
				for (i = 0; i < enemyCount; i++) {
					if (enemies[i].active && enemies[i].hp > 0) {
						dx = enemies[i].x - t->x; dy = enemies[i].y - t->y;
						if (dx*dx + dy*dy <= t->range * t->range) enemies[i].stunTimer = stunDuration;
					}
				}
			}
			
			// B2：每次怒吼附带范围伤害30（无论A/B路，只要B2）
			if (t->levelB >= 2) {
				for (i = 0; i < enemyCount; i++) {
					if (enemies[i].active && enemies[i].hp > 0) {
						dx = enemies[i].x - t->x; dy = enemies[i].y - t->y;
						if (dx*dx + dy*dy <= t->range * t->range) {
							enemies[i].hp -= 30;
							enemies[i].hitFlashTimer = 0.15;
							if (*fTextCount < 300) {
								fTexts[*fTextCount].x = enemies[i].x; fTexts[*fTextCount].y = enemies[i].y - 20;
								fTexts[*fTextCount].damage = 30; fTexts[*fTextCount].life = 0.8;
								(*fTextCount)++;
							}
						}
					}
				}
			}
			
			if (*fTextCount < 300) {
				fTexts[*fTextCount].x = t->x - 15; fTexts[*fTextCount].y = t->y - 40;
				fTexts[*fTextCount].damage = -3; fTexts[*fTextCount].life = 1.0;
				(*fTextCount)++;
			}
			t->timer2 = 0.5;
			t->currentCooldown = t->cooldown;
		}
		if (t->timer2 > 0) t->timer2 -= deltaTime;
		break;
		
		// ============================================================
	case TOWER_FIRE:
		// A2：神力不灭之火（state==2），跳过木材消耗
		if (t->levelA < 2) {
			t->timer1 += deltaTime;
			if (t->timer1 >= 1.0) {
				t->timer1 -= 1.0;
				if ((*playerWood) >= 1) {
					(*playerWood) -= 1;
					if (t->state == 0) t->state = 1;
				} else {
					t->state = 0;
				}
			}
		} else {
			// 神力模式：始终燃烧
			t->state = 2;
		}
		
		// 攻击
		if ((t->state == 1 || t->state == 2) && t->currentCooldown <= 0) {
			int triggered = 0;
			for (i = 0; i < enemyCount; i++) {
				if (enemies[i].active && enemies[i].hp > 0) {
					dx = enemies[i].x - t->x; dy = enemies[i].y - t->y;
					if (dx*dx + dy*dy <= t->range * t->range) {
						enemies[i].hp -= t->damage;
						enemies[i].hitFlashTimer = 0.1;
						enemies[i].dotTimer      = 3.0;
						enemies[i].dotTickTimer  = 1.0;
						enemies[i].dotDamage     = 5;
						enemies[i].isInvisible   = 0;
						
						// A1：点燃周围其他敌人（连锁燃烧3秒）
						if (t->levelA >= 1) {
							int k;
							for (k = 0; k < enemyCount; k++) {
								if (k == i || !enemies[k].active) continue;
								double ddx = enemies[k].x - enemies[i].x;
								double ddy = enemies[k].y - enemies[i].y;
								if (ddx*ddx + ddy*ddy <= 50*50) {
									enemies[k].dotTimer    = 3.0;
									enemies[k].dotTickTimer = 1.0;
									enemies[k].dotDamage   = 5;
								}
							}
						}
						
						triggered = 1;
						if (*fTextCount < 300) {
							fTexts[*fTextCount].x = enemies[i].x; fTexts[*fTextCount].y = enemies[i].y - 20;
							fTexts[*fTextCount].damage = t->damage; fTexts[*fTextCount].life = 0.8;
							(*fTextCount)++;
						}
					}
				}
			}
			if (triggered) {
				t->currentCooldown = t->cooldown;
				t->timer2 = 0.3;
			}
		}
		if (t->timer2 > 0) t->timer2 -= deltaTime;
		break;
	}
}

void Tower_draw(Tower* t, Texture2D slingTex, Texture2D spearTex) {
	Rectangle destRec = { (float)t->x, (float)t->y, 32.0f, 32.0f };
	Vector2 origin    = { 16.0f, 16.0f };
	
	if (t->era == 0) {
		// ---- 投石索 ----
		if (t->type == TOWER_SLING) {
			DrawTexturePro(slingTex, t->frameRec, destRec, origin, 0.0f, WHITE);
			if (t->ammo <= 0) DrawText("Empty", t->x-15, t->y-20, 10, RED);
			// 显示升级标记
			if (t->levelA > 0) DrawText("A", t->x+10, t->y-20, 10, GREEN);
			if (t->levelB > 0) DrawText("B", t->x+18, t->y-20, 10, SKYBLUE);
			
			// ---- 骨矛阵 ----
		} else if (t->type == TOWER_BONESPEAR) {
			if (t->ammo <= 0) {
				// 弹药耗尽：画废墟（折断的骨矛），提示玩家可点击重建
				DrawTexturePro(spearTex, t->frameRec, destRec, origin, 0.0f, Fade(DARKGRAY, 0.4f));
				DrawText("Rebuild", t->x-16, t->y-22, 10, (Color){200,150,100,255});
			} else {
				Color spearColor = (t->state == 0) ? Fade(DARKBROWN, 0.6f) : WHITE;
				// A1升级后骨矛涂黑（减速）
				if (t->levelA >= 1) spearColor = (t->state == 0) ? Fade(DARKBROWN, 0.6f) : (Color){40,40,40,255};
				DrawTexturePro(spearTex, t->frameRec, destRec, origin, 0.0f, spearColor);
				if (t->levelA > 0) DrawText("A", t->x+10, t->y-20, 10, GREEN);
				if (t->levelB > 0) DrawText("B", t->x+18, t->y-20, 10, SKYBLUE);
			}
			
			// ---- 图腾 ----
		} else if (t->type == TOWER_TOTEM) {
			// B1升级后顶部加骷髅头颜色变化
			Color poleColor = (t->levelB >= 1) ? (Color){60,10,10,255} : (Color){101,67,33,255};
			DrawRectangle(t->x-10, t->y-15, 20, 30, poleColor);
			DrawCircle(t->x, t->y-5, 6, (t->levelA >= 1) ? ORANGE : RED);
			// A1：光环显示为橙色（鼓舞）
			if (t->timer2 > 0) {
				double progress = 1.0 - (t->timer2 / 0.5);
				Color auraColor = (t->levelA >= 1) ? ORANGE : PURPLE;
				DrawCircleLines(t->x, t->y, t->range * progress, Fade(auraColor, 1.0 - progress));
			}
			if (t->currentCooldown > 0) {
				DrawRectangle(t->x-10, t->y+18, 20, 3, BLACK);
				DrawRectangle(t->x-10, t->y+18, (int)(20*(1.0-t->currentCooldown/t->cooldown)), 3, PURPLE);
			}
			if (t->levelA > 0) DrawText("A", t->x+10, t->y-22, 10, ORANGE);
			if (t->levelB > 0) DrawText("B", t->x+18, t->y-22, 10, RED);
			
			// ---- 火堆 ----
		} else if (t->type == TOWER_FIRE) {
			DrawCircle(t->x, t->y, 14, DARKGRAY);
			DrawCircle(t->x, t->y, 10, BLACK);
			
			int burning = (t->state == 1 || t->state == 2);
			if (burning) {
				double pulse = sin(GetTime() * 15.0) * 2.0;
				// A2：神力之火颜色变白蓝
				Color innerColor = (t->levelA >= 2) ? (Color){200,220,255,255} : ORANGE;
				Color coreColor  = (t->levelA >= 2) ? WHITE : YELLOW;
				DrawCircle(t->x, t->y+2, 8+pulse, innerColor);
				DrawCircle(t->x, t->y+2, 4+pulse/2, coreColor);
				if (t->timer2 > 0) {
					double progress = 1.0 - (t->timer2 / 0.3);
					DrawCircleLines(t->x, t->y, t->range * progress, Fade(RED, 1.0 - progress));
				}
			} else {
				DrawCircle(t->x, t->y+2, 5, GRAY);
				DrawText("Out of Wood", t->x-25, t->y-20, 10, GRAY);
			}
			if (t->levelA > 0) DrawText("A", t->x+10, t->y-20, 10, GREEN);
			if (t->levelB > 0) DrawText("B", t->x+18, t->y-20, 10, SKYBLUE);
		}
		
	} else {
		DrawRectangle(t->x-14, t->y-14, 28, 28, MAGENTA);
	}
	
	// Buff 光环
	if (t->buffTimer > 0) {
		DrawCircleLines(t->x, t->y-25, 4, ORANGE);
		DrawCircleLines(t->x, t->y-25, 6, Fade(ORANGE, 0.5f));
	}
}
