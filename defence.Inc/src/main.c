/**
 * @file    main.c
 * @brief   程序入口：窗口初始化、主游戏循环
 *          Program entry point: window initialization and main game loop.
 * @author  主工程师 / Lead Engineer
 * @date    2026-03-10
 * @version 2.0  添加全屏支持、RenderTexture 离屏渲染、可调窗口大小
 */

#include "raylib.h"
#include "core/game.h"
#include "core/audio.h"

/* 逻辑分辨率（固定）/ Logical resolution (fixed) */
#define GAME_W   1024
#define LOGICAL_H 768
#define WINDOW_TITLE "千年城影 · 旧石器时代防线"
#define TARGET_FPS    60

/* 全局缩放/偏移参数（在 scale.h 中声明 extern）
   Global scale/offset params — declared extern in scale.h */
float g_scaleX  = 1.0f;
float g_scaleY  = 1.0f;
int   g_offsetX = 0;
int   g_offsetY = 0;

int main(void) {
    /* 切换到可执行文件所在目录（保证相对路径正确）
       Change to executable directory (ensures relative paths work) */
    ChangeDirectory(GetApplicationDirectory());

    /* 可调窗口大小 / Resizable window */
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(GAME_W, LOGICAL_H, WINDOW_TITLE);
    SetTargetFPS(TARGET_FPS);
    SetWindowMinSize(640, 480);

    /* 离屏渲染纹理（固定 1024×768 逻辑画布）
       Offscreen render texture (fixed 1024×768 logical canvas) */
    RenderTexture2D canvas = LoadRenderTexture(GAME_W, LOGICAL_H);
    SetTextureFilter(canvas.texture, TEXTURE_FILTER_BILINEAR);

    /* 初始化游戏状态 / Initialize game state */
    Game g;
    game_init(&g, 0); /* 0 = 启动主菜单 / start at main menu */

    /* 主游戏循环 / Main game loop */
    while (!WindowShouldClose() && !g.should_quit) {

        /* ── F11 全屏切换 / F11 fullscreen toggle ── */
        if (IsKeyPressed(KEY_F11)) {
            if (IsWindowFullscreen()) {
                ToggleFullscreen();
                SetWindowSize(GAME_W, LOGICAL_H);
            } else {
                SetWindowSize(GetMonitorWidth(GetCurrentMonitor()),
                              GetMonitorHeight(GetCurrentMonitor()));
                ToggleFullscreen();
            }
        }

        /* ── 每帧更新缩放/偏移（窗口大小可能变化）
               Update scale/offset each frame (window may be resized) ── */
        {
            int sw = GetScreenWidth();
            int sh = GetScreenHeight();
            float sx = (float)sw / GAME_W;
            float sy = (float)sh / LOGICAL_H;
            float scale = sx < sy ? sx : sy;
            int dw = (int)(GAME_W * scale);
            int dh = (int)(LOGICAL_H * scale);
            g_scaleX  = scale;
            g_scaleY  = scale;
            g_offsetX = (sw - dw) / 2;
            g_offsetY = (sh - dh) / 2;
        }

        float dt = GetFrameTime();

        audio_update(&g.audio);
        game_process_input(&g);
        game_update(&g, dt);

        /* ── 游戏渲染到离屏纹理 / Render game to offscreen texture ── */
        BeginTextureMode(canvas);
        game_render(&g);
        EndTextureMode();

        /* ── 离屏纹理等比缩放居中绘制到实际窗口
               Scale offscreen texture to actual window, letterboxed ── */
        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexturePro(
            canvas.texture,
            (Rectangle){ 0, 0, (float)GAME_W, -(float)LOGICAL_H },
            (Rectangle){ (float)g_offsetX, (float)g_offsetY,
                         (float)(GAME_W * g_scaleX), (float)(LOGICAL_H * g_scaleY) },
            (Vector2){ 0, 0 }, 0.0f, WHITE
        );
        EndDrawing();
    }

    /* 释放资源并关闭窗口 / Release resources and close window */
    UnloadRenderTexture(canvas);
    game_unload_all_textures(&g);
    CloseWindow();

    return 0;
}
