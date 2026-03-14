/**
 * @file    projectile.h
 * @brief   子弹/抛射物模块：视觉抛射物与伤害飘字的数据结构和管理接口
 *          Projectile module: visual projectiles and floating damage text data structures and management.
 * @author  副工程师 / Junior Engineer
 * @date    2026-03-10
 * @version 1.0
 *
 * @note  本模块仅处理视觉表现（抛射物动画、伤害飘字），伤害计算在 tower 模块中完成。
 *        This module handles visual effects only (projectile animation, damage numbers).
 *        Damage calculation is done in the tower module.
 */

#ifndef PROJECTILE_H
#define PROJECTILE_H

/* ============================================================
 * 常量定义 | Constants
 * ============================================================ */

#define MAX_PROJECTILES     300     /* 最大同屏抛射物数量 / Max projectiles on screen */
#define MAX_FLOAT_TEXTS     300     /* 最大同屏伤害飘字数量 / Max floating texts on screen */

#define PROJ_RADIUS         5       /* 抛射物圆形半径（像素）/ Projectile circle radius (pixels) */
#define PROJ_ARC_HEIGHT     80.0f   /* 抛物线弧高（像素）/ Parabola arc height (pixels) */
#define PROJ_DEFAULT_LIFE   0.2f    /* 抛射物飞行时间（秒）/ Projectile flight duration (seconds) */

#define FLOAT_TEXT_SPEED    30.0f   /* 飘字上升速度（像素/秒）/ Float text rise speed (pixels/second) */
#define FLOAT_TEXT_LIFE     0.8f    /* 飘字存续时间（秒）/ Float text duration (seconds) */
#define FLOAT_TEXT_FONT_SZ  20      /* 飘字字体大小 / Float text font size */

/* ============================================================
 * 类型定义 | Type Definitions
 * ============================================================ */

/**
 * @brief 视觉抛射物（石块飞行动画，从炮塔飞向目标位置）
 *        Visual projectile (stone in flight, from tower to target position).
 */
typedef struct {
    float start_x;   /* 发射起始 x（像素）/ Launch start x (pixels) */
    float start_y;   /* 发射起始 y（像素）/ Launch start y (pixels) */
    float target_x;  /* 目标落点 x（像素）/ Target landing x (pixels) */
    float target_y;  /* 目标落点 y（像素）/ Target landing y (pixels) */
    float life;      /* 剩余飞行时间（秒）/ Remaining flight time (seconds) */
    float max_life;  /* 最大飞行时间（秒，用于进度插值）
                        Max flight time (seconds, used for progress interpolation) */
} VisualProjectile;

/**
 * @brief 伤害飘字（击中时显示的伤害数字，向上飘动后消失）
 *        Floating damage text (damage number shown on hit, floats upward then disappears).
 */
typedef struct {
    float x;      /* 当前 x 位置（像素）/ Current x position (pixels) */
    float y;      /* 当前 y 位置（像素，向上减少）/ Current y position (pixels, decreases upward) */
    int   damage; /* 显示的伤害数值 / Damage value to display */
    float life;   /* 剩余显示时间（秒）/ Remaining display time (seconds) */
} FloatingText;

/* ============================================================
 * 公开接口 | Public API
 * ============================================================ */

/**
 * @brief  生成一个新的视觉抛射物，追加到数组中
 *         Spawn a new visual projectile and append it to the array.
 * @param  projs       抛射物数组 / Projectile array
 * @param  proj_count  当前数组长度指针（函数内部会递增）/ Pointer to current array length (incremented inside)
 * @param  sx          起始 x / Start x
 * @param  sy          起始 y / Start y
 * @param  tx          目标 x / Target x
 * @param  ty          目标 y / Target y
 */
void projectile_spawn(VisualProjectile* projs, int* proj_count,
                      float sx, float sy, float tx, float ty);

/**
 * @brief  生成一个新的伤害飘字，追加到数组中
 *         Spawn a new floating damage text and append it to the array.
 * @param  texts       飘字数组 / Float text array
 * @param  text_count  当前数组长度指针（函数内部会递增）/ Pointer to current array length (incremented inside)
 * @param  x           显示位置 x / Display position x
 * @param  y           显示位置 y（初始值）/ Display position y (initial value)
 * @param  damage      伤害数值 / Damage value
 */
void floattext_spawn(FloatingText* texts, int* text_count,
                     float x, float y, int damage);

/**
 * @brief  更新所有活跃抛射物（每帧调用，自动删除已到期的条目）
 *         Update all active projectiles (call every frame, auto-remove expired entries).
 * @param  projs       抛射物数组 / Projectile array
 * @param  proj_count  当前数组长度指针 / Pointer to current array length
 * @param  dt          本帧时间差（秒）/ Frame delta time in seconds
 */
void projectile_update_all(VisualProjectile* projs, int* proj_count, float dt);

/**
 * @brief  更新所有活跃飘字（每帧调用，自动删除已到期的条目）
 *         Update all active floating texts (call every frame, auto-remove expired entries).
 * @param  texts       飘字数组 / Float text array
 * @param  text_count  当前数组长度指针 / Pointer to current array length
 * @param  dt          本帧时间差（秒）/ Frame delta time in seconds
 */
void floattext_update_all(FloatingText* texts, int* text_count, float dt);

/**
 * @brief  渲染所有活跃抛射物
 *         Render all active visual projectiles.
 * @param  projs       抛射物数组 / Projectile array
 * @param  proj_count  当前数量 / Current count
 */
void projectile_draw_all(const VisualProjectile* projs, int proj_count);

/**
 * @brief  渲染所有活跃伤害飘字
 *         Render all active floating damage texts.
 * @param  texts       飘字数组 / Float text array
 * @param  text_count  当前数量 / Current count
 */
void floattext_draw_all(const FloatingText* texts, int text_count);

#endif /* PROJECTILE_H */
