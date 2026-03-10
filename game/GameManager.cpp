#include "GameManager.h"
#include <stdio.h>
#include <math.h>
#include <time.h>

GameManager::GameManager() {
	currentState = MENU;
}

void GameManager::init() {
	gameMap.init();
	baseHealth = 10;    
	money = 150;        
	currentWave = 0;
	isWaveActive = false;
	waveDelayTimer = 3.0; 
	warningTimer = 0.0;
	currentState = PLAYING_LEVEL_1; 
}

void GameManager::startNextWave() {
	currentWave++;
	enemiesSpawned = 0;
	isWaveActive = true;
	if (currentWave == 1) { enemiesToSpawn = 5; currentSpawnInterval = 2.0; } 
	else if (currentWave == 2) { enemiesToSpawn = 10; currentSpawnInterval = 1.5; } 
	else if (currentWave == 3) { enemiesToSpawn = 15; currentSpawnInterval = 1.0; } 
	else { isWaveActive = false; currentState = VICTORY; return; }
	spawnTimer = currentSpawnInterval; 
}

void GameManager::processInput() {
	if (currentState != PLAYING_LEVEL_1) return;
	while (mousemsg()) {
		mouse_msg msg = getmouse();
		if (msg.is_down() && msg.is_left()) {
			// 注意：因为顶部加了 40px 的 UI 栏，建造判断稍微下移，但网格坐标不受影响
			int col = msg.x / CELL_SIZE;
			int row = msg.y / CELL_SIZE;
			if (row >= 0 && row < ROWS && col >= 0 && col < COLS) {
				if (gameMap.grid[row][col] == 0) {
					if (money >= 50) {
						money -= 50;
						gameMap.grid[row][col] = 2; 
						Point center = gameMap.getCenter(row, col);
						towers.push_back(Tower(center.x, center.y));
					} else {
						warningTimer = 1.0; 
						warningX = msg.x;
						warningY = msg.y;
					}
				}
			}
		}
	}
}

void GameManager::updateLogic(double deltaTime) {
	if (currentState == GAME_OVER || currentState == VICTORY) return;
	
	if (currentState == PLAYING_LEVEL_1) {
		if (baseHealth <= 0) { currentState = GAME_OVER; return; }
		if (warningTimer > 0) warningTimer -= deltaTime;
		
		// 波次逻辑
		if (isWaveActive) {
			if (enemiesSpawned < enemiesToSpawn) {
				spawnTimer -= deltaTime;
				if (spawnTimer <= 0) {
					enemies.push_back(Enemy(gameMap.waypoints[0]));
					enemiesSpawned++;
					spawnTimer = currentSpawnInterval; 
				}
			} else if (enemies.empty()) {
				isWaveActive = false;
				waveDelayTimer = 5.0; 
			}
		} else {
			waveDelayTimer -= deltaTime;
			if (waveDelayTimer <= 0) startNextWave();
		}
		
		// 更新塔 (传入特效数组)
		for (size_t i = 0; i < towers.size(); i++) {
			towers[i].update(deltaTime, enemies, gameMap.waypoints, projectiles, floatTexts);
		}
		
		// 更新敌人
		for (auto it = enemies.begin(); it != enemies.end(); ) {
			it->update(deltaTime, gameMap.waypoints);
			if (it->reachedBase) {
				baseHealth--;
				it = enemies.erase(it); 
			} else if (it->hp <= 0) {
				money += 10;
				it = enemies.erase(it);
			} else { it++; }
		}
		
		// 【特效】更新抛物线石块
		for (auto it = projectiles.begin(); it != projectiles.end(); ) {
			it->life -= deltaTime;
			if (it->life <= 0) it = projectiles.erase(it);
			else it++;
		}
		
		// 【特效】更新浮动伤害数字
		for (auto it = floatTexts.begin(); it != floatTexts.end(); ) {
			it->life -= deltaTime;
			it->y -= 30.0 * deltaTime; // 向上飘
			if (it->life <= 0) it = floatTexts.erase(it);
			else it++;
		}
	}
}

