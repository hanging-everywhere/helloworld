/**
 * @file    ui.h
 * @brief   UI 模块：状态栏、波次提示、游戏结果画面、悬浮预览的渲染接口
 *          UI module: status bar, wave alerts, game result screen, hover preview rendering.
 * @author  主工程师 / Lead Engineer
 * @date    2026-03-10
 * @version 1.0
 *
 * @note  本模块仅负责渲染 UI 元素，不持有游戏状态数据（全部通过参数传入）。
 *        This module only renders UI elements; it does NOT own game state data (all passed as parameters).
 */

#ifndef UI_H
#define UI_H

#include "raylib.h"
#include "../map/map.h"

/* ============================================================
 * 常量定义 | Constants
 * ============================================================ */

#define UI_BAR_HEIGHT       40      /* 顶部状态栏高度（像素）/ Top status bar height (pixels) */
#define UI_FONT_SIZE_LARGE  60      /* 大字体（游戏结果标题）/ Large font size (game result title) */
#define UI_FONT_SIZE_MEDIUM 30      /* 中字体（副标题）/ Medium font size (subtitle) */
#define UI_FONT_SIZE_SMALL  20      /* 小字体（状态栏/提示）/ Small font size (status bar / hints) */

/* ============================================================
 * 公开接口 | Public API
 * ============================================================ */

/**
 * @brief  渲染顶部状态栏（基地血量、金币、波次）
 *         Render top status bar (base HP, gold, wave number).
 * @param  base_hp       当前基地血量 / Current base HP
 * @param  money         当前金币数量 / Current gold amount
 * @param  current_wave  当前波次编号 / Current wave number
 * @param  total_waves   总波次数量 / Total wave count
 */
void ui_draw_status_bar(int base_hp, int money, int current_wave, int total_waves);

/**
 * @brief  渲染下一波次倒计时提示文字
 *         Render next wave countdown text.
 * @param  seconds_remaining  剩余等待秒数 / Remaining seconds before next wave
 */
void ui_draw_wave_countdown(float seconds_remaining);

/**
 * @brief  渲染金币不足浮动警告
 *         Render floating "insufficient gold" warning.
 * @param  x              警告显示位置 x / Warning display x
 * @param  y              警告显示位置 y / Warning display y
 * @param  timer          警告剩余显示时间（秒，用于透明度/位置计算）
 *                        Remaining display time (seconds, for alpha/position calc)
 */
void ui_draw_insufficient_gold_warning(int x, int y, float timer);

/**
 * @brief  渲染建塔悬停预览（半透明方块 + 射程圈）
 *         Render tower placement hover preview (semi-transparent square + range circle).
 * @param  center    目标格子的像素中心坐标 / Pixel center of target cell
 * @param  range     射程半径（像素）/ Range radius (pixels)
 * @param  can_build 1 = 可建造（显示正常颜色）；0 = 不可建造（显示红色警示）
 *                   1 = buildable (normal color); 0 = not buildable (red warning)
 */
void ui_draw_tower_hover(Point center, int range, int can_build);

/**
 * @brief  渲染游戏失败画面
 *         Render game over screen.
 */
void ui_draw_game_over(void);

/**
 * @brief  渲染游戏胜利画面
 *         Render victory screen.
 */
void ui_draw_victory(void);

/**
 * @brief  渲染大本营篝火特效（呼吸动画）
 *         Render base bonfire visual effect (breathing animation).
 * @param  base_pos  大本营像素坐标 / Base pixel position
 */
void ui_draw_base_bonfire(Point base_pos);

#endif /* UI_H */
