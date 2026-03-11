/**
 * @file    main.c
 * @brief   程序入口：窗口初始化、主游戏循环
 *          Program entry point: window initialization and main game loop.
 * @author  主工程师 / Lead Engineer
 * @date    2026-03-10
 * @version 1.0
 */

#include "raylib.h"
#include "core/game.h"

/* 窗口配置常量 / Window configuration constants */
#define WINDOW_WIDTH   1024
#define WINDOW_HEIGHT  768
#define WINDOW_TITLE   "千年城影 · 旧石器时代防线"
#define TARGET_FPS     60

int main(void) {
    /* 初始化 Raylib 窗口
       Initialize Raylib window */
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
    SetTargetFPS(TARGET_FPS);

    /* 初始化游戏状态
       Initialize game state */
    Game g;
    game_init(&g);

    /* 主游戏循环：窗口未关闭时持续运行
       Main game loop: runs until window is closed */
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        game_process_input(&g);
        game_update(&g, dt);
        game_render(&g);
    }

    /* 释放资源并关闭窗口
       Release resources and close window */
    CloseWindow();

    return 0;
}