void GameManager::renderGraphics() {
	cleardevice();
	
	// 1. 画地图底层
	gameMap.draw();
	
	// 2. 悬浮建造预览
	int mx, my; mousepos(&mx, &my);
	int hoverCol = mx / CELL_SIZE; int hoverRow = my / CELL_SIZE;
	if (currentState == PLAYING_LEVEL_1 && hoverRow >= 0 && hoverRow < ROWS && hoverCol >= 0 && hoverCol < COLS) {
		if (gameMap.grid[hoverRow][hoverCol] == 0) {
			Point c = gameMap.getCenter(hoverRow, hoverCol);
			setfillcolor(EGERGB(80, 80, 80));
			bar(c.x - 20, c.y - 20, c.x + 20, c.y + 20);
			setcolor(EGERGB(200, 200, 200));
			circle(c.x, c.y, 150); // 射程圈
		}
	}
	
	// 3. 画实体
	for (size_t i = 0; i < towers.size(); i++) towers[i].draw();
	for (size_t i = 0; i < enemies.size(); i++) enemies[i].draw();
	
	// 4. 【氛围特效】呼吸的篝火大本营
	Point basePos = gameMap.waypoints.back();
	double t = (double)clock() / CLOCKS_PER_SEC;
	int pulse = (int)(sin(t * 8.0) * 6); // 火焰跳动偏移量
	
	setfillcolor(EGERGB(255, 80, 0)); // 外层暗红火焰
	fillcircle(basePos.x, basePos.y, 35 + pulse);
	setfillcolor(EGERGB(255, 160, 0)); // 中层橙色火焰
	fillcircle(basePos.x, basePos.y, 25 + pulse/2);
	setfillcolor(EGERGB(255, 230, 100)); // 内层亮黄火心
	fillcircle(basePos.x, basePos.y, 15);
	
	// 5. 【特效】画抛物线石块
	for (size_t i = 0; i < projectiles.size(); i++) {
		double prog = 1.0 - (projectiles[i].life / projectiles[i].maxLife);
		double cx = projectiles[i].startX + (projectiles[i].targetX - projectiles[i].startX) * prog;
		double cy = projectiles[i].startY + (projectiles[i].targetY - projectiles[i].startY) * prog;
		// 减去 sin 计算出的高度，形成漂亮抛物线
		cy -= 80.0 * sin(prog * 3.1415926); 
		
		setfillcolor(EGERGB(180, 180, 180));
		fillcircle((int)cx, (int)cy, 5);
	}
	
	// 6. UI 状态栏 (置于顶层)
	setfillcolor(EGERGB(30, 30, 30));
	bar(0, 0, 1024, 40); // 顶部黑条
	setcolor(EGERGB(100, 100, 100));
	line(0, 40, 1024, 40);
	
	// 带阴影的精美文字字体
	char infoText[128];
	sprintf(infoText, "  Base HP: %d    Money: %d    Wave: %d/3", baseHealth, money, currentWave);
	setbkmode(TRANSPARENT); // 文字背景透明
	setfont(24, 0, "Consolas");
	
	setcolor(EGERGB(0, 0, 0)); // 黑阴影
	outtextxy(12, 12, infoText);
	setcolor(EGERGB(255, 215, 0)); // 金色主字
	outtextxy(10, 10, infoText);
	
	// 7. 【特效】浮动伤害飘字
	for (size_t i = 0; i < floatTexts.size(); i++) {
		char dmgText[16];
		sprintf(dmgText, "-%d", floatTexts[i].damage);
		setcolor(EGERGB(0, 0, 0));
		outtextxy((int)floatTexts[i].x + 1, (int)floatTexts[i].y + 1, dmgText);
		setcolor(EGERGB(255, 50, 50));
		outtextxy((int)floatTexts[i].x, (int)floatTexts[i].y, dmgText);
	}
	
	// 8. 资源不足提示
	if (warningTimer > 0) {
		int floatY = warningY - 20 - (int)((1.0 - warningTimer) * 30);
		setcolor(EGERGB(0, 0, 0));
		outtextxy(warningX - 40 + 2, floatY + 2, "资源不足!");
		setcolor(EGERGB(255, 50, 50));
		outtextxy(warningX - 40, floatY, "资源不足!");
	}
	
	// 9. 游戏结束/胜利提示
	if (!isWaveActive && currentState == PLAYING_LEVEL_1 && currentWave < 3) {
		char delayText[64]; sprintf(delayText, "Next Wave in: %.1f s", waveDelayTimer);
		setcolor(EGERGB(255, 255, 255)); setfont(40, 0, "Consolas");
		outtextxy(350, 350, delayText);
	}
	if (currentState == GAME_OVER) {
		setcolor(EGERGB(255, 0, 0)); setfont(80, 0, "Consolas"); outtextxy(300, 300, "GAME OVER");
		setfont(30, 0, "Consolas"); outtextxy(350, 400, "火种熄灭，文明陨落"); 
	}
	if (currentState == VICTORY) {
		setcolor(EGERGB(0, 255, 0)); setfont(60, 0, "Consolas"); outtextxy(250, 300, "LEVEL 1 CLEARED");
		setfont(30, 0, "Consolas"); outtextxy(320, 400, "先民挺过了野兽的侵袭...");
	}
}
