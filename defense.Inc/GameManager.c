#include "GameManager.h"
#include <stdio.h>
#include <math.h>

void GameManager_loadAllTextures(GameManager* gm) {
	gm->bgMapTexture1    = LoadTexture("background(1).png"); 
	gm->skeletonTexture  = LoadTexture("Skeleton.png"); 
	gm->slingTexture     = LoadTexture("sling.png");
	gm->bonespearTexture = LoadTexture("bonespear.png");
}

void GameManager_unloadAllTextures(GameManager* gm) {
	UnloadTexture(gm->bgMapTexture1);
	UnloadTexture(gm->skeletonTexture);
	UnloadTexture(gm->slingTexture);
	UnloadTexture(gm->bonespearTexture);
}

void GameManager_init(GameManager* gm, int startLevel) {
	int i;
	Map_init(&gm->gameMap, startLevel);
	
	gm->enemyCount       = 0; 
	gm->towerCount       = 0;
	gm->floatTextCount   = 0; 
	gm->projectileCount  = 0;
	gm->selectedTowerIndex = -1;
	gm->baseSelected     = 0;
	
	for(i = 0; i < MAX_TRAPS; i++) gm->traps[i].active = 0;
	
	gm->wood = 50; gm->stone = 50; gm->metal = 0; gm->material = 0;
	gm->resourceTimer  = 0.0;      
	gm->currentWave    = 0;
	gm->isWaveActive   = 0;
	gm->waveDelayTimer = 3.0; 
	gm->warningTimer   = 0.0;
	gm->currentSelectedTower = TOWER_SLING;
	
	if      (startLevel == 1) { gm->currentState = PLAYING_LEVEL_1; gm->currentEra = ERA_STONE; }
	else if (startLevel == 2) { gm->currentState = PLAYING_LEVEL_2; gm->currentEra = ERA_ANCIENT; }
	else if (startLevel == 3) { gm->currentState = PLAYING_LEVEL_3; gm->currentEra = ERA_FUTURE; }
	
	GameManager_loadAllTextures(gm); 
	if (startLevel == 1) gm->gameMap.bgImg = gm->bgMapTexture1;
	
	// 初始化基地：位于地图最后一个路点
	// 路点在 Map_init 里已经设置好，这里取最后一个
	Point basePos = gm->gameMap.waypoints[gm->gameMap.waypointCount - 1];
	int era = (startLevel == 1) ? 0 : (startLevel == 2) ? 1 : 2;
	Base_init(&gm->base, basePos.x, basePos.y, era);
	gm->baseHealth = gm->base.hp;
}

void GameManager_startNextWave(GameManager* gm) {
	gm->currentWave++;
	gm->enemiesSpawned = 0;
	gm->isWaveActive   = 1;
	
	if      (gm->currentWave == 1) { gm->enemiesToSpawn = 5;  gm->currentSpawnInterval = 2.0; }
	else if (gm->currentWave == 2) { gm->enemiesToSpawn = 10; gm->currentSpawnInterval = 1.5; }
	else if (gm->currentWave == 3) { gm->enemiesToSpawn = 15; gm->currentSpawnInterval = 1.0; }
	else {
		gm->isWaveActive = 0;
		if (gm->currentState == PLAYING_LEVEL_1) {
			gm->currentState = PLAYING_LEVEL_2; gm->currentEra = ERA_ANCIENT; gm->currentWave = 0;
			gm->towerCount = 0; gm->enemyCount = 0; Map_init(&gm->gameMap, 2);
			Point bp = gm->gameMap.waypoints[gm->gameMap.waypointCount - 1];
			Base_init(&gm->base, bp.x, bp.y, 1);
			gm->baseHealth = gm->base.hp;
		} else if (gm->currentState == PLAYING_LEVEL_2) {
			gm->currentState = PLAYING_LEVEL_3; gm->currentEra = ERA_FUTURE; gm->currentWave = 0;
			gm->towerCount = 0; gm->enemyCount = 0; Map_init(&gm->gameMap, 3);
			Point bp = gm->gameMap.waypoints[gm->gameMap.waypointCount - 1];
			Base_init(&gm->base, bp.x, bp.y, 2);
			gm->baseHealth = gm->base.hp;
		} else if (gm->currentState == PLAYING_LEVEL_3) {
			gm->currentState = VICTORY;
		}
		return;
	}
	gm->spawnTimer = gm->currentSpawnInterval;
}

