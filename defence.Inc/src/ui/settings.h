/**
 * @file    settings.h
 * @brief   设置界面：音量调节、音效开关
 *          Settings screen: volume control and SFX toggle.
 *
 * 注意：此头文件仅使用前向声明 struct Game，不 include game.h，
 * 避免循环依赖。settings.c 中 include ../core/game.h 获取完整定义。
 * Note: This header uses only a forward declaration of struct Game to
 * avoid circular includes. settings.c includes ../core/game.h for full def.
 */

#ifndef SETTINGS_H
#define SETTINGS_H

/* 前向声明，避免循环 include / Forward declaration to avoid circular include */
struct Game;

/**
 * @brief  渲染设置界面（音量±、SFX开关、Back按钮）
 *         Render the settings screen (volume ±, SFX toggle, Back button).
 */
void settings_render(struct Game* g);

/**
 * @brief  处理设置界面的键盘和鼠标输入
 *         Handle keyboard and mouse input on the settings screen.
 *
 * 逻辑：
 *   [−] / [+] 按钮或左右键 → 调整 g->master_volume（步进 0.1）
 *   SFX 按钮 → 切换 g->sfx_enabled
 *   Back 按钮 / ESC → 写档并返回 GAME_STATE_MENU
 * Logic:
 *   [−]/[+] buttons or LEFT/RIGHT keys → adjust g->master_volume (step 0.1)
 *   SFX button → toggle g->sfx_enabled
 *   Back button / ESC → save and return to GAME_STATE_MENU
 */
void settings_process_input(struct Game* g);

#endif /* SETTINGS_H */
