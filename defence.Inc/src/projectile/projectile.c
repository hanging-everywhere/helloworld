/**
 * @file    projectile.c
 * @brief   抛射物与飘字模块实现
 *          Projectile & floating text module implementation.
 * @version 2.0
 */

#include "projectile.h"
#include "raylib.h"
#include <math.h>

/* ============================================================
 * 抛射物 | Projectile
 * ============================================================ */

void projectile_spawn(VisualProjectile* projs, int* proj_count,
                      float sx, float sy, float tx, float ty) {
    projectile_spawn_full(projs, proj_count, sx, sy, tx, ty,
                          -1, 0, 1, 0, PROJ_DEFAULT_LIFE);
}

void projectile_spawn_full(VisualProjectile* projs, int* proj_count,
                           float sx, float sy, float tx, float ty,
                           int target_id, int damage, int is_hit, int is_crow,
                           float life) {
    if (*proj_count >= MAX_PROJECTILES) return;
    VisualProjectile* p = &projs[*proj_count];
    p->start_x   = sx;        p->start_y  = sy;
    p->target_x  = tx;        p->target_y = ty;
    p->life      = life;      p->max_life = life;
    p->target_id = target_id;
    p->damage    = damage;
    p->is_hit    = is_hit;
    p->is_crow   = is_crow;
    (*proj_count)++;
}

void projectile_update_all(VisualProjectile* projs, int* proj_count, float dt) {
    int i = 0;
    while (i < *proj_count) {
        projs[i].life -= dt;
        if (projs[i].life <= 0.0f) {
            /* Swap-remove / 交换删除 */
            projs[i] = projs[*proj_count - 1];
            (*proj_count)--;
        } else {
            i++;
        }
    }
}

void projectile_draw_all(const VisualProjectile* projs, int proj_count) {
    int i;
    for (i = 0; i < proj_count; i++) {
        float prog = 1.0f - (projs[i].life / projs[i].max_life);
        float cx = projs[i].start_x + (projs[i].target_x - projs[i].start_x) * prog;
        float cy = projs[i].start_y + (projs[i].target_y - projs[i].start_y) * prog;

        /* 抛物线弧度 / Parabolic arc */
        float arc = projs[i].is_crow ? PROJ_CROW_ARC : PROJ_ARC_HEIGHT;
        cy -= arc * sinf(prog * 3.1415926f);

        if (projs[i].is_crow) {
            /* 飞鸦弹：金色圆 / Firecrow: gold circle */
            DrawCircle((int)cx, (int)cy, 6, (Color){220, 180, 40, 255});
        } else {
            /* 普通石块：灰色圆 / Normal stone: gray circle */
            DrawCircle((int)cx, (int)cy, PROJ_RADIUS, (Color){180, 180, 180, 255});
        }
    }
}

/* ============================================================
 * 伤害飘字 | Floating Text
 * ============================================================ */

void floattext_spawn(FloatingText* texts, int* text_count,
                     float x, float y, int damage) {
    if (*text_count >= MAX_FLOAT_TEXTS) return;
    FloatingText* t = &texts[*text_count];
    t->x      = x;
    t->y      = y - 20.0f;
    t->damage = damage;
    t->life   = FLOAT_TEXT_LIFE;
    (*text_count)++;
}

void floattext_update_all(FloatingText* texts, int* text_count, float dt) {
    int i = 0;
    while (i < *text_count) {
        texts[i].life -= dt;
        texts[i].y    -= FLOAT_TEXT_SPEED * dt;
        if (texts[i].life <= 0.0f) {
            texts[i] = texts[*text_count - 1];
            (*text_count)--;
        } else {
            i++;
        }
    }
}

void floattext_draw_all(const FloatingText* texts, int text_count) {
    int i;
    const char* s;
    Color col;
    for (i = 0; i < text_count; i++) {
        /* 特殊伤害标记 / Special damage codes */
        switch (texts[i].damage) {
            case -1: s = "Miss!";  col = LIGHTGRAY;              break;
            case -2: s = "Stun!";  col = YELLOW;                 break;
            case -3: s = "ROAR!";  col = PURPLE;                 break;
            case -4: s = "VOID";   col = (Color){100,0,200,255}; break;
            default: s = TextFormat("-%d", texts[i].damage);
                     col = RED;
                     break;
        }
        /* 阴影 + 主色 / Shadow + main color */
        DrawText(s, (int)texts[i].x + 1, (int)texts[i].y + 1, FLOAT_TEXT_FONT_SZ, BLACK);
        DrawText(s, (int)texts[i].x,     (int)texts[i].y,     FLOAT_TEXT_FONT_SZ, col);
    }
}