// ---- 升级塔逻辑（石器时代四塔）----
void GameManager_upgradeTower(GameManager* gm, int path) {
	if (gm->selectedTowerIndex < 0 || gm->selectedTowerIndex >= gm->towerCount) return;
	Tower* t = &gm->towers[gm->selectedTowerIndex];
	
	if (t->type == TOWER_SLING) {
		if (path == 0) {
			if (t->levelB > 0) { gm->warningTimer = 1.0; return; } // A/B互斥
			if (t->levelA == 0) {
				if (gm->wood < 30) { gm->warningTimer = 1.0; return; }
				gm->wood -= 30; t->levelA = 1; t->pairedIndex = 85;
			} else if (t->levelA == 1) {
				if (gm->wood < 30) { gm->warningTimer = 1.0; return; }
				gm->wood -= 30; t->levelA = 2;
				t->damage = (int)(t->damage * 1.5); t->range = (int)(t->range * 1.2);
			}
		} else {
			if (t->levelA > 0) { gm->warningTimer = 1.0; return; } // A/B互斥
			if (t->levelB == 0) {
				if (gm->stone < 40) { gm->warningTimer = 1.0; return; }
				gm->stone -= 40; t->levelB = 1;
				t->pairedIndex = 100;
				t->cooldown = (double)t->cooldown * 10.0 / 13.0; // 攻速+30%
			}
		}
	} else if (t->type == TOWER_BONESPEAR) {
		if (path == 0) {
			if (t->levelA == 0) {
				if (gm->wood < 20) { gm->warningTimer = 1.0; return; }
				gm->wood -= 20; t->levelA = 1;
			} else if (t->levelA == 1) {
				if (gm->wood < 30) { gm->warningTimer = 1.0; return; }
				gm->wood -= 30; t->levelA = 2;
			}
		} else {
			if (t->levelB == 0) {
				if (gm->wood < 50) { gm->warningTimer = 1.0; return; }
				gm->wood -= 50; t->levelB = 1;
			}
		}
	} else if (t->type == TOWER_TOTEM) {
		if (path == 0) {
			if (t->levelA == 0) {
				if (gm->wood < 30) { gm->warningTimer = 1.0; return; }
				gm->wood -= 30; t->levelA = 1;
			}
		} else {
			if (t->levelB == 0) {
				if (gm->wood < 40) { gm->warningTimer = 1.0; return; }
				gm->wood -= 40; t->levelB = 1;
			} else if (t->levelB == 1) {
				if (gm->wood < 60) { gm->warningTimer = 1.0; return; }
				gm->wood -= 60; t->levelB = 2;
			}
		}
	} else if (t->type == TOWER_FIRE) {
		if (path == 0) {
			if (t->levelA == 0) {
				if (gm->wood < 100) { gm->warningTimer = 1.0; return; }
				gm->wood -= 100; t->levelA = 1; t->damage = (int)(t->damage * 1.4);
			} else if (t->levelA == 1) {
				if (gm->wood < 150) { gm->warningTimer = 1.0; return; }
				gm->wood -= 150; t->levelA = 2;
				// state=2 由 Tower_update 在下一帧自动设置
			}
		} else {
			if (t->levelB == 0) {
				if (gm->stone < 50) { gm->warningTimer = 1.0; return; }
				gm->stone -= 50; t->levelB = 1;
			} else if (t->levelB == 1) {
				if (gm->stone < 80) { gm->warningTimer = 1.0; return; }
				gm->stone -= 80; t->levelB = 2;
			}
		}
	}
}

