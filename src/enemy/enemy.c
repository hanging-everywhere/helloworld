/**
 * @file    enemy.c
 * @brief   敌人模块实现（迁移自 defense.Inc/Enemy.c，已适配新 API）
 *          Enemy module implementation (migrated from defense.Inc/Enemy.c, adapted to new API).
 */

#include "enemy.h"
#include "../utils/utils.h"

void enemy_init(EnemyState* e, Point start_pos) {
    e->x = (float)start_pos.x;
    e->y = (float)start_pos.y;
    e->speed = ENEMY_SPEED_PPS;
    e->max_hp = ENEMY_INITIAL_HP;
    e->hp = e->max_hp;
    e->target_waypoint_index = 1;   /* 从第二个路点开始追踪 / Start tracking from 2nd waypoint */
    e->active = 1;
    e->reached_base = 0;
    e->hit_flash_timer = 0.0f;
}

int enemy_update(EnemyState* e, float dt, const Point* waypoints, int waypoint_count) {
    if (!e->active) return 0;

    /* 受击闪白倒计时 / Hit flash countdown */
    if (e->hit_flash_timer > 0) {
        e->hit_flash_timer -= dt;
    }

    if (e->target_waypoint_index < waypoint_count) {
        float tx = (float)waypoints[e->target_waypoint_index].x;
        float ty = (float)waypoints[e->target_waypoint_index].y;
        float dist = utils_distance(e->x, e->y, tx, ty);

        if (dist < 2.0f) {
            e->target_waypoint_index++;
        } else {
            float move = e->speed * dt;
            if (move > dist) move = dist;
            float dx = tx - e->x;
            float dy = ty - e->y;
            e->x += (dx / dist) * move;
            e->y += (dy / dist) * move;
        }
    } else {
        /* 到达大本营 / Reached the base */
        e->active = 0;
        e->reached_base = 1;
        return 1;
    }
    return 0;
}

void enemy_draw(const EnemyState* e) {
    if (!e->active) return;

    /* 受击闪白：白色；否则：暗红色 / Hit flash: white; otherwise: dark red */
    Color body = (e->hit_flash_timer > 0)
                 ? WHITE
                 : (Color){150, 30, 30, 255};
    DrawCircle((int)e->x, (int)e->y, ENEMY_RADIUS, body);

    /* 血条 / HP bar */
    int bar_w = ENEMY_HP_BAR_WIDTH;
    int current_bar = (int)((float)e->hp / (float)e->max_hp * bar_w);
    int bx = (int)e->x - bar_w / 2;
    int by = (int)e->y - ENEMY_HP_BAR_OFFSET;
    DrawRectangle(bx, by, bar_w, 5, (Color){100, 0, 0, 255});
    DrawRectangle(bx, by, current_bar, 5, (Color){0, 200, 0, 255});
}

void enemy_take_damage(EnemyState* e, int damage) {
    e->hp -= damage;
    e->hit_flash_timer = ENEMY_HIT_FLASH_SEC;
}

int enemy_get_waypoint_index(const EnemyState* e) {
    return e->target_waypoint_index;
}
