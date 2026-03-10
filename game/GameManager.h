#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <graphics.h>
#include "Map.h"
#include "Enemy.h"
#include "Tower.h"
#include <vector>

// 【新增】浮动伤害数字
struct FloatingText {
	double x, y;
	int damage;
	double life;
};

// 【新增】视觉抛物线投射物 (石块)
struct VisualProjectile {
	double startX, startY;
	double targetX, targetY;
	double life;
	double maxLife;
};

enum GameState {
	MENU, PLAYING_LEVEL_1, PLAYING_LEVEL_2, PLAYING_LEVEL_3, 
	PLAYING_LEVEL_4, PLAYING_LEVEL_5, GAME_OVER, VICTORY
};

class GameManager {
public:
	GameState currentState;
	Map gameMap;
	std::vector<Enemy> enemies; 
	std::vector<Tower> towers; 
	
	// 特效数组
	std::vector<FloatingText> floatTexts;
	std::vector<VisualProjectile> projectiles;
	
	int baseHealth;
	int money;
	
	int currentWave;           
	int enemiesSpawned;        
	int enemiesToSpawn;        
	double spawnTimer;         
	double currentSpawnInterval;
	
	double waveDelayTimer;     
	bool isWaveActive;         
	double warningTimer;   
	int warningX, warningY;
	
	GameManager();
	void init();
	void startNextWave();      
	void processInput();
	void updateLogic(double deltaTime);
	void renderGraphics();
};

#endif