// ============================================================
//  输入处理
// ============================================================
void GameManager_processInput(GameManager* gm) {
	int mx, my, col, row, i, requiredType, occupied;
	Point center;
	
	if (gm->currentState != PLAYING_LEVEL_1 && gm->currentState != PLAYING_LEVEL_2 && gm->currentState != PLAYING_LEVEL_3) return;
	
	if (IsKeyPressed(KEY_ONE))   gm->currentSelectedTower = TOWER_SLING;
	if (IsKeyPressed(KEY_TWO))   gm->currentSelectedTower = TOWER_BONESPEAR;
	if (IsKeyPressed(KEY_THREE)) gm->currentSelectedTower = TOWER_TOTEM;
	if (IsKeyPressed(KEY_FOUR))  gm->currentSelectedTower = TOWER_FIRE;
	if (IsKeyPressed(KEY_ESCAPE)) { gm->selectedTowerIndex = -1; gm->baseSelected = 0; }
	
	
	
	if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
		mx = GetMouseX(); my = GetMouseY();
		
		// --- 升级面板按钮检测（塔）---
		if (gm->selectedTowerIndex >= 0) {
			Tower* sel = &gm->towers[gm->selectedTowerIndex];
			int panelX = sel->x + 24;
			int panelY = sel->y - 60;
			if (panelX + 160 > 1024) panelX = sel->x - 168;
			if (panelY < 65)          panelY = 65;
			Rectangle btnA = { panelX + 5, panelY + 44, 150, 22 };
			Rectangle btnB = { panelX + 5, panelY + 72, 150, 22 };
			if (CheckCollisionPointRec((Vector2){mx, my}, btnA)) { GameManager_upgradeTower(gm, 0); return; }
			if (CheckCollisionPointRec((Vector2){mx, my}, btnB)) { GameManager_upgradeTower(gm, 1); return; }
		}
		
		// --- 升级面板按钮检测（基地）---
		if (gm->baseSelected) {
			int panelX = gm->base.x + 50;
			int panelY = gm->base.y - 70;
			if (panelX + 170 > 1024) panelX = gm->base.x - 220;
			if (panelY < 65) panelY = 65;
			Rectangle btnA = { panelX + 5, panelY + 44, 160, 22 };
			Rectangle btnB = { panelX + 5, panelY + 72, 160, 22 };
			if (CheckCollisionPointRec((Vector2){mx, my}, btnA)) {
				int r = Base_upgrade(&gm->base, 0, &gm->wood, &gm->stone, &gm->metal, &gm->material);
				if (r == 1) gm->warningTimer = 1.0;
				return;
			}
			if (CheckCollisionPointRec((Vector2){mx, my}, btnB)) {
				int r = Base_upgrade(&gm->base, 1, &gm->wood, &gm->stone, &gm->metal, &gm->material);
				if (r == 1) gm->warningTimer = 1.0;
				return;
			}
		}
		
		col = mx / CELL_SIZE; row = my / CELL_SIZE;
		
		// --- 检测点击基地 ---
		{
			double ddx = mx - gm->base.x;
			double ddy = my - gm->base.y;
			if (ddx*ddx + ddy*ddy < 50*50) {
				gm->baseSelected = !gm->baseSelected;
				gm->selectedTowerIndex = -1;
				return;
			}
		}
		
		// --- 检测点击已有塔 ---
		int clickedExisting = 0;
		for (i = 0; i < gm->towerCount; i++) {
			double ddx = gm->towers[i].x - (col*CELL_SIZE + CELL_SIZE/2.0);
			double ddy = gm->towers[i].y - (row*CELL_SIZE + CELL_SIZE/2.0);
			if (ddx*ddx + ddy*ddy < 20*20) {
				gm->selectedTowerIndex = (gm->selectedTowerIndex == i) ? -1 : i;
				gm->baseSelected = 0;
				clickedExisting = 1;
				return;
			}
		}
		
		// --- 点击空地：建造 ---
		if (!clickedExisting) {
			gm->selectedTowerIndex = -1;
			gm->baseSelected = 0;
			
			int costWood = 0, costStone = 0;
			if      (gm->currentSelectedTower == TOWER_SLING)     costWood = 20;
			else if (gm->currentSelectedTower == TOWER_BONESPEAR) { costWood = 20; costStone = 30; }
			else if (gm->currentSelectedTower == TOWER_TOTEM)     costWood = 40;
			
			requiredType = (gm->currentSelectedTower == TOWER_BONESPEAR) ? TYPE_PATH : TYPE_BUILDABLE;
			
			if (row >= 0 && row < ROWS && col >= 0 && col < COLS) {
				if (gm->gameMap.grid[row][col] == requiredType) {
					center = Map_getCenter(row, col);
					occupied = 0;
					int replaceIndex = -1;
					for (i = 0; i < gm->towerCount; i++) {
						double d2 = (gm->towers[i].x-center.x)*(gm->towers[i].x-center.x)
						+ (gm->towers[i].y-center.y)*(gm->towers[i].y-center.y);
						if (d2 < 100) {
							if (gm->towers[i].type == TOWER_BONESPEAR && gm->towers[i].ammo <= 0)
								replaceIndex = i;
							else
								occupied = 1;
						}
					}
					if (!occupied) {
						int canAfford = (gm->wood >= costWood && gm->stone >= costStone);
						if (canAfford && gm->towerCount < MAX_TOWERS) {
							gm->wood -= costWood; gm->stone -= costStone;
							if (gm->currentSelectedTower != TOWER_BONESPEAR) gm->gameMap.grid[row][col] = TYPE_OBSTACLE;
							if (replaceIndex != -1)
								Tower_init(&gm->towers[replaceIndex], center.x, center.y, gm->currentEra, gm->currentSelectedTower);
							else {
								Tower_init(&gm->towers[gm->towerCount], center.x, center.y, gm->currentEra, gm->currentSelectedTower);
								gm->towerCount++;
							}
						} else if (!canAfford) {
							gm->warningTimer = 1.0; gm->warningX = mx; gm->warningY = my;
						}
					}
				}
			}
		}
	}
}

