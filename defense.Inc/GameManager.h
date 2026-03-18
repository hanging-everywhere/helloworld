#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "raylib.h"
#include "Map.h"
#include "Enemy.h"
#include "Tower.h"
#include "Base.h"

#define MAX_ENEMIES 200
#define MAX_TOWERS 100
#define MAX_PROJECTILES 300
#define MAX_FLOAT_TEXTS 300
#define MAX_TRAPS 100 

enum Era { ERA_STONE, ERA_ANCIENT, ERA_FUTURE };
enum GameState { MENU, PLAYING_LEVEL_1, PLAYING_LEVEL_2, PLAYING_LEVEL_3, LEVEL_TRANSITION, GAME_OVER, VICTORY };

typedef struct {
	double x, y;
	double life;
	int active;
} Trap;

typedef struct {
	enum GameState currentState;
	enum Era currentEra;          
	
	Map  gameMap;
	Base base;
	
	Enemy  enemies[MAX_ENEMIES]; 
	int    enemyCount;
	Tower  towers[MAX_TOWERS]; 
	int    towerCount;
	FloatingText     floatTexts[MAX_FLOAT_TEXTS];
	int              floatTextCount;
	VisualProjectile projectiles[MAX_PROJECTILES];
	int              projectileCount;
	Trap traps[MAX_TRAPS]; 
	
	int currentSelectedTower;
	int selectedTowerIndex;  // -1=无，>=0=选中已建塔
	int baseSelected;        // 1=点击的是基地（显示基地升级面板）
	
	int    baseHealth;       // 与 base.hp 同步
	int    currentWave;
	int    wood, stone, metal, material;
	double resourceTimer;
	int    enemiesSpawned, enemiesToSpawn, isWaveActive;        
	double spawnTimer, currentSpawnInterval, waveDelayTimer;     
	double warningTimer;   
	int    warningX, warningY;
	
	Texture2D bgMapTexture1;
	Texture2D skeletonTexture;
	Texture2D slingTexture;
	Texture2D bonespearTexture;
	
} GameManager;

void GameManager_init(GameManager* gm, int startLevel);
void GameManager_startNextWave(GameManager* gm);
void GameManager_loadAllTextures(GameManager* gm);
void GameManager_unloadAllTextures(GameManager* gm);
void GameManager_processInput(GameManager* gm);
void GameManager_updateLogic(GameManager* gm, double deltaTime);
void GameManager_renderGraphics(GameManager* gm);
void GameManager_upgradeTower(GameManager* gm, int path);

#endif
