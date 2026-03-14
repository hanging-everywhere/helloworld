#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "raylib.h"
#include "Map.h"
#include "Enemy.h"
#include "Tower.h"

#define MAX_ENEMIES 200
#define MAX_TOWERS 100
#define MAX_PROJECTILES 300
#define MAX_FLOAT_TEXTS 300

// 定义三个时代
enum Era { 
	ERA_STONE,      // 0: 石器时代
	ERA_ANCIENT,    // 1: 古代文明
	ERA_FUTURE      // 2: 科幻未来
};

// 扩展游戏状态
enum GameState { 
	MENU, 
	PLAYING_LEVEL_1, 
	PLAYING_LEVEL_2, 
	PLAYING_LEVEL_3, 
	LEVEL_TRANSITION,
	GAME_OVER, 
	VICTORY 
};

typedef struct {
	enum GameState currentState;
	enum Era currentEra;          // 记录当前时代
	
	Map gameMap;
	
	Enemy enemies[MAX_ENEMIES]; 
	int enemyCount;
	Tower towers[MAX_TOWERS]; 
	int towerCount;
	FloatingText floatTexts[MAX_FLOAT_TEXTS];
	int floatTextCount;
	VisualProjectile projectiles[MAX_PROJECTILES];
	int projectileCount;
	
	int baseHealth, money, currentWave;
	int enemiesSpawned, enemiesToSpawn, isWaveActive;        
	double spawnTimer, currentSpawnInterval, waveDelayTimer;     
	double warningTimer;   
	int warningX, warningY;
	
	Texture2D skeletonTexture; // 骷髅怪的贴图
} GameManager;

// 注意这里多了一个 startLevel 参数
void GameManager_init(GameManager* gm, int startLevel);
void GameManager_startNextWave(GameManager* gm);
void GameManager_processInput(GameManager* gm);
void GameManager_updateLogic(GameManager* gm, double deltaTime);
void GameManager_renderGraphics(GameManager* gm);

#endif