// ============================================================
//  逻辑更新
// ============================================================
void GameManager_updateLogic(GameManager* gm, double deltaTime) {
	int i, j; double dx, dy;
	
	if (gm->currentState == GAME_OVER || gm->currentState == VICTORY ||
		gm->currentState == MENU || gm->currentState == LEVEL_TRANSITION) return;
	
	// 基地 HP 归零判定（护盾激活时免疫）
	if (gm->base.hp <= 0) { gm->currentState = GAME_OVER; return; }
	gm->baseHealth = gm->base.hp; // 同步
	
	if (gm->warningTimer > 0) gm->warningTimer -= deltaTime;
	
	// 资源自然增长
	// 石器A1升级后产量×1.5
	double resMulti = (gm->currentEra == ERA_STONE && gm->base.levelA >= 1) ? 1.5 : 1.0;
	gm->resourceTimer += deltaTime;
	if (gm->currentEra == ERA_STONE) {
		if (gm->resourceTimer >= 5.0) {
			gm->wood  += (int)(3 * resMulti);
			gm->stone += (int)(3 * resMulti);
			gm->resourceTimer -= 5.0;
		}
	} else if (gm->currentEra == ERA_ANCIENT) {
		if (gm->resourceTimer >= 1.0) { gm->wood += 2; gm->stone += 2; gm->metal += 2; gm->resourceTimer -= 1.0; }
	} else if (gm->currentEra == ERA_FUTURE) {
		if (gm->resourceTimer >= 1.0) { gm->material += 7; gm->resourceTimer -= 1.0; }
	}
	
	// 刷怪
	if (gm->isWaveActive) {
		if (gm->enemiesSpawned < gm->enemiesToSpawn) {
			gm->spawnTimer -= deltaTime;
			if (gm->spawnTimer <= 0 && gm->enemyCount < MAX_ENEMIES) {
				Enemy_init(&gm->enemies[gm->enemyCount], gm->gameMap.waypoints[0], gm->enemiesSpawned);
				gm->enemyCount++; gm->enemiesSpawned++;
				gm->spawnTimer = gm->currentSpawnInterval;
			}
		} else if (gm->enemyCount == 0) {
			gm->isWaveActive = 0; gm->waveDelayTimer = 5.0;
		}
	} else {
		gm->waveDelayTimer -= deltaTime;
		if (gm->waveDelayTimer <= 0) GameManager_startNextWave(gm);
	}
	
	// 塔更新（石器A2：基地给所有友塔持续加攻速+30%）
	// 用独立的 baseBuff 标记，避免和图腾 buffTimer 冲突
	for (i = 0; i < gm->towerCount; i++) {
		if (gm->currentEra == ERA_STONE && gm->base.levelA >= 2) {
			// 直接叠加：如果当前 buffSpeedRatio 小于 1.3，则设为 1.3
			// 图腾的 buff（2.0）比它高会覆盖，图腾 buff 结束后这里再把底线补回来
			if (gm->towers[i].buffSpeedRatio < 1.3) {
				gm->towers[i].buffSpeedRatio = 1.3;
				gm->towers[i].buffTimer = 0.5; // 短续期，每半秒刷新一次
			}
		}
		Tower_update(&gm->towers[i], deltaTime, gm->enemies, gm->enemyCount,
					 gm->towers, gm->towerCount,
					 gm->gameMap.waypoints, gm->projectiles, &gm->projectileCount,
					 gm->floatTexts, &gm->floatTextCount, &gm->wood, &gm->stone);
	}
	
	// 基地更新
	Base_update(&gm->base, deltaTime, gm->enemies, gm->enemyCount,
				gm->floatTexts, &gm->floatTextCount,
				gm->projectiles, &gm->projectileCount);
	
	// 敌人更新
	for (i = 0; i < gm->enemyCount; ) {
		Enemy_update(&gm->enemies[i], deltaTime, gm->gameMap.waypoints, gm->gameMap.waypointCount);
		if (gm->enemies[i].reachedBase || gm->enemies[i].hp <= 0) {
			if (gm->enemies[i].reachedBase) {
				gm->base.hp--;
				gm->baseHealth = gm->base.hp;
			}
			gm->enemies[i] = gm->enemies[gm->enemyCount - 1]; gm->enemyCount--;
		} else i++;
	}
	
	// 子弹结算
	for (i = 0; i < gm->projectileCount; ) {
		gm->projectiles[i].life -= deltaTime;
		if (gm->projectiles[i].life <= 0) {
			if (!gm->projectiles[i].isHit) {
				for (j = 0; j < MAX_TRAPS; j++) {
					if (!gm->traps[j].active) {
						gm->traps[j].x = gm->projectiles[i].targetX; gm->traps[j].y = gm->projectiles[i].targetY;
						gm->traps[j].life = 5.0; gm->traps[j].active = 1; break;
					}
				}
			} else {
				for (j = 0; j < gm->enemyCount; j++) {
					if (gm->enemies[j].id == gm->projectiles[i].targetId && gm->enemies[j].active && gm->enemies[j].hp > 0) {
						gm->enemies[j].hp -= gm->projectiles[i].damage;
						gm->enemies[j].hitFlashTimer = 0.1;
						if (gm->floatTextCount < MAX_FLOAT_TEXTS) {
							gm->floatTexts[gm->floatTextCount].x      = gm->enemies[j].x;
							gm->floatTexts[gm->floatTextCount].y      = gm->enemies[j].y - 20;
							gm->floatTexts[gm->floatTextCount].damage = gm->projectiles[i].damage;
							gm->floatTexts[gm->floatTextCount].life   = 0.8;
							gm->floatTextCount++;
						}
						break;
					}
				}
			}
			gm->projectiles[i] = gm->projectiles[gm->projectileCount - 1]; gm->projectileCount--;
		} else i++;
	}
	
	// 绊脚石陷阱
	for (i = 0; i < MAX_TRAPS; i++) {
		if (gm->traps[i].active) {
			gm->traps[i].life -= deltaTime;
			if (gm->traps[i].life <= 0) { gm->traps[i].active = 0; continue; }
			for (j = 0; j < gm->enemyCount; j++) {
				if (gm->enemies[j].active && gm->enemies[j].stunTimer <= 0) {
					dx = gm->enemies[j].x - gm->traps[i].x; dy = gm->enemies[j].y - gm->traps[i].y;
					if (dx*dx + dy*dy < 15*15) {
						gm->enemies[j].stunTimer = 1.0; gm->traps[i].active = 0;
						if (gm->floatTextCount < MAX_FLOAT_TEXTS) {
							gm->floatTexts[gm->floatTextCount].x = gm->enemies[j].x;
							gm->floatTexts[gm->floatTextCount].y = gm->enemies[j].y - 20;
							gm->floatTexts[gm->floatTextCount].damage = -2;
							gm->floatTexts[gm->floatTextCount].life = 1.0;
							gm->floatTextCount++;
						}
						break;
					}
				}
			}
		}
	}
	
	// 飘字
	for (i = 0; i < gm->floatTextCount; ) {
		gm->floatTexts[i].life -= deltaTime; gm->floatTexts[i].y -= 30.0 * deltaTime;
		if (gm->floatTexts[i].life <= 0) {
			gm->floatTexts[i] = gm->floatTexts[gm->floatTextCount - 1]; gm->floatTextCount--;
		} else i++;
	}
}

