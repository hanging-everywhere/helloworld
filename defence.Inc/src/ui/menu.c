/**
 * @file    menu.c
 * @brief   主菜单界面实现
 *          Main menu screen implementation.
 */

#include "../core/game.h"
#include "../core/scale.h"
#include "menu.h"

/* ── 布局常量 / Layout constants ── */
#define M_CX      512   /* 屏幕中心 X / Screen center X */
#define M_BTN_W   300
#define M_BTN_H    60
#define M_BTN_GAP  80   /* 按钮行间距 / Row spacing */
#define M_BTN_Y0  330   /* 第一个按钮的 Y / First button Y */

/* ── 内部按钮描述 / Internal button descriptor ── */
typedef struct {
    const char* label;
    int         visible; /* 1=显示 0=隐藏 */
    int         y;       /* 渲染 Y 坐标（动态计算）*/
} MenuBtn;

/**
 * @brief  根据当前存档状态构建按钮表
 *         Build button table based on current save state.
 * @param  g     指向 Game / Pointer to Game
 * @param  btns  输出按钮数组（至少4项）/ Output array (at least 4 entries)
 * @return 可见按钮数量 / Number of visible buttons
 */
static int build_buttons(struct Game* g, MenuBtn btns[4]) {
    int has_save = (g->max_level_cleared > 0);
    int idx = 0, visible = 0, y = M_BTN_Y0;
    int i;

    btns[idx].label   = "Start Game";
    btns[idx].visible = 1;
    idx++;

    btns[idx].label   = "Continue";
    btns[idx].visible = has_save;
    idx++;

    btns[idx].label   = "Settings";
    btns[idx].visible = 1;
    idx++;

    btns[idx].label   = "Quit";
    btns[idx].visible = 1;
    idx++;

    /* 为可见按钮分配 Y 坐标 / Assign Y positions to visible buttons */
    for (i = 0; i < 4; i++) {
        if (btns[i].visible) {
            btns[i].y = y;
            y += M_BTN_GAP;
            visible++;
        } else {
            btns[i].y = -9999; /* 隐藏：屏幕外 / Hidden: off-screen */
        }
    }
    return visible;
}

/* ── 激活按钮逻辑 / Button activation logic ── */
static void activate_button(struct Game* g, int logical_index) {
    audio_play(&g->audio, "click");
    switch (logical_index) {
        case 0: /* Start Game */
            game_reset_to_level(g, 1);
            game_start_next_wave(g);
            break;
        case 1: /* Continue */
            if (g->max_level_cleared > 0) {
                int lvl = g->max_level_cleared + 1;
                if (lvl > 3) lvl = 3; /* 最多进入第3关 */
                game_reset_to_level(g, lvl);
                game_start_next_wave(g);
            }
            break;
        case 2: /* Settings */
            g->current_state = GAME_STATE_SETTINGS;
            break;
        case 3: /* Quit */
            g->should_quit = 1;
            break;
    }
}

/* ============================================================
 * menu_render
 * ============================================================ */
void menu_render(struct Game* g) {
    MenuBtn btns[4];
    int vis_count, vis_idx, i;
    vis_count = build_buttons(g, btns);
    vis_idx   = 0; /* 当前遍历到的可见序号 */
    (void)vis_count;

    /* 背景图（半透明）/ Background texture (semi-transparent) */
    if (g->bg_map_textures[0].id != 0) {
        Texture2D bg = g->bg_map_textures[0];
        DrawTexturePro(bg,
                       (Rectangle){0,0,(float)bg.width,(float)bg.height},
                       (Rectangle){0,0,1024,768},
                       (Vector2){0,0}, 0.0f, Fade(WHITE, 0.45f));
    }
    DrawRectangle(0, 0, 1024, 768, (Color){0, 0, 0, 155});

    /* 游戏标题 / Game title */
    DrawText("Defence.Inc",
             M_CX - MeasureText("Defence.Inc", 72)/2, 160, 72, BLACK);
    DrawText("Defence.Inc",
             M_CX - MeasureText("Defence.Inc", 72)/2 - 2, 158, 72, GOLD);

    DrawText("Millennium Shadow",
             M_CX - MeasureText("Millennium Shadow", 28)/2, 242, 28, BLACK);
    DrawText("Millennium Shadow",
             M_CX - MeasureText("Millennium Shadow", 28)/2 - 1, 241, 28, RAYWHITE);

    /* 按钮列表 / Button list */
    for (i = 0; i < 4; i++) {
        if (!btns[i].visible) continue;

        int highlighted = (g->menu_selected == vis_idx);
        Color bg     = (Color){30, 30, 30, 200};
        Color border = highlighted ? GOLD : DARKGRAY;
        Color text_c = highlighted ? GOLD : LIGHTGRAY;

        int bx = M_CX - M_BTN_W/2;
        int by = btns[i].y;

        DrawRectangle(bx, by, M_BTN_W, M_BTN_H, bg);
        DrawRectangleLines(bx, by, M_BTN_W, M_BTN_H, border);

        int fs = 26;
        int tw = MeasureText(btns[i].label, fs);
        DrawText(btns[i].label,
                 M_CX - tw/2,
                 by + (M_BTN_H - fs)/2,
                 fs, text_c);

        vis_idx++;
    }

    /* 操作提示 / Controls hint */
    DrawText("[W/S] or Mouse to select  ·  [Enter/Click] to confirm",
             M_CX - MeasureText("[W/S] or Mouse to select  ·  [Enter/Click] to confirm", 15)/2,
             690, 15, (Color){120, 120, 120, 255});
}

/* ============================================================
 * menu_process_input
 * ============================================================ */
void menu_process_input(struct Game* g) {
    MenuBtn btns[4];
    int vis_count, mx, my, vis_idx, i, clicked;
    vis_count = build_buttons(g, btns);
    mx = MOUSE_LX(); my = MOUSE_LY();
    vis_idx = 0;
    clicked = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

    /* ── 键盘光标移动（在可见按钮间循环）── */
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        g->menu_selected = (g->menu_selected - 1 + vis_count) % vis_count;
    }
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        g->menu_selected = (g->menu_selected + 1) % vis_count;
    }

    /* ── 鼠标悬浮设置光标 / Mouse hover sets cursor ── */
    for (vis_idx = 0, i = 0; i < 4; i++) {
        if (!btns[i].visible) continue;
        if (my >= btns[i].y && my <= btns[i].y + M_BTN_H &&
            mx >= M_CX - M_BTN_W/2 && mx <= M_CX + M_BTN_W/2) {
            g->menu_selected = vis_idx;
        }
        vis_idx++;
    }

    /* ── 确认激活 / Confirm activation ── */
    if (IsKeyPressed(KEY_ENTER) || clicked) {
        /* 判断点击是否落在某个可见按钮上 / Check if click landed on a button */
        for (vis_idx = 0, i = 0; i < 4; i++) {
            if (!btns[i].visible) continue;
            int in_btn = (my >= btns[i].y && my <= btns[i].y + M_BTN_H &&
                          mx >= M_CX - M_BTN_W/2 && mx <= M_CX + M_BTN_W/2);
            if (IsKeyPressed(KEY_ENTER) && g->menu_selected == vis_idx) {
                activate_button(g, i);
                return;
            }
            if (clicked && in_btn) {
                activate_button(g, i);
                return;
            }
            vis_idx++;
        }
    }
}
