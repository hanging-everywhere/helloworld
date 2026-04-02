/**
 * @file    settings.c
 * @brief   设置界面实现
 *          Settings screen implementation.
 */

#include "../core/game.h"
#include "../core/audio.h"
#include "../core/scale.h"
#include "../utils/save.h"
#include "settings.h"
#include <stdio.h>

/* ── 布局常量 / Layout constants ── */
#define S_CX  512   /* 屏幕中心 X / Screen center X */
#define S_BTN_W  300
#define S_BTN_H   50

/* 各行 Y 坐标 / Row Y positions */
#define Y_TITLE   180
#define Y_VOL     320
#define Y_SFX     410
#define Y_BACK    520

/* ── 内部工具函数 / Internal helpers ── */

/** 绘制一个可高亮的按钮 / Draw a highlightable button */
static void draw_btn(int cx, int y, int w, int h,
                     const char* label, int highlighted, int font_size) {
    Color bg    = (Color){30, 30, 30, 210};
    Color border = highlighted ? GOLD : DARKGRAY;
    Color text_c = highlighted ? GOLD : LIGHTGRAY;

    DrawRectangle(cx - w/2, y, w, h, bg);
    DrawRectangleLines(cx - w/2, y, w, h, border);

    int tw = MeasureText(label, font_size);
    DrawText(label, cx - tw/2, y + (h - font_size)/2, font_size, text_c);
}

/** 判断鼠标是否在按钮矩形内（使用逻辑坐标）/ Check mouse inside button (logical coords) */
static int mouse_in(int cx, int y, int w, int h) {
    int mx = MOUSE_LX(), my = MOUSE_LY();
    return mx >= cx - w/2 && mx <= cx + w/2 &&
           my >= y         && my <= y + h;
}

/* ── 存档并返回菜单 / Save and return to menu ── */
static void back_to_menu(struct Game* g) {
    SaveData sd;
    sd.max_level_cleared = g->max_level_cleared;
    sd.master_volume     = g->master_volume;
    sd.sfx_enabled       = g->sfx_enabled;
    save_write(&sd);
    g->current_state = GAME_STATE_MENU;
    g->menu_selected = 0;
}

/* ============================================================
 * settings_render
 * ============================================================ */
void settings_render(struct Game* g) {
    char vol_label[64];

    /* 背景遮罩（复用菜单背景图）/ Dark overlay over bg */
    if (g->bg_map_textures[0].id != 0)
        DrawTexturePro(g->bg_map_textures[0],
                       (Rectangle){0,0,(float)g->bg_map_textures[0].width,
                                       (float)g->bg_map_textures[0].height},
                       (Rectangle){0,0,1024,768}, (Vector2){0,0}, 0.0f,
                       Fade(WHITE, 0.30f));
    DrawRectangle(0, 0, 1024, 768, (Color){0, 0, 0, 180});

    /* 标题 / Title */
    DrawText("SETTINGS", S_CX - MeasureText("SETTINGS", 52)/2, Y_TITLE, 52, GOLD);

    /* ── 音量行 / Volume row ── */
    DrawText("Master Volume", S_CX - MeasureText("Master Volume", 20)/2,
             Y_VOL - 32, 20, RAYWHITE);

    /* [−] 按钮 */
    draw_btn(S_CX - 100, Y_VOL, 60, S_BTN_H, "-", mouse_in(S_CX-100, Y_VOL, 60, S_BTN_H), 28);

    /* 音量数值 / Volume value */
    snprintf(vol_label, sizeof(vol_label), "%.0f%%", g->master_volume * 100.0f);
    DrawText(vol_label, S_CX - MeasureText(vol_label, 26)/2, Y_VOL + 12, 26, GOLD);

    /* [+] 按钮 */
    draw_btn(S_CX + 100, Y_VOL, 60, S_BTN_H, "+", mouse_in(S_CX+100, Y_VOL, 60, S_BTN_H), 28);

    /* ── SFX 开关 / SFX toggle ── */
    DrawText("Sound Effects", S_CX - MeasureText("Sound Effects", 20)/2,
             Y_SFX - 32, 20, RAYWHITE);
    {
        const char* sfx_label = g->sfx_enabled ? "ON" : "OFF";
        Color sfx_col = g->sfx_enabled ? GREEN : DARKGRAY;
        DrawRectangle(S_CX - S_BTN_W/2, Y_SFX, S_BTN_W, S_BTN_H, (Color){30,30,30,210});
        DrawRectangleLines(S_CX - S_BTN_W/2, Y_SFX, S_BTN_W, S_BTN_H, sfx_col);
        DrawText(sfx_label,
                 S_CX - MeasureText(sfx_label, 26)/2,
                 Y_SFX + (S_BTN_H - 26)/2, 26, sfx_col);
    }

    /* ── Back 按钮 / Back button ── */
    draw_btn(S_CX, Y_BACK, S_BTN_W, S_BTN_H, "< Back to Menu",
             mouse_in(S_CX, Y_BACK, S_BTN_W, S_BTN_H), 22);

    /* 操作提示 / Controls hint */
    DrawText("[←/→] or click  ·  [ESC] Back",
             S_CX - MeasureText("[←/→] or click  ·  [ESC] Back", 15)/2,
             690, 15, (Color){120, 120, 120, 255});
}

/* ============================================================
 * settings_process_input
 * ============================================================ */
void settings_process_input(struct Game* g) {
    int clicked = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

    /* ── 音量调节 / Volume adjustment ── */
    /* 键盘 / Keyboard */
    if (IsKeyPressed(KEY_LEFT)) {
        g->master_volume -= 0.1f;
        if (g->master_volume < 0.0f) g->master_volume = 0.0f;
        audio_set_volume(&g->audio, g->master_volume);
        audio_play(&g->audio, "click");
    }
    if (IsKeyPressed(KEY_RIGHT)) {
        g->master_volume += 0.1f;
        if (g->master_volume > 1.0f) g->master_volume = 1.0f;
        audio_set_volume(&g->audio, g->master_volume);
        audio_play(&g->audio, "click");
    }
    /* 鼠标点击 [−] / Mouse click minus */
    if (clicked && mouse_in(S_CX - 100, Y_VOL, 60, S_BTN_H)) {
        g->master_volume -= 0.1f;
        if (g->master_volume < 0.0f) g->master_volume = 0.0f;
        audio_set_volume(&g->audio, g->master_volume);
        audio_play(&g->audio, "click");
    }
    /* 鼠标点击 [+] / Mouse click plus */
    if (clicked && mouse_in(S_CX + 100, Y_VOL, 60, S_BTN_H)) {
        g->master_volume += 0.1f;
        if (g->master_volume > 1.0f) g->master_volume = 1.0f;
        audio_set_volume(&g->audio, g->master_volume);
        audio_play(&g->audio, "click");
    }

    /* ── SFX 开关 / SFX toggle ── */
    if (clicked && mouse_in(S_CX, Y_SFX, S_BTN_W, S_BTN_H)) {
        g->sfx_enabled ^= 1;
        audio_set_sfx_enabled(&g->audio, g->sfx_enabled);
        audio_play(&g->audio, "click");
    }

    /* ── Back / ESC ── */
    if (IsKeyPressed(KEY_ESCAPE) ||
        (clicked && mouse_in(S_CX, Y_BACK, S_BTN_W, S_BTN_H))) {
        audio_play(&g->audio, "click");
        back_to_menu(g);
    }
}
