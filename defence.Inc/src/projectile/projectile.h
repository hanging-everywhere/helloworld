/**
 * @file    projectile.h
 * @brief   子弹/抛射物模块：视觉抛射物、伤害飘字、燃烧区域的数据结构和管理接口
 *          Projectile module: visual projectiles, floating damage text, burn zones.
 * @author  主工程师 / Lead Engineer
 * @date    2026-03-10
 * @version 2.0  扩展自 v1，新增 BurnZone、target_id、is_crow 等字段
 *               Extended from v1, added BurnZone, target_id, is_crow fields.
 *
 * @note  本模块仅管理视觉数据与简单的生命周期。伤害结算在 game.c 中完成。
 *        This module manages visual data and simple lifecycle. Damage is resolved in game.c.
 */

#ifndef PROJECTILE_H
#define PROJECTILE_H

/* ============================================================
 * 常量定义 | Constants
 * ============================================================ */

#define MAX_PROJECTILES     300     /* 最大同屏抛射物数量 / Max projectiles on screen */
#define MAX_FLOAT_TEXTS     300     /* 最大同屏飘字数量 / Max floating texts on screen */
#define MAX_BURN_ZONES      50      /* 最大同屏燃烧区域数量 / Max burn zones on screen */

#define PROJ_RADIUS         5       /* 抛射物圆形半径（像素）/ Projectile circle radius */
#define PROJ_ARC_HEIGHT     80.0f   /* 普通抛物线弧高（像素）/ Normal arc height */
#define PROJ_CROW_ARC       120.0f  /* 飞鸦抛物线弧高（像素）/ Firecrow arc height */
#define PROJ_DEFAULT_LIFE   0.2f    /* 默认飞行时间（秒）/ Default flight duration */

#define FLOAT_TEXT_SPEED    30.0f   /* 飘字上升速度（像素/秒）/ Rise speed (pixels/second) */
#define FLOAT_TEXT_LIFE     0.8f    /* 飘字存续时间（秒）/ Float text duration (seconds) */
#define FLOAT_TEXT_FONT_SZ  20      /* 飘字字体大小 / Float text font size */

/* ============================================================
 * 类型定义 | Type Definitions
 * ============================================================ */

/**
 * @brief 视觉抛射物（从炮塔/基地飞向目标，支持延迟伤害）
 *        Visual projectile (from tower/base to target, supports deferred damage).
 */
typedef struct {
    float start_x, start_y;  /* 发射起始坐标 / Launch origin */
    float target_x, target_y;/* 目标落点坐标 / Target landing */
    float life;               /* 剩余飞行时间（秒）/ Remaining flight time */
    float max_life;           /* 最大飞行时间（秒）/ Max flight time */
    int   target_id;          /* 目标敌人 ID（-1=无）/ Target enemy ID (-1=none) */
    int   damage;             /* 到达时造成的伤害（0=纯视觉）/ Damage on arrival (0=visual only) */
    int   is_hit;             /* 1=命中弹道 / 0=未命中弹道（落地变陷阱）/ 1=hit / 0=miss */
    int   is_crow;            /* 1=飞鸦弹（更高弧线）/ 1=firecrow (higher arc) */
} VisualProjectile;

/**
 * @brief 伤害飘字（击中时显示，向上飘动后消失）
 *        Floating damage text (shown on hit, floats upward then fades).
 */
typedef struct {
    float x, y;    /* 当前坐标（y 向上减少）/ Current position (y decreases upward) */
    int   damage;  /* 伤害数值（特殊值：-1=Miss -2=Stun -3=ROAR -4=VOID）/ Damage value */
    float life;    /* 剩余显示时间（秒）/ Remaining display time */
} FloatingText;

/**
 * @brief 燃烧区域（飞鸦落地产生，持续造成 DoT）
 *        Burn zone (created by firecrow landing, deals persistent DoT).
 */
typedef struct {
    float x, y;        /* 中心坐标 / Center position */
    float life;         /* 剩余燃烧时间（秒）/ Remaining burn time */
    float tick_timer;   /* 每秒跳一次的计时器 / Per-second tick timer */
    int   damage;       /* 每秒伤害量 / Damage per second */
    int   active;       /* 1=激活 / active；0=空闲 / inactive */
} BurnZone;

/* ============================================================
 * 公开接口 | Public API
 * ============================================================ */

/**
 * @brief  生成一个视觉抛射物（纯视觉，无延迟伤害）
 *         Spawn a visual-only projectile (no deferred damage).
 * @param  projs       抛射物数组 / Projectile array
 * @param  proj_count  当前数量指针 / Pointer to current count
 * @param  sx, sy      起点 / Start position
 * @param  tx, ty      终点 / End position
 */
void projectile_spawn(VisualProjectile* projs, int* proj_count,
                      float sx, float sy, float tx, float ty);

/**
 * @brief  生成一个完整配置的抛射物（支持延迟伤害和各种类型）
 *         Spawn a fully configured projectile (supports deferred damage and all types).
 * @param  projs       抛射物数组 / Projectile array
 * @param  proj_count  当前数量指针 / Pointer to current count
 * @param  sx, sy      起点 / Start position
 * @param  tx, ty      终点 / End position
 * @param  target_id   目标敌人 ID（延迟伤害用，-1=无）/ Target ID (-1 for visual only)
 * @param  damage      到达时伤害（0=无）/ Damage on arrival (0 for none)
 * @param  is_hit      是否命中弹道 / Hit trajectory flag
 * @param  is_crow     是否飞鸦弹 / Firecrow flag
 * @param  life        飞行时间（秒）/ Flight duration (seconds)
 */
void projectile_spawn_full(VisualProjectile* projs, int* proj_count,
                           float sx, float sy, float tx, float ty,
                           int target_id, int damage, int is_hit, int is_crow,
                           float life);

/**
 * @brief  生成一个伤害飘字
 *         Spawn a floating damage text.
 * @param  texts       飘字数组 / Float text array
 * @param  text_count  当前数量指针 / Pointer to current count
 * @param  x, y        显示初始位置 / Initial display position
 * @param  damage      伤害数值 / Damage value
 */
void floattext_spawn(FloatingText* texts, int* text_count,
                     float x, float y, int damage);

/**
 * @brief  更新所有活跃抛射物（swap-remove 过期条目）
 *         Update all active projectiles (swap-remove expired entries).
 * @param  projs       抛射物数组 / Projectile array
 * @param  proj_count  当前数量指针 / Pointer to current count
 * @param  dt          帧时差（秒）/ Frame delta time
 */
void projectile_update_all(VisualProjectile* projs, int* proj_count, float dt);

/**
 * @brief  更新所有活跃飘字
 *         Update all active floating texts.
 * @param  texts       飘字数组 / Float text array
 * @param  text_count  当前数量指针 / Pointer to current count
 * @param  dt          帧时差（秒）/ Frame delta time
 */
void floattext_update_all(FloatingText* texts, int* text_count, float dt);

/**
 * @brief  渲染所有活跃抛射物
 *         Render all active visual projectiles.
 * @param  projs       抛射物数组（const）/ Const projectile array
 * @param  proj_count  当前数量 / Current count
 */
void projectile_draw_all(const VisualProjectile* projs, int proj_count);

/**
 * @brief  渲染所有活跃伤害飘字
 *         Render all active floating damage texts.
 * @param  texts       飘字数组（const）/ Const float text array
 * @param  text_count  当前数量 / Current count
 */
void floattext_draw_all(const FloatingText* texts, int text_count);

#endif /* PROJECTILE_H */