// ============================================================
//  升级面板辅助
// ============================================================
static void GetUpgradeCost(Tower* t, int path, const char** outLabel, int* outAffordable, int wood, int stone) {
	*outLabel = ""; *outAffordable = 0;
	if (t->type == TOWER_SLING) {
		if (path == 0) {
			if      (t->levelA == 0) { *outLabel = "A1: Hit85%  -30Wood";  *outAffordable = (wood >= 30); }
			else if (t->levelA == 1) { *outLabel = "A2: BigRock -30Wood";  *outAffordable = (wood >= 30); }
			else                     { *outLabel = "A: MAX"; }
		} else {
			if (t->levelB == 0 && t->levelA == 0) { *outLabel = "B1: Hit100% -40Stone"; *outAffordable = (stone >= 40); }
			else if (t->levelB >= 1)               { *outLabel = "B: MAX"; }
			else                                    { *outLabel = "B: Need A=0"; }
		}
	} else if (t->type == TOWER_BONESPEAR) {
		if (path == 0) {
			if      (t->levelA == 0) { *outLabel = "A1: Slow20% -20Wood"; *outAffordable = (wood >= 20); }
			else if (t->levelA == 1) { *outLabel = "A2: Bleed   -30Wood"; *outAffordable = (wood >= 30); }
			else                     { *outLabel = "A: MAX"; }
		} else {
			if (t->levelB == 0) { *outLabel = "B1: Triple  -50Wood"; *outAffordable = (wood >= 50); }
			else                { *outLabel = "B: MAX"; }
		}
	} else if (t->type == TOWER_TOTEM) {
		if (path == 0) {
			if (t->levelA == 0) { *outLabel = "A1: BuffAtk -30Wood"; *outAffordable = (wood >= 30); }
			else                { *outLabel = "A: MAX"; }
		} else {
			if      (t->levelB == 0) { *outLabel = "B1: Stun5s  -40Wood"; *outAffordable = (wood >= 40); }
			else if (t->levelB == 1) { *outLabel = "B2: AoeDmg  -60Wood"; *outAffordable = (wood >= 60); }
			else                     { *outLabel = "B: MAX"; }
		}
	} else if (t->type == TOWER_FIRE) {
		if (path == 0) {
			if      (t->levelA == 0) { *outLabel = "A1: Dmg+40%  -100Wood";  *outAffordable = (wood >= 100); }
			else if (t->levelA == 1) { *outLabel = "A2: Eternal  -150Wood";  *outAffordable = (wood >= 150); }
			else                     { *outLabel = "A: MAX"; }
		} else {
			if      (t->levelB == 0) { *outLabel = "B1: Spray    -50Stone"; *outAffordable = (stone >= 50); }
			else if (t->levelB == 1) { *outLabel = "B2: Firewall -80Stone"; *outAffordable = (stone >= 80); }
			else                     { *outLabel = "B: MAX"; }
		}
	}
}

