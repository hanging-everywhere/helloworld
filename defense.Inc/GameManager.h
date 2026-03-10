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

enum GameState { MENU, PLAYING_LEVEL_1, GAME_OVER, VICTORY };

typedef struct {
	enum GameState currentState;
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
} GameManager;

void GameManager_init(GameManager* gm);
void GameManager_startNextWave(GameManager* gm);
void GameManager_processInput(GameManager* gm);
void GameManager_updateLogic(GameManager* gm, double deltaTime);
void GameManager_renderGraphics(GameManager* gm);

#endif
