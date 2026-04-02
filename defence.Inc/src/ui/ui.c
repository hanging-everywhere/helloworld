/**
 * @file    ui.c
 * @brief   UI 模块实现（渲染逻辑提取自 defense.Inc/GameManager.c）
 *          UI module implementation (rendering logic extracted from defense.Inc/GameManager.c).
 */

#include "ui.h"
#include <math.h>

void ui_draw_status_bar(int base_hp, int money, int current_wave, int total_waves) {
    /* 顶部黑色背景条 / Top dark background bar */
    DrawRectangle(0, 0, 1024, UI_BAR_HEIGHT, (Color){30, 30, 30, 255});
    DrawLine(0, UI_BAR_HEIGHT, 1024, UI_BAR_HEIGHT, (Color){100, 100, 100, 255});

    /* 状态文字（阴影 + 金色主字）/ Status text (shadow + gold main) */
    const char* info = TextFormat("Base HP: %d    Money: %d    Wave: %d/%d",
                                  base_hp, money, current_wave, total_waves);
    DrawText(info, 12, 12, UI_FONT_SIZE_SMALL, BLACK);
    DrawText(info, 10, 10, UI_FONT_SIZE_SMALL, GOLD);
}

void ui_draw_wave_countdown(float seconds_remaining) {
    const char* text = TextFormat("Next Wave in: %.1f s", seconds_remaining);
    DrawText(text, 350, 350, 40, WHITE);
}

void ui_draw_insufficient_gold_warning(int x, int y, float timer) {
    int float_y = y - 20 - (int)((1.0f - timer) * 30.0f);
    DrawText("Need 50 Money!", x - 40 + 2, float_y + 2, UI_FONT_SIZE_SMALL, BLACK);
    DrawText("Need 50 Money!", x - 40, float_y, UI_FONT_SIZE_SMALL, RED);
}

void ui_draw_tower_hover(Point center, int range, int can_build) {
    int half = 20;
    Color preview_color = can_build
                          ? (Color){80, 80, 80, 255}
                          : (Color){200, 50, 50, 150};
    DrawRectangle(center.x - half, center.y - half, half * 2, half * 2, preview_color);
    DrawCircleLines(center.x, center.y, (float)range, LIGHTGRAY);
}

void ui_draw_game_over(void) {
    DrawText("GAME OVER", 300, 300, 80, RED);
    DrawText("The fire went out...", 350, 400, UI_FONT_SIZE_MEDIUM, LIGHTGRAY);
}

void ui_draw_victory(void) {
    DrawText("LEVEL 1 CLEARED", 250, 300, UI_FONT_SIZE_LARGE, GREEN);
    DrawText("Humanity survived the night...", 320, 400, UI_FONT_SIZE_MEDIUM, LIGHTGRAY);
}

void ui_draw_base_bonfire(Point base_pos) {
    /* 呼吸动画 / Breathing animation */
    int pulse = (int)(sin(GetTime() * 8.0) * 6);
    DrawCircle(base_pos.x, base_pos.y, 35 + pulse,
               (Color){255, 80, 0, 255});
    DrawCircle(base_pos.x, base_pos.y, 25 + pulse / 2,
               (Color){255, 160, 0, 255});
    DrawCircle(base_pos.x, base_pos.y, 15,
               (Color){255, 230, 100, 255});
}
