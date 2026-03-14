#include "GameManager.h"
#include <stdio.h>
#include <math.h>

void GameManager_init(GameManager* gm, int startLevel) {
	// 加载对应关卡的地图
	Map_init(&gm->gameMap, startLevel);
	
	gm->enemyCount = 0; 
	gm->towerCount = 0;
	gm->floatTextCount = 0; 
	gm->projectileCount = 0;
	
	gm->baseHealth = 10;    
	gm->money = 150;        
	gm->currentWave = 0;
	gm->isWaveActive = 0;
	gm->waveDelayTimer = 3.0; 
	gm->warningTimer = 0.0;
	
	// 根据初始关卡设定状态
	if (startLevel == 1) {
		gm->currentState = PLAYING_LEVEL_1;
		gm->currentEra = ERA_STONE;
	} else if (startLevel == 2) {
		gm->currentState = PLAYING_LEVEL_2;
		gm->currentEra = ERA_ANCIENT;
	} else if (startLevel == 3) {
		gm->currentState = PLAYING_LEVEL_3;
		gm->currentEra = ERA_FUTURE;
	}
	
	gm->skeletonTexture = LoadTexture("Skeleton.png"); 
}

void GameManager_startNextWave(GameManager* gm) {
	gm->currentWave++;
	gm->enemiesSpawned = 0;
	gm->isWaveActive = 1;
	
	if (gm->currentWave == 1) { gm->enemiesToSpawn = 5; gm->currentSpawnInterval = 2.0; } 
	else if (gm->currentWave == 2) { gm->enemiesToSpawn = 10; gm->currentSpawnInterval = 1.5; } 
	else if (gm->currentWave == 3) { gm->enemiesToSpawn = 15; gm->currentSpawnInterval = 1.0; } 
	else { 
		// 当前关卡波次打完，进行关卡跳转
		gm->isWaveActive = 0; 
		
		if (gm->currentState == PLAYING_LEVEL_1) {
			gm->currentState = PLAYING_LEVEL_2;
			gm->currentEra = ERA_ANCIENT;
			gm->currentWave = 0;
			gm->towerCount = 0; 
			gm->enemyCount = 0;
			Map_init(&gm->gameMap, 2); // 加载第二关地图
			
		} else if (gm->currentState == PLAYING_LEVEL_2) {
			gm->currentState = PLAYING_LEVEL_3;
			gm->currentEra = ERA_FUTURE;
			gm->currentWave = 0;
			gm->towerCount = 0; 
			gm->enemyCount = 0;
			Map_init(&gm->gameMap, 3); // 加载第三关地图
			
		} else if (gm->currentState == PLAYING_LEVEL_3) {
			gm->currentState = VICTORY; 
		}
		return; 
	}
	gm->spawnTimer = gm->currentSpawnInterval; 
}

void GameManager_processInput(GameManager* gm) {
	int mx, my, col, row;
	Point center;
	
	// 现在允许在这三个游玩状态下建塔
	if (gm->currentState != PLAYING_LEVEL_1 && gm->currentState != PLAYING_LEVEL_2 && gm->currentState != PLAYING_LEVEL_3) return;
	
	if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
		mx = GetMouseX();
		my = GetMouseY();
		col = mx / CELL_SIZE;
		row = my / CELL_SIZE; 
		
		if (row >= 0 && row < ROWS && col >= 0 && col < COLS) {
			if (gm->gameMap.grid[row][col] == TYPE_BUILDABLE) { 
				if (gm->money >= 50 && gm->towerCount < MAX_TOWERS) {
					gm->money -= 50;
					gm->gameMap.grid[row][col] = TYPE_OBSTACLE; 
					
					center = Map_getCenter(row, col);
					Tower_init(&gm->towers[gm->towerCount], center.x, center.y, gm->currentEra, TOWER_SLING);
					gm->towerCount++;
				} else if (gm->money < 50) {
					gm->warningTimer = 1.0; 
					gm->warningX = mx;
					gm->warningY = my;
				}
			}
		}
	}
}

void GameManager_updateLogic(GameManager* gm, double deltaTime) {
	int i; 
	
	if (gm->currentState == GAME_OVER || gm->currentState == VICTORY || gm->currentState == MENU || gm->currentState == LEVEL_TRANSITION) return;
	
	if (gm->baseHealth <= 0) { gm->currentState = GAME_OVER; return; }
	if (gm->warningTimer > 0) gm->warningTimer -= deltaTime;
	
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
	
	for (i = 0; i < gm->towerCount; i++) {
		Tower_update(&gm->towers[i], deltaTime, gm->enemies, gm->enemyCount, gm->gameMap.waypoints, gm->projectiles, &gm->projectileCount, gm->floatTexts, &gm->floatTextCount);
	}
	
	for (i = 0; i < gm->enemyCount; ) {
		Enemy_update(&gm->enemies[i], deltaTime, gm->gameMap.waypoints, gm->gameMap.waypointCount);
		if (gm->enemies[i].reachedBase || gm->enemies[i].hp <= 0) {
			if (gm->enemies[i].reachedBase) gm->baseHealth--;
			else gm->money += 10;
			
			gm->enemies[i] = gm->enemies[gm->enemyCount - 1]; 
			gm->enemyCount--; 
		} else i++; 
	}
	
	for (i = 0; i < gm->projectileCount; ) {
		gm->projectiles[i].life -= deltaTime;
		if (gm->projectiles[i].life <= 0) {
			gm->projectiles[i] = gm->projectiles[gm->projectileCount - 1];
			gm->projectileCount--;
		} else i++;
	}
	
	for (i = 0; i < gm->floatTextCount; ) {
		gm->floatTexts[i].life -= deltaTime;
		gm->floatTexts[i].y -= 30.0 * deltaTime;
		if (gm->floatTexts[i].life <= 0) {
			gm->floatTexts[i] = gm->floatTexts[gm->floatTextCount - 1];
			gm->floatTextCount--;
		} else i++;
	}
}

