/**
 * @file    tower.h
 * @brief   炮塔模块：炮塔实体的初始化、目标选择、攻击逻辑、渲染
 *          Tower module: tower entity initialization, target selection, attack logic, rendering.
 * @author  副工程师 / Junior Engineer
 * @date    2026-03-10
 * @version 1.0
 */

#ifndef TOWER_H
#define TOWER_H

#include "raylib.h"
#include "../map/map.h"
#include "../enemy/enemy.h"
#include "../projectile/projectile.h"

/* ============================================================
 * 常量定义 | Constants
 * ============================================================ */

#define MAX_TOWERS          100     /* 场上最大炮塔数量 / Max towers on screen */

/* 炮塔初始属性（来自 GAME_DESIGN.md，数值锁定后更新此处）
   Tower initial stats (from GAME_DESIGN.md — update here when values are locked) */
#define TOWER_RANGE         150     /* 攻击射程（像素）/ Attack range (pixels) */
#define TOWER_DAMAGE        35      /* 单次攻击伤害 / Damage per attack */
#define TOWER_COOLDOWN_SEC  1.0f    /* 攻击冷却时间（秒）/ Attack cooldown (seconds) */
#define TOWER_BUILD_COST    50      /* 建造费用（金币）/ Build cost (gold) */
#define TOWER_SIZE          40      /* 炮塔方块边长（像素）/ Tower square side length (pixels) */

/* ============================================================
 * 类型定义 | Type Definitions
 * ============================================================ */

/**
 * @brief 单个炮塔的完整状态
 *        Complete state of a single tower entity.
 */
typedef struct {
    int   x;                /* 中心像素横坐标 / Center pixel x position */
    int   y;                /* 中心像素纵坐标 / Center pixel y position */
    int   range;            /* 攻击射程（像素）/ Attack range (pixels) */
    int   damage;           /* 单次攻击伤害 / Damage per attack */
    float cooldown;         /* 攻击冷却时间（秒）/ Attack cooldown duration (seconds) */
    float current_cooldown; /* 当前冷却剩余时间（秒）/ Current cooldown remaining (seconds) */
} Tower;

/* ============================================================
 * 公开接口 | Public API
 * ============================================================ */

/**
 * @brief  在指定像素坐标初始化一个炮塔
 *         Initialize a tower at the given pixel coordinates.
 * @param  t       指向炮塔结构体的指针，不得为 NULL / Pointer to Tower struct, must not be NULL
 * @param  center  炮塔中心像素坐标 / Tower center pixel coordinates
 */
void tower_init(Tower* t, Point center);

/**
 * @brief  更新炮塔逻辑（冷却计时、目标选择、攻击）
 *         Update tower logic (cooldown, target selection, attack).
 * @param  t           指向炮塔结构体的指针，不得为 NULL / Pointer to Tower struct, must not be NULL
 * @param  dt          本帧时间差（秒）/ Frame delta time in seconds
 * @param  enemies     敌人数组指针 / Pointer to enemy array
 * @param  enemy_count 当前敌人数量 / Current enemy count
 * @param  projs       抛射物数组指针 / Pointer to projectile array
 * @param  proj_count  抛射物数量指针 / Pointer to projectile count
 * @param  texts       飘字数组指针 / Pointer to floating text array
 * @param  text_count  飘字数量指针 / Pointer to floating text count
 */
void tower_update(Tower* t, float dt,
                  EnemyState* enemies, int enemy_count,
                  VisualProjectile* projs, int* proj_count,
                  FloatingText* texts, int* text_count);

/**
 * @brief  渲染单个炮塔（方块本体 + 炮口）
 *         Render a single tower (square body + cannon opening).
 * @param  t  指向炮塔结构体的指针，不得为 NULL / Pointer to Tower struct, must not be NULL
 */
void tower_draw(const Tower* t);

/**
 * @brief  渲染炮塔的悬停射程预览圈
 *         Render the tower range preview circle (shown on hover).
 * @param  t  指向炮塔结构体的指针，不得为 NULL / Pointer to Tower struct, must not be NULL
 */
void tower_draw_range_preview(const Tower* t);

#endif /* TOWER_H */
