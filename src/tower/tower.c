/**
 * @file    tower.c
 * @brief   炮塔模块实现（迁移自 defense.Inc/Tower.c，已适配新 API）
 *          Tower module implementation (migrated from defense.Inc/Tower.c, adapted to new API).
 *
 * @note    原型中 Tower_update 直接访问 Enemy 内部字段（hp、hitFlashTimer、targetWaypointIndex）。
 *          新 API 通过 enemy_take_damage() 和 enemy_get_waypoint_index() 间接访问，已修正。
 *          Prototype directly accessed Enemy internals. New API uses accessor functions — fixed.
 */

#include "tower.h"
#include "../utils/utils.h"

void tower_init(Tower* t, Point center) {
    t->x = center.x;
    t->y = center.y;
    t->range = TOWER_RANGE;
    t->damage = TOWER_DAMAGE;
    t->cooldown = TOWER_COOLDOWN_SEC;
    t->current_cooldown = 0.0f;
}

void tower_update(Tower* t, float dt,
                  EnemyState* enemies, int enemy_count,
                  VisualProjectile* projs, int* proj_count,
                  FloatingText* texts, int* text_count) {

    if (t->current_cooldown > 0) {
        t->current_cooldown -= dt;
    }

    if (t->current_cooldown > 0) return;

    /* 选择射程内进度最远的敌人 / Select the most advanced enemy in range */
    int best_idx = -1;
    float max_progress = -1.0f;
    int i;

    for (i = 0; i < enemy_count; i++) {
        if (!enemies[i].active) continue;

        float dist = utils_distance((float)t->x, (float)t->y,
                                    enemies[i].x, enemies[i].y);

        if (dist <= (float)t->range) {
            int wp = enemy_get_waypoint_index(&enemies[i]);
            float score = (float)(wp * 10000) - dist;
            if (score > max_progress) {
                max_progress = score;
                best_idx = i;
            }
        }
    }

    if (best_idx != -1) {
        /* 造成伤害（通过 API，不直接访问 hp）/ Deal damage via API */
        enemy_take_damage(&enemies[best_idx], t->damage);
        t->current_cooldown = t->cooldown;

        /* 生成抛射物 / Spawn projectile */
        projectile_spawn(projs, proj_count,
                         (float)t->x, (float)t->y,
                         enemies[best_idx].x, enemies[best_idx].y);

        /* 生成伤害飘字 / Spawn floating damage text */
        floattext_spawn(texts, text_count,
                        enemies[best_idx].x, enemies[best_idx].y,
                        t->damage);
    }
}

void tower_draw(const Tower* t) {
    int half = TOWER_SIZE / 2;
    /* 石堆色方块 / Stone pile square */
    DrawRectangle(t->x - half, t->y - half, TOWER_SIZE, TOWER_SIZE,
                  (Color){100, 100, 100, 255});
    /* 深色射击孔 / Dark cannon opening */
    DrawCircle(t->x, t->y, 8, (Color){40, 40, 40, 255});
}

void tower_draw_range_preview(const Tower* t) {
    DrawCircleLines(t->x, t->y, (float)t->range, LIGHTGRAY);
}
