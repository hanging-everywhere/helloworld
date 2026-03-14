#ifndef TOWER_H
#define TOWER_H

#include "raylib.h"
#include "Enemy.h"

// 【核心新增】：塔的具体种类枚举（对应你的设计文档）
enum TowerType {
	TOWER_NONE = 0,
	// 石器时代
	TOWER_SLING = 1,    // 投石索
	TOWER_BONESPEAR,    // 骨矛阵
	TOWER_TOTEM,        // 图腾
	TOWER_FIRE,         // 火堆
	// 古代文明
	TOWER_CROSSBOW = 5, // 诸葛连弩
	TOWER_MINE,         // 震天雷
	TOWER_ALCHEMY,      // 炼丹炉
	TOWER_CROW,         // 神火飞鸦
	// 科幻未来
	TOWER_PULSE = 9,    // 脉冲塔
	TOWER_GRAVITY,      // 重力塔
	TOWER_PORTAL,       // 传送门
	TOWER_TIME,         // 时间塔
	TOWER_LASER         // 激光塔
};

typedef struct {
	double x, y;
	int damage;
	double life;
} FloatingText;

typedef struct {
	double startX, startY;
	double targetX, targetY;
	double life;
	double maxLife;
} VisualProjectile;

typedef struct {
	int x, y;
	
	// --- 身份与科技树 ---
	int era;            // 时代 (0:石器, 1:古代, 2:未来)
	int type;           // 塔的具体类型 (enum TowerType)
	int levelA;         // 升级分支A (0-3)
	int levelB;         // 升级分支B (0-3)
	
	// --- 基础战斗属性 ---
	int range;
	int damage;
	double cooldown;
	double currentCooldown;
	
	// --- 【核心新增：万能通用寄存器】 ---
	int ammo;           // 弹药量/剩余次数（投石索、骨矛阵用）
	int state;          // 状态机标记（0:闲置, 1:上弦中, 2:燃烧中...）
	double timer1;      // 万能计时器1（伪装倒计时、图腾怒吼CD）
	double timer2;      // 万能计时器2
	int targetId;       // 持续锁定的目标ID（激光塔用）
	int pairedIndex;    // 关联的另一座塔的索引（传送门用）
	
} Tower;

// 注意初始化函数变了，现在需要传入时代和类型
void Tower_init(Tower* t, int startX, int startY, int era, int type);
void Tower_update(Tower* t, double deltaTime, 
				  Enemy* enemies, int enemyCount, 
				  Point* waypoints, 
				  VisualProjectile* projs, int* projCount, 
				  FloatingText* fTexts, int* fTextCount);
void Tower_draw(Tower* t);

#endif