void GameManager_renderGraphics(GameManager* gm) {
	int i;
	int mx, my, hoverCol, hoverRow;
	Point c, basePos;
	int pulse;
	double prog, cx, cy;
	const char* infoText;
	const char* dmgText;
	int floatY;
	int currentLevelNum;
	
	BeginDrawing();
	ClearBackground(BLACK);
	
	Map_draw(&gm->gameMap);
	
	mx = GetMouseX();
	my = GetMouseY();
	hoverCol = mx / CELL_SIZE; 
	hoverRow = my / CELL_SIZE;
	
	if ((gm->currentState == PLAYING_LEVEL_1 || gm->currentState == PLAYING_LEVEL_2 || gm->currentState == PLAYING_LEVEL_3) && hoverRow >= 0 && hoverRow < ROWS && hoverCol >= 0 && hoverCol < COLS) {
		if (gm->gameMap.grid[hoverRow][hoverCol] == TYPE_BUILDABLE) {
			c = Map_getCenter(hoverRow, hoverCol);
			DrawRectangle(c.x - 14, c.y - 14, 28, 28, (Color){80, 80, 80, 255});
			DrawCircleLines(c.x, c.y, 150, LIGHTGRAY);
		}
	}
	
	for (i = 0; i < gm->towerCount; i++) Tower_draw(&gm->towers[i]);
	for (i = 0; i < gm->enemyCount; i++) Enemy_draw(&gm->enemies[i], gm->skeletonTexture);
	
	// 只有当地图里有路点时才画老家（防止切图报错）
	if (gm->gameMap.waypointCount > 0) {
		basePos = gm->gameMap.waypoints[gm->gameMap.waypointCount - 1];
		pulse = (int)(sin(GetTime() * 8.0) * 6); 
		
		DrawCircle(basePos.x, basePos.y, 35 + pulse, (Color){255, 80, 0, 255});
		DrawCircle(basePos.x, basePos.y, 25 + pulse/2, (Color){255, 160, 0, 255});
		DrawCircle(basePos.x, basePos.y, 15, (Color){255, 230, 100, 255});
	}
	
	for (i = 0; i < gm->projectileCount; i++) {
		prog = 1.0 - (gm->projectiles[i].life / gm->projectiles[i].maxLife);
		cx = gm->projectiles[i].startX + (gm->projectiles[i].targetX - gm->projectiles[i].startX) * prog;
		cy = gm->projectiles[i].startY + (gm->projectiles[i].targetY - gm->projectiles[i].startY) * prog;
		cy -= 80.0 * sin(prog * 3.1415926); 
		DrawCircle((int)cx, (int)cy, 5, (Color){180, 180, 180, 255});
	}
	
	DrawRectangle(0, 0, 1024, 40, (Color){30, 30, 30, 255});
	DrawLine(0, 40, 1024, 40, (Color){100, 100, 100, 255});
	
	currentLevelNum = (gm->currentState == PLAYING_LEVEL_1) ? 1 : ((gm->currentState == PLAYING_LEVEL_2) ? 2 : 3);
	infoText = TextFormat("Base HP: %d    Money: %d    Wave: %d/3    Level: %d", gm->baseHealth, gm->money, gm->currentWave, currentLevelNum);
	DrawText(infoText, 12, 12, 20, BLACK); 
	DrawText(infoText, 10, 10, 20, GOLD);  
	
	for (i = 0; i < gm->floatTextCount; i++) {
		dmgText = TextFormat("-%d", gm->floatTexts[i].damage);
		DrawText(dmgText, (int)gm->floatTexts[i].x + 1, (int)gm->floatTexts[i].y + 1, 20, BLACK);
		DrawText(dmgText, (int)gm->floatTexts[i].x, (int)gm->floatTexts[i].y, 20, RED);
	}
	
	if (gm->warningTimer > 0) {
		floatY = gm->warningY - 20 - (int)((1.0 - gm->warningTimer) * 30);
		DrawText("Need 50 Money!", gm->warningX - 40 + 2, floatY + 2, 20, BLACK);
		DrawText("Need 50 Money!", gm->warningX - 40, floatY, 20, RED);
	}
	
	if (!gm->isWaveActive && (gm->currentState == PLAYING_LEVEL_1 || gm->currentState == PLAYING_LEVEL_2 || gm->currentState == PLAYING_LEVEL_3) && gm->currentWave < 3) {
		DrawText(TextFormat("Next Wave in: %.1f s", gm->waveDelayTimer), 350, 350, 40, WHITE);
	}
	if (gm->currentState == GAME_OVER) {
		DrawText("GAME OVER", 300, 300, 80, RED);
		DrawText("The fire went out...", 350, 400, 30, LIGHTGRAY);
	}
	if (gm->currentState == VICTORY) {
		DrawText("YOU WIN!", 350, 300, 80, GREEN);
		DrawText("All ages conquered!", 350, 400, 30, LIGHTGRAY);
	}
	
	EndDrawing();
}
