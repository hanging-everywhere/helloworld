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
	
	Map gameMap;
	Base base;
	
	Enemy enemies[MAX_ENEMIES]; 
	int enemyCount;
	Tower towers[MAX_TOWERS]; 
	int towerCount;
	FloatingText floatTexts[MAX_FLOAT_TEXTS];
	int floatTextCount;
	VisualProjectile projectiles[MAX_PROJECTILES];
	int projectileCount;
	Trap traps[MAX_TRAPS];
	BurnZone burnZones[50];
	int burnZoneCount;
	
	// 记录玩家当前选中的塔类型
	int currentSelectedTower;
	int selectedTowerIndex;
	int baseSelected;
	int portalPendingIndex;
	
	int baseHealth, currentWave;
	int wood;       // 木材
	int stone;      // 石材
	int metal;      // 金属
	int material;   // 未来材料
	double resourceTimer; // 用于计算自然资源增长的计时器
	int enemiesSpawned, enemiesToSpawn, isWaveActive;        
	double spawnTimer, currentSpawnInterval, waveDelayTimer;     
	double warningTimer;   
	int warningX, warningY;
	
	// --- 【集中管理的贴图资源库】 ---
	Texture2D bgMapTexture1;    // 第一关地图背景
	Texture2D bgMapTexture2;    // 第二关地图背景
	Texture2D bgMapTexture3;    // 第三关地图背景
	Texture2D skeletonTexture;  // 骷髅兵
	Texture2D slingTexture;     // 投石索
	Texture2D bonespearTexture; // 骨矛阵（保留兼容）
	Texture2D towerTextures[14]; // [1..13] 对应 13 种塔
	
} GameManager;

// 游戏生命周期函数
void GameManager_init(GameManager* gm, int startLevel);
void GameManager_startNextWave(GameManager* gm);

// --- 【新增：统一管理图片的函数声明】 ---
void GameManager_loadAllTextures(GameManager* gm);
void GameManager_unloadAllTextures(GameManager* gm);

// 核心循环函数
void GameManager_processInput(GameManager* gm);
void GameManager_updateLogic(GameManager* gm, double deltaTime);
void GameManager_renderGraphics(GameManager* gm);
void GameManager_upgradeTower(GameManager* gm, int path);

#endif