static const char* GetTowerName(int type) {
	if (type == TOWER_SLING)     return "Sling";
	if (type == TOWER_BONESPEAR) return "BoneSpear";
	if (type == TOWER_TOTEM)     return "Totem";
	if (type == TOWER_FIRE)      return "Fire Pit";
	return "Unknown";
}

// ---- 基地升级面板绘制 ----
static void DrawBaseUpgradePanel(GameManager* gm) {
	Base* b = &gm->base;
	int panelW = 175, panelH = 115;
	int panelX = b->x + 50;
	int panelY = b->y - 70;
	if (panelX + panelW > 1024) panelX = b->x - panelW - 20;
	if (panelY < 65)             panelY = 65;
	if (panelY + panelH > 768)   panelY = 768 - panelH - 4;
	
	DrawRectangle(panelX, panelY, panelW, panelH, (Color){10,10,40,230});
	DrawRectangleLines(panelX, panelY, panelW, panelH, GOLD);
	
	const char* baseName = (b->era == 0) ? "Base: Fire Shrine" : (b->era == 1) ? "Base: Mech Tower" : "Base: Core";
	DrawText(baseName, panelX+6, panelY+6, 13, GOLD);
	DrawText(TextFormat("HP: %d/%d  LvA:%d LvB:%d", b->hp, b->maxHp, b->levelA, b->levelB), panelX+6, panelY+23, 11, LIGHTGRAY);
	
	const char* labelA; int affordA;
	Base_getUpgradeLabel(b, 0, &labelA, &affordA, gm->wood, gm->stone, gm->metal, gm->material);
	Color colA = affordA ? GREEN : DARKGRAY;
	DrawRectangle(panelX+5, panelY+44, panelW-10, 22, (Color){30,30,30,255});
	DrawRectangleLines(panelX+5, panelY+44, panelW-10, 22, colA);
	DrawText(labelA, panelX+8, panelY+49, 10, colA);
	
	const char* labelB; int affordB;
	Base_getUpgradeLabel(b, 1, &labelB, &affordB, gm->wood, gm->stone, gm->metal, gm->material);
	Color colB = affordB ? SKYBLUE : DARKGRAY;
	DrawRectangle(panelX+5, panelY+72, panelW-10, 22, (Color){30,30,30,255});
	DrawRectangleLines(panelX+5, panelY+72, panelW-10, 22, colB);
	DrawText(labelB, panelX+8, panelY+77, 10, colB);
	
	DrawText("[ESC] Close", panelX+6, panelY+100, 10, (Color){100,100,100,255});
}

