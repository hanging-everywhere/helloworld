#include "GameManager.h"
#include <stdio.h>
#include <math.h>

void GameManager_init(GameManager* gm) {
	Map_init(&gm->gameMap);
	gm->enemyCount = 0; gm->towerCount = 0;
	gm->floatTextCount = 0; gm->projectileCount = 0;
	
	gm->baseHealth = 10;    
	gm->money = 150;        
	gm->currentWave = 0;
	gm->isWaveActive = 0;
	gm->waveDelayTimer = 3.0; 
	gm->warningTimer = 0.0;
	gm->currentState = PLAYING_LEVEL_1; 
}

void GameManager_startNextWave(GameManager* gm) {
	gm->currentWave++;
	gm->enemiesSpawned = 0;
	gm->isWaveActive = 1;
	if (gm->currentWave == 1) { gm->enemiesToSpawn = 5; gm->currentSpawnInterval = 2.0; } 
	else if (gm->currentWave == 2) { gm->enemiesToSpawn = 10; gm->currentSpawnInterval = 1.5; } 
	else if (gm->currentWave == 3) { gm->enemiesToSpawn = 15; gm->currentSpawnInterval = 1.0; } 
	else { gm->isWaveActive = 0; gm->currentState = VICTORY; return; }
	gm->spawnTimer = gm->currentSpawnInterval; 
}

