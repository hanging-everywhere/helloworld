/**
 * @file    menu.h
 * @brief   主菜单界面：独立模块，方便美术和策划修改
 *          Main menu screen: standalone module for easy art/design iteration.
 *
 * 按钮布局（动态，有/无存档时不同）/ Button layout (dynamic):
 *   无存档时（3个按钮）/ No save (3 buttons):
 *     0 = Start Game
 *     1 = Settings
 *     2 = Quit
 *
 *   有存档时（4个按钮）/ Save exists (4 buttons):
 *     0 = Start Game
 *     1 = Continue       ← 仅当 max_level_cleared > 0 时显示
 *     2 = Settings
 *     3 = Quit
 */

#ifndef MENU_H
#define MENU_H

/* 前向声明，避免循环 include */
struct Game;

/**
 * @brief  渲染主菜单界面（背景、标题、按钮列表）
 *         Render the main menu screen (background, title, button list).
 */
void menu_render(struct Game* g);

/**
 * @brief  处理主菜单的键盘和鼠标输入
 *         Handle keyboard and mouse input on the main menu.
 *
 * 逻辑：
 *   W/S 或上下键 → 移动光标（在可见按钮间循环）
 *   鼠标 hover   → 设置光标
 *   Enter / 左键 → 激活选中按钮
 * Logic:
 *   W/S or arrow keys → move cursor (wraps around visible buttons)
 *   Mouse hover       → set cursor
 *   Enter / left click → activate selected button
 */
void menu_process_input(struct Game* g);

#endif /* MENU_H */
