#ifndef TOWER_H
#define TOWER_H
#include <graphics.h>
#include <vector>
#include "Enemy.h"

// 提前声明，防止互相包含报错
struct VisualProjectile;
struct FloatingText;

class Tower {
public:
	int x, y;
	int range;
	int damage;
	double cooldown;
	double currentCooldown;
	
	Tower(int startX, int startY);
	
	// 【修改】传入特效数组，用于生成石块和飘字
	void update(double deltaTime, std::vector<Enemy>& enemies, const std::vector<Point>& waypoints, 
				std::vector<VisualProjectile>& projectiles, std::vector<FloatingText>& floatTexts);
	void draw();
};
#endif
