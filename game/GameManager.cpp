#include "GameManager.h"
#include <stdio.h>
#include <math.h>
#include <time.h>

void GameManager_init(GameManager* gm) {
	Map_init(&gm->gameMap);
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
	while (mousemsg()) {
		mouse_msg msg = getmouse();
		if (msg.is_down() && msg.is_left()) {
			int col = msg.x / CELL_SIZE;
			int row = msg.y / CELL_SIZE;
			if (row >= 0 && row < ROWS && col >= 0 && col < COLS) {
				if (gm->gameMap.grid[row][col] == 0) {
					if (gm->money >= 50 && gm->towerCount < MAX_TOWERS) {
						gm->money -= 50;
						gm->gameMap.grid[row][col] = 2; 
						Point center = Map_getCenter(row, col);
						Tower_init(&gm->towers[gm->towerCount], center.x, center.y);
						gm->towerCount++;
					} else if (gm->money < 50) {
						gm->warningTimer = 1.0; 
						gm->warningX = msg.x;
						gm->warningY = msg.y;
					}
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
					gm->enemyCount++;
					gm->enemiesSpawned++;
					gm->spawnTimer = gm->currentSpawnInterval; 
				}
			} else if (gm->enemyCount == 0) {
				gm->isWaveActive = 0;
				gm->waveDelayTimer = 5.0; 
			}
		} else {
			gm->waveDelayTimer -= deltaTime;
			if (gm->waveDelayTimer <= 0) GameManager_startNextWave(gm);
		}
		
		for (i = 0; i < gm->towerCount; i++) {
			Tower_update(&gm->towers[i], deltaTime, 
						 gm->enemies, gm->enemyCount, 
						 gm->gameMap.waypoints, 
						 gm->projectiles, &gm->projectileCount, 
						 gm->floatTexts, &gm->floatTextCount);
		}
		
		for (i = 0; i < gm->enemyCount; ) {
			Enemy_update(&gm->enemies[i], deltaTime, gm->gameMap.waypoints, gm->gameMap.waypointCount);
			if (gm->enemies[i].reachedBase || gm->enemies[i].hp <= 0) {
				if (gm->enemies[i].reachedBase) gm->baseHealth--;
				else gm->money += 10;
				
				gm->enemies[i] = gm->enemies[gm->enemyCount - 1]; 
				gm->enemyCount--; 
			} else { 
				i++; 
			}
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
	cleardevice();
	
	// 1. 画地图底层
	Map_draw(&gm->gameMap);
	
	// 2. 【已恢复】悬浮建造预览与射程显示
	int mx, my; 
	mousepos(&mx, &my); // 获取当前鼠标坐标
	int hoverCol = mx / CELL_SIZE; 
	int hoverRow = my / CELL_SIZE;
	
	// 确保鼠标在合法地图范围内，并且当前在游戏进行中
	if (gm->currentState == PLAYING_LEVEL_1 && hoverRow >= 0 && hoverRow < ROWS && hoverCol >= 0 && hoverCol < COLS) {
		// 如果是空地，可以建塔
		if (gm->gameMap.grid[hoverRow][hoverCol] == 0) {
			Point c = Map_getCenter(hoverRow, hoverCol);
			
			// 画一个半透明/较暗的虚影方块代表塔
			setfillcolor(EGERGB(80, 80, 80));
			bar(c.x - 20, c.y - 20, c.x + 20, c.y + 20);
			
			// 画出白色的射程圆圈 (150 是我们在 Tower_init 里写的射程)
			setcolor(EGERGB(255, 255, 255));
			circle(c.x, c.y, 150); 
		}
	}
	
	// 3. 画实体（塔和敌人）
	for (i = 0; i < gm->towerCount; i++) Tower_draw(&gm->towers[i]);
	for (i = 0; i < gm->enemyCount; i++) Enemy_draw(&gm->enemies[i]);
	
	// 4. 大本营篝火动画
	Point basePos = gm->gameMap.waypoints[gm->gameMap.waypointCount - 1];
	double t = (double)clock() / CLOCKS_PER_SEC;
	int pulse = (int)(sin(t * 8.0) * 6); 
	
	setfillcolor(EGERGB(255, 80, 0)); 
	fillcircle(basePos.x, basePos.y, 35 + pulse);
	setfillcolor(EGERGB(255, 160, 0)); 
	fillcircle(basePos.x, basePos.y, 25 + pulse/2);
	setfillcolor(EGERGB(255, 230, 100)); 
	fillcircle(basePos.x, basePos.y, 15);
	
	// 5. 抛物线石块特效
	for (i = 0; i < gm->projectileCount; i++) {
		double prog = 1.0 - (gm->projectiles[i].life / gm->projectiles[i].maxLife);
		double cx = gm->projectiles[i].startX + (gm->projectiles[i].targetX - gm->projectiles[i].startX) * prog;
		double cy = gm->projectiles[i].startY + (gm->projectiles[i].targetY - gm->projectiles[i].startY) * prog;
		cy -= 80.0 * sin(prog * 3.1415926); 
		setfillcolor(EGERGB(180, 180, 180));
		fillcircle((int)cx, (int)cy, 5);
	}
	
	// 6. UI 顶部状态栏
	setfillcolor(EGERGB(30, 30, 30));
	bar(0, 0, 1024, 40);
	char infoText[128];
	sprintf(infoText, "  Base HP: %d    Money: %d    Wave: %d/3", gm->baseHealth, gm->money, gm->currentWave);
	setbkmode(TRANSPARENT); 
	setfont(24, 0, "Consolas");
	setcolor(EGERGB(255, 215, 0)); 
	outtextxy(10, 10, infoText);
	
	// 7. 伤害数字飘字
	for (i = 0; i < gm->floatTextCount; i++) {
		char dmgText[16];
		sprintf(dmgText, "-%d", gm->floatTexts[i].damage);
		setcolor(EGERGB(255, 50, 50));
		outtextxy((int)gm->floatTexts[i].x, (int)gm->floatTexts[i].y, dmgText);
	}
	
	// 8. 【已恢复】资源不足提示文字
	if (gm->warningTimer > 0) {
		int floatY = gm->warningY - 20 - (int)((1.0 - gm->warningTimer) * 30);
		setcolor(EGERGB(0, 0, 0));
		outtextxy(gm->warningX - 40 + 2, floatY + 2, "资源不足!");
		setcolor(EGERGB(255, 50, 50));
		outtextxy(gm->warningX - 40, floatY, "资源不足!");
	}
	
	// 9. 【已恢复】各种游戏状态结算提示
	if (!gm->isWaveActive && gm->currentState == PLAYING_LEVEL_1 && gm->currentWave < 3) {
		char delayText[64]; 
		sprintf(delayText, "Next Wave in: %.1f s", gm->waveDelayTimer);
		setcolor(EGERGB(255, 255, 255)); 
		setfont(40, 0, "Consolas");
		outtextxy(350, 350, delayText);
	}
	if (gm->currentState == GAME_OVER) {
		setcolor(EGERGB(255, 0, 0)); setfont(80, 0, "Consolas"); outtextxy(300, 300, "GAME OVER");
		setfont(30, 0, "Consolas"); outtextxy(350, 400, "火种熄灭，文明陨落"); 
	}
	if (gm->currentState == VICTORY) {
		setcolor(EGERGB(0, 255, 0)); setfont(60, 0, "Consolas"); outtextxy(250, 300, "LEVEL 1 CLEARED");
		setfont(30, 0, "Consolas"); outtextxy(320, 400, "先民挺过了野兽的侵袭...");
	}
}