void GameManager_processInput(GameManager* gm) {
	if (gm->currentState != PLAYING_LEVEL_1) return;
	
	// Raylib 的输入检测极其简单清晰
	if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
		int mx = GetMouseX();
		int my = GetMouseY();
		int col = mx / CELL_SIZE;
		int row = my / CELL_SIZE;
		
		if (row >= 0 && row < ROWS && col >= 0 && col < COLS) {
			if (gm->gameMap.grid[row][col] == 0) { // 0 代表空地草地
				if (gm->money >= 50 && gm->towerCount < MAX_TOWERS) {
					gm->money -= 50;
					gm->gameMap.grid[row][col] = 2; // 2 标记为已有塔
					Point center = Map_getCenter(row, col);
					Tower_init(&gm->towers[gm->towerCount], center.x, center.y);
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
	if (gm->currentState == GAME_OVER || gm->currentState == VICTORY) return;
	
	if (gm->currentState == PLAYING_LEVEL_1) {
		if (gm->baseHealth <= 0) { gm->currentState = GAME_OVER; return; }
		if (gm->warningTimer > 0) gm->warningTimer -= deltaTime;
		
		if (gm->isWaveActive) {
			if (gm->enemiesSpawned < gm->enemiesToSpawn) {
				gm->spawnTimer -= deltaTime;
				if (gm->spawnTimer <= 0 && gm->enemyCount < MAX_ENEMIES) {
					Enemy_init(&gm->enemies[gm->enemyCount], gm->gameMap.waypoints[0]);
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
}

void GameManager_renderGraphics(GameManager* gm) {
	int i;
	
	// Raylib 必须在 BeginDrawing 和 EndDrawing 之间渲染
	BeginDrawing();
	ClearBackground(BLACK);
	
	// 1. 画地图底层
	Map_draw(&gm->gameMap);
	
	// 2. 悬浮建造预览 (Raylib 鼠标获取非常简单)
	int mx = GetMouseX();
	int my = GetMouseY();
	int hoverCol = mx / CELL_SIZE; 
	int hoverRow = my / CELL_SIZE;
	
	if (gm->currentState == PLAYING_LEVEL_1 && hoverRow >= 0 && hoverRow < ROWS && hoverCol >= 0 && hoverCol < COLS) {
		if (gm->gameMap.grid[hoverRow][hoverCol] == 0) {
			Point c = Map_getCenter(hoverRow, hoverCol);
			DrawRectangle(c.x - 20, c.y - 20, 40, 40, (Color){80, 80, 80, 255});
			DrawCircleLines(c.x, c.y, 150, LIGHTGRAY); // 射程圈
		}
	}
	
	// 3. 画实体
	for (i = 0; i < gm->towerCount; i++) Tower_draw(&gm->towers[i]);
	for (i = 0; i < gm->enemyCount; i++) Enemy_draw(&gm->enemies[i]);
	
	// 4. 大本营呼吸篝火特效
	Point basePos = gm->gameMap.waypoints[gm->gameMap.waypointCount - 1];
	// Raylib 提供了极简的 GetTime() 获取程序运行秒数
	int pulse = (int)(sin(GetTime() * 8.0) * 6); 
	
	DrawCircle(basePos.x, basePos.y, 35 + pulse, (Color){255, 80, 0, 255});
	DrawCircle(basePos.x, basePos.y, 25 + pulse/2, (Color){255, 160, 0, 255});
	DrawCircle(basePos.x, basePos.y, 15, (Color){255, 230, 100, 255});
	
	// 5. 抛物线石块
	for (i = 0; i < gm->projectileCount; i++) {
		double prog = 1.0 - (gm->projectiles[i].life / gm->projectiles[i].maxLife);
		double cx = gm->projectiles[i].startX + (gm->projectiles[i].targetX - gm->projectiles[i].startX) * prog;
		double cy = gm->projectiles[i].startY + (gm->projectiles[i].targetY - gm->projectiles[i].startY) * prog;
		cy -= 80.0 * sin(prog * 3.1415926); 
		DrawCircle((int)cx, (int)cy, 5, (Color){180, 180, 180, 255});
	}
	
	// 6. UI 状态栏 (置于顶层)
	DrawRectangle(0, 0, 1024, 40, (Color){30, 30, 30, 255}); // 顶部黑条
	DrawLine(0, 40, 1024, 40, (Color){100, 100, 100, 255});
	
	// Raylib 的文字渲染不需要处理背景透明，默认就是透明的，极其好用
	// 使用 TextFormat 可以直接像 printf 一样格式化字符串
	const char* infoText = TextFormat("Base HP: %d    Money: %d    Wave: %d/3", gm->baseHealth, gm->money, gm->currentWave);
	DrawText(infoText, 12, 12, 20, BLACK); // 黑阴影
	DrawText(infoText, 10, 10, 20, GOLD);  // 金色主字
	
	// 7. 浮动伤害飘字
	for (i = 0; i < gm->floatTextCount; i++) {
		const char* dmgText = TextFormat("-%d", gm->floatTexts[i].damage);
		DrawText(dmgText, (int)gm->floatTexts[i].x + 1, (int)gm->floatTexts[i].y + 1, 20, BLACK);
		DrawText(dmgText, (int)gm->floatTexts[i].x, (int)gm->floatTexts[i].y, 20, RED);
	}
	
	// 8. 资源不足提示
	if (gm->warningTimer > 0) {
		int floatY = gm->warningY - 20 - (int)((1.0 - gm->warningTimer) * 30);
		DrawText("Need 50 Money!", gm->warningX - 40 + 2, floatY + 2, 20, BLACK);
		DrawText("Need 50 Money!", gm->warningX - 40, floatY, 20, RED);
	}
	
	// 9. 游戏状态提示
	if (!gm->isWaveActive && gm->currentState == PLAYING_LEVEL_1 && gm->currentWave < 3) {
		DrawText(TextFormat("Next Wave in: %.1f s", gm->waveDelayTimer), 350, 350, 40, WHITE);
	}
	if (gm->currentState == GAME_OVER) {
		DrawText("GAME OVER", 300, 300, 80, RED);
		DrawText("The fire went out...", 350, 400, 30, LIGHTGRAY);
	}
	if (gm->currentState == VICTORY) {
		DrawText("LEVEL 1 CLEARED", 250, 300, 60, GREEN);
		DrawText("Humanity survived the night...", 320, 400, 30, LIGHTGRAY);
	}
	
	EndDrawing();
}
