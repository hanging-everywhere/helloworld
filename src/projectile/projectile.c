/**
 * @file    projectile.c
 * @brief   抛射物与飘字模块实现（逻辑提取自 defense.Inc/GameManager.c + Tower.c）
 *          Projectile & floating text implementation (extracted from defense.Inc/GameManager.c + Tower.c).
 */

#include "projectile.h"
#include "raylib.h"
#include <stdio.h>
#include <math.h>

/* ── 抛射物 / Projectile ── */

void projectile_spawn(VisualProjectile* projs, int* proj_count,
                      float sx, float sy, float tx, float ty) {
    if (*proj_count >= MAX_PROJECTILES) return;
    VisualProjectile* p = &projs[*proj_count];
    p->start_x  = sx;
    p->start_y  = sy;
    p->target_x = tx;
    p->target_y = ty;
    p->life     = PROJ_DEFAULT_LIFE;
    p->max_life = PROJ_DEFAULT_LIFE;
    (*proj_count)++;
}

void projectile_update_all(VisualProjectile* projs, int* proj_count, float dt) {
    int i = 0;
    while (i < *proj_count) {
        projs[i].life -= dt;
        if (projs[i].life <= 0) {
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
        cy -= PROJ_ARC_HEIGHT * sinf(prog * 3.1415926f);
        DrawCircle((int)cx, (int)cy, PROJ_RADIUS, (Color){180, 180, 180, 255});
    }
}

/* ── 伤害飘字 / Floating Text ── */

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
        texts[i].y -= FLOAT_TEXT_SPEED * dt;
        if (texts[i].life <= 0) {
            texts[i] = texts[*text_count - 1];
            (*text_count)--;
        } else {
            i++;
        }
    }
}

void floattext_draw_all(const FloatingText* texts, int text_count) {
    int i;
    for (i = 0; i < text_count; i++) {
        const char* s = TextFormat("-%d", texts[i].damage);
        DrawText(s, (int)texts[i].x + 1, (int)texts[i].y + 1,
                 FLOAT_TEXT_FONT_SZ, BLACK);
        DrawText(s, (int)texts[i].x, (int)texts[i].y,
                 FLOAT_TEXT_FONT_SZ, RED);
    }
}