// ============================================================
//  渲染
// ============================================================
void GameManager_renderGraphics(GameManager* gm) {
	int i, mx, my, hoverCol, hoverRow, floatY, reqType;
	Point c;
	double prog, cx, cy;
	const char *infoText, *dmgText, *uiText;
	Color textColor;
	
	BeginDrawing();
	ClearBackground(BLACK);
	Map_draw(&gm->gameMap);
	
	mx = GetMouseX(); my = GetMouseY();
	hoverCol = mx / CELL_SIZE; hoverRow = my / CELL_SIZE;
	
	// 建造预览（无升级面板时显示）
	if ((gm->currentState == PLAYING_LEVEL_1 || gm->currentState == PLAYING_LEVEL_2 || gm->currentState == PLAYING_LEVEL_3)
		&& gm->selectedTowerIndex < 0 && !gm->baseSelected
		&& hoverRow >= 0 && hoverRow < ROWS && hoverCol >= 0 && hoverCol < COLS) {
		reqType = (gm->currentSelectedTower == TOWER_BONESPEAR) ? TYPE_PATH : TYPE_BUILDABLE;
		if (gm->gameMap.grid[hoverRow][hoverCol] == reqType) {
			c = Map_getCenter(hoverRow, hoverCol);
			if      (gm->currentSelectedTower == TOWER_SLING)     { DrawRectangle(c.x-14,c.y-14,28,28,(Color){80,80,80,255}); DrawCircleLines(c.x,c.y,150,LIGHTGRAY); }
			else if (gm->currentSelectedTower == TOWER_BONESPEAR) { DrawRectangle(c.x-14,c.y-14,28,28,Fade(RED,0.5f)); DrawCircleLines(c.x,c.y,40,RED); }
			else if (gm->currentSelectedTower == TOWER_TOTEM)     { DrawRectangle(c.x-14,c.y-14,28,28,Fade(PURPLE,0.5f)); DrawCircleLines(c.x,c.y,180,PURPLE); }
			else if (gm->currentSelectedTower == TOWER_FIRE)      { DrawRectangle(c.x-14,c.y-14,28,28,Fade(ORANGE,0.5f)); DrawCircleLines(c.x,c.y,70,ORANGE); }
		}
	}
	
	// 选中塔高亮
	if (gm->selectedTowerIndex >= 0) {
		Tower* sel = &gm->towers[gm->selectedTowerIndex];
		DrawRectangleLines(sel->x-16, sel->y-16, 32, 32, YELLOW);
	}
	
	// 基地选中高亮
	if (gm->baseSelected) DrawCircleLines(gm->base.x, gm->base.y, 52, YELLOW);
	
	// 塔、敌人
	for (i = 0; i < gm->towerCount; i++) Tower_draw(&gm->towers[i], gm->slingTexture, gm->bonespearTexture);
	for (i = 0; i < gm->enemyCount; i++) Enemy_draw(&gm->enemies[i], gm->skeletonTexture);
	
	// 基地绘制（替代之前的三圆圈）
	Base_draw(&gm->base);
	
	// 陷阱
	for (i = 0; i < MAX_TRAPS; i++) {
		if (gm->traps[i].active) {
			DrawCircle(gm->traps[i].x, gm->traps[i].y, 4, GRAY);
			DrawCircleLines(gm->traps[i].x, gm->traps[i].y, 4, DARKGRAY);
		}
	}
	
	// 子弹
	for (i = 0; i < gm->projectileCount; i++) {
		prog = 1.0 - (gm->projectiles[i].life / gm->projectiles[i].maxLife);
		cx = gm->projectiles[i].startX + (gm->projectiles[i].targetX - gm->projectiles[i].startX) * prog;
		cy = gm->projectiles[i].startY + (gm->projectiles[i].targetY - gm->projectiles[i].startY) * prog;
		cy -= 80.0 * sin(prog * 3.1415926);
		DrawCircle((int)cx, (int)cy, 5, (Color){180,180,180,255});
	}
	
	// 升级面板（塔）
	if (gm->selectedTowerIndex >= 0 && gm->selectedTowerIndex < gm->towerCount) {
		Tower* sel = &gm->towers[gm->selectedTowerIndex];
		int panelW = 165, panelH = 115;
		int panelX = sel->x + 24;
		int panelY = sel->y - 60;
		if (panelX + panelW > 1024) panelX = sel->x - panelW - 8;
		if (panelY < 65)             panelY = 65;
		if (panelY + panelH > 768)   panelY = 768 - panelH - 4;
		
		DrawRectangle(panelX, panelY, panelW, panelH, (Color){20,20,20,220});
		DrawRectangleLines(panelX, panelY, panelW, panelH, GOLD);
		DrawText(GetTowerName(sel->type), panelX+6, panelY+6, 14, GOLD);
		DrawText(TextFormat("Lv A:%d  B:%d", sel->levelA, sel->levelB), panelX+6, panelY+24, 12, LIGHTGRAY);
		
		const char* labelA; int affordA;
		GetUpgradeCost(sel, 0, &labelA, &affordA, gm->wood, gm->stone);
		Color colA = affordA ? GREEN : DARKGRAY;
		DrawRectangle(panelX+5, panelY+44, panelW-10, 22, (Color){40,40,40,255});
		DrawRectangleLines(panelX+5, panelY+44, panelW-10, 22, colA);
		DrawText(labelA, panelX+8, panelY+49, 10, colA);
		
		const char* labelB; int affordB;
		GetUpgradeCost(sel, 1, &labelB, &affordB, gm->wood, gm->stone);
		Color colB = affordB ? SKYBLUE : DARKGRAY;
		DrawRectangle(panelX+5, panelY+72, panelW-10, 22, (Color){40,40,40,255});
		DrawRectangleLines(panelX+5, panelY+72, panelW-10, 22, colB);
		DrawText(labelB, panelX+8, panelY+77, 10, colB);
		DrawText("[ESC] Close", panelX+6, panelY+100, 10, (Color){120,120,120,255});
	}
	
	// 升级面板（基地）
	if (gm->baseSelected) DrawBaseUpgradePanel(gm);
	
	// 顶部 UI 条
	DrawRectangle(0, 0, 1024, 60, (Color){30,30,30,255});
	DrawLine(0, 60, 1024, 60, (Color){100,100,100,255});
	
	if      (gm->currentEra == ERA_STONE)   infoText = TextFormat("HP:%d  Wood:%d  Stone:%d  Wave:%d/3", gm->base.hp, gm->wood, gm->stone, gm->currentWave);
	else if (gm->currentEra == ERA_ANCIENT) infoText = TextFormat("HP:%d  Wood:%d  Stone:%d  Metal:%d  Wave:%d/3", gm->base.hp, gm->wood, gm->stone, gm->metal, gm->currentWave);
	else                                    infoText = TextFormat("HP:%d  Material:%d  Wave:%d/3", gm->base.hp, gm->material, gm->currentWave);
	DrawText(infoText, 12, 10, 20, BLACK);
	DrawText(infoText, 10,  8, 20, GOLD);
	
	if      (gm->currentSelectedTower == TOWER_SLING)     uiText = "[1]Sling(20W)  [2]BoneSpear(20W+30S)  [3]Totem(40W)  [4]FirePit  | Click tower/base to upgrade";
	else if (gm->currentSelectedTower == TOWER_BONESPEAR) uiText = "[1]Sling(20W)  [2]BoneSpear(20W+30S)  [3]Totem(40W)  [4]FirePit  | Click tower/base to upgrade";
	else if (gm->currentSelectedTower == TOWER_TOTEM)     uiText = "[1]Sling(20W)  [2]BoneSpear(20W+30S)  [3]Totem(40W)  [4]FirePit  | Click tower/base to upgrade";
	else                                                   uiText = "[1]Sling(20W)  [2]BoneSpear(20W+30S)  [3]Totem(40W)  [4]FirePit  | Click tower/base to upgrade";
	DrawText(uiText, 10, 35, 14, RAYWHITE);
	
	if (gm->warningTimer > 0) {
		floatY = gm->warningY - 20 - (int)((1.0 - gm->warningTimer) * 30);
		DrawText("Not Enough Resources!", gm->warningX-40+2, floatY+2, 20, BLACK);
		DrawText("Not Enough Resources!", gm->warningX-40,   floatY,   20, RED);
	}
	
	// 飘字
	for (i = 0; i < gm->floatTextCount; i++) {
		if      (gm->floatTexts[i].damage == -1) { dmgText = "Miss!"; textColor = LIGHTGRAY; }
		else if (gm->floatTexts[i].damage == -2) { dmgText = "Stun!"; textColor = YELLOW; }
		else if (gm->floatTexts[i].damage == -3) { dmgText = "ROAR!"; textColor = PURPLE; }
		else { dmgText = TextFormat("-%d", gm->floatTexts[i].damage); textColor = RED; }
		DrawText(dmgText, (int)gm->floatTexts[i].x+1, (int)gm->floatTexts[i].y+1, 20, BLACK);
		DrawText(dmgText, (int)gm->floatTexts[i].x,   (int)gm->floatTexts[i].y,   20, textColor);
	}
	
	if (!gm->isWaveActive && (gm->currentState == PLAYING_LEVEL_1 || gm->currentState == PLAYING_LEVEL_2 || gm->currentState == PLAYING_LEVEL_3) && gm->currentWave < 3) {
		DrawText(TextFormat("Next Wave in: %.1f s", gm->waveDelayTimer), 350, 350, 40, WHITE);
	}
	if (gm->currentState == GAME_OVER) { DrawText("GAME OVER", 300, 300, 80, RED); DrawText("The base was destroyed...", 310, 400, 28, LIGHTGRAY); }
	if (gm->currentState == VICTORY)   { DrawText("YOU WIN!", 350, 300, 80, GREEN); DrawText("All ages conquered!", 350, 400, 30, LIGHTGRAY); }
	
	EndDrawing();
}
