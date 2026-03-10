/**
 * @file    enemy.h
 * @brief   敌人模块：敌人实体的创建、路径移动、状态更新、死亡处理
 *          Enemy module: entity creation, path movement, state updates, death cleanup.
 * @author  主工程师 / Lead Engineer
 * @date    2026-03-10
 * @version 1.0
 */

#ifndef ENEMY_H
#define ENEMY_H

#include "raylib.h"
#include "../map/map.h"  /* 依赖 Point 类型与路点接口 / Depends on Point type and waypoint API */

/* ============================================================
 * 常量定义 | Constants
 * ============================================================ */

#define MAX_ENEMIES         200     /* 场上最大同屏敌人数 / Max enemies on screen */
#define ENEMY_RADIUS        15      /* 敌人圆形半径（像素）/ Enemy circle radius (pixels) */
#define ENEMY_HP_BAR_WIDTH  30      /* 血条宽度（像素）/ HP bar width (pixels) */
#define ENEMY_HP_BAR_OFFSET 25      /* 血条距圆心距离（像素）/ HP bar offset above center */

/* 敌人初始数值（来自 GAME_DESIGN.md，数值锁定后更新此处）
   Enemy initial values (from GAME_DESIGN.md — update here when values are locked) */
#define ENEMY_INITIAL_HP    100
#define ENEMY_SPEED_PPS     60.0f   /* 移动速度（像素/秒）/ Movement speed (pixels per second) */
#define ENEMY_KILL_REWARD   10      /* 击杀金币奖励 / Gold reward on kill */
#define ENEMY_HIT_FLASH_SEC 0.1f   /* 受击闪白持续时间（秒）/ Hit flash duration (seconds) */

/* ============================================================
 * 类型定义 | Type Definitions
 * ============================================================ */

/**
 * @brief 单个敌人的完整状态
 *        Complete state of a single enemy entity.
 */
typedef struct {
    float x;                    /* 当前像素横坐标 / Current pixel x position */
    float y;                    /* 当前像素纵坐标 / Current pixel y position */
    float speed;                /* 移动速度（像素/秒）/ Movement speed (pixels per second) */
    int   max_hp;               /* 最大血量 / Maximum HP */
    int   hp;                   /* 当前血量 / Current HP */
    int   target_waypoint_index;/* 下一个目标路点索引 / Next target waypoint index */
    int   active;               /* 1 = 存活中 / alive；0 = 已失效 / inactive */
    int   reached_base;         /* 1 = 已到达大本营 / reached base；0 = 未到达 */
    float hit_flash_timer;      /* 受击闪白倒计时（秒）/ Hit flash countdown (seconds) */
} EnemyState;

/* ============================================================
 * 公开接口 | Public API
 * ============================================================ */

/**
 * @brief  在指定起始位置初始化一个敌人实体
 *         Initialize an enemy entity at the given start position.
 * @param  e          指向敌人状态的指针，不得为 NULL / Pointer to EnemyState, must not be NULL
 * @param  start_pos  起始像素坐标（通常为 waypoints[0]）
 *                    Starting pixel position (usually waypoints[0])
 */
void enemy_init(EnemyState* e, Point start_pos);

/**
 * @brief  更新单个敌人的位置与状态（每帧调用）
 *         Update a single enemy's position and state (call every frame).
 * @param  e              指向敌人状态的指针，不得为 NULL / Pointer to EnemyState, must not be NULL
 * @param  dt             本帧时间差（秒）/ Frame delta time in seconds
 * @param  waypoints      路点数组指针 / Pointer to waypoints array
 * @param  waypoint_count 路点总数量 / Total waypoint count
 * @return 若敌人到达大本营返回 1，否则返回 0
 *         Returns 1 if enemy reached the base, 0 otherwise.
 */
int enemy_update(EnemyState* e, float dt, const Point* waypoints, int waypoint_count);

/**
 * @brief  渲染单个敌人（圆形本体 + 血条）
 *         Render a single enemy (circle body + HP bar).
 * @param  e  指向敌人状态的指针，不得为 NULL / Pointer to EnemyState, must not be NULL
 */
void enemy_draw(const EnemyState* e);

/**
 * @brief  对敌人造成伤害，触发受击闪白效果
 *         Deal damage to an enemy and trigger hit flash effect.
 * @param  e       指向敌人状态的指针，不得为 NULL / Pointer to EnemyState, must not be NULL
 * @param  damage  造成的伤害量 / Amount of damage to deal
 */
void enemy_take_damage(EnemyState* e, int damage);

/**
 * @brief  获取敌人当前路点索引（供 tower 模块优先级计算使用）
 *         Get enemy's current waypoint index (used by tower module for targeting priority).
 * @param  e  指向敌人状态的指针，不得为 NULL / Pointer to EnemyState, must not be NULL
 * @return 当前目标路点索引 / Current target waypoint index
 */
int enemy_get_waypoint_index(const EnemyState* e);

#endif /* ENEMY_H */
