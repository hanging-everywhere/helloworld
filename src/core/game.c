/**
 * @file    game.c
 * @brief   核心游戏模块实现（迁移自 defense.Inc/GameManager.c，已适配新模块化 API）
 *          Core game module implementation (migrated from defense.Inc/GameManager.c, adapted to new modular API).
 *
 * @note    原型中 GameManager 直接操作所有实体。新架构将渲染逻辑拆分到 ui 模块、
 *          将抛射物/飘字管理拆分到 projectile 模块。此文件只保留状态管理与逻辑调度。
 *          Prototype's GameManager handled everything directly. New architecture splits
 *          rendering to ui module and projectile/text management to projectile module.
 *          This file retains only state management and logic dispatch.
 */

#include "game.h"
#include "../ui/ui.h"

void game_init(Game* g) {
    map_init(&g->game_map);

    g->enemy_count       = 0;
    g->tower_count       = 0;
    g->projectile_count  = 0;
    g->float_text_count  = 0;

    g->base_hp       = BASE_INITIAL_HP;
    g->money         = INITIAL_GOLD;
    g->current_wave  = 0;

    g->is_wave_active    = 0;
    g->enemies_spawned   = 0;
    g->enemies_to_spawn  = 0;
    g->spawn_timer       = 0.0f;
    g->spawn_interval    = 0.0f;
    g->wave_delay_timer  = FIRST_WAVE_DELAY_SEC;

    g->warning_timer = 0.0f;
    g->warning_x     = 0;
    g->warning_y     = 0;

    g->current_state = GAME_STATE_PLAYING;
}

void game_start_next_wave(Game* g) {
    g->current_wave++;
    g->enemies_spawned = 0;
    g->is_wave_active = 1;

    switch (g->current_wave) {
        case 1:
            g->enemies_to_spawn = WAVE1_ENEMY_COUNT;
            g->spawn_interval   = WAVE1_SPAWN_INTERVAL;
            break;
        case 2:
            g->enemies_to_spawn = WAVE2_ENEMY_COUNT;
            g->spawn_interval   = WAVE2_SPAWN_INTERVAL;
            break;
        case 3:
            g->enemies_to_spawn = WAVE3_ENEMY_COUNT;
            g->spawn_interval   = WAVE3_SPAWN_INTERVAL;
            break;
        default:
            g->is_wave_active = 0;
            g->current_state = GAME_STATE_VICTORY;
            return;
    }
    g->spawn_timer = g->spawn_interval;
}

void game_process_input(Game* g) {
    if (g->current_state != GAME_STATE_PLAYING) return;

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        int mx  = GetMouseX();
        int my  = GetMouseY();
        int col = mx / CELL_SIZE;
        int row = my / CELL_SIZE;

        if (map_can_place_tower(&g->game_map, row, col)) {
            if (g->money >= TOWER_BUILD_COST && g->tower_count < MAX_TOWERS) {
                g->money -= TOWER_BUILD_COST;
                map_place_tower(&g->game_map, row, col);

                Point center = map_get_center(row, col);
                tower_init(&g->towers[g->tower_count], center);
                g->tower_count++;
            } else if (g->money < TOWER_BUILD_COST) {
                g->warning_timer = 1.0f;
                g->warning_x = mx;
                g->warning_y = my;
            }
        }
    }
}

void game_update(Game* g, float dt) {
    int i;

    if (g->current_state == GAME_STATE_GAME_OVER ||
        g->current_state == GAME_STATE_VICTORY) {
        return;
    }

    /* 基地血量检查 / Base HP check */
    if (g->base_hp <= 0) {
        g->current_state = GAME_STATE_GAME_OVER;
        return;
    }

    /* 金币不足警告倒计时 / Insufficient gold warning */
    if (g->warning_timer > 0) g->warning_timer -= dt;

    /* ── 波次生成逻辑 / Wave spawn logic ── */
    if (g->is_wave_active) {
        if (g->enemies_spawned < g->enemies_to_spawn) {
            g->spawn_timer -= dt;
            if (g->spawn_timer <= 0 && g->enemy_count < MAX_ENEMIES) {
                enemy_init(&g->enemies[g->enemy_count],
                           g->game_map.waypoints[0]);
                g->enemy_count++;
                g->enemies_spawned++;
                g->spawn_timer = g->spawn_interval;
            }
        } else if (g->enemy_count == 0) {
            g->is_wave_active = 0;
            g->wave_delay_timer = WAVE_DELAY_SECONDS;
        }
    } else {
        g->wave_delay_timer -= dt;
        if (g->wave_delay_timer <= 0) {
            game_start_next_wave(g);
        }
    }

    /* ── 炮塔更新 / Tower updates ── */
    for (i = 0; i < g->tower_count; i++) {
        tower_update(&g->towers[i], dt,
                     g->enemies, g->enemy_count,
                     g->projectiles, &g->projectile_count,
                     g->float_texts, &g->float_text_count);
    }

    /* ── 敌人更新（死亡/到达时交换删除）/ Enemy updates (swap-remove) ── */
    i = 0;
    while (i < g->enemy_count) {
        const Point* wps = map_get_waypoints(&g->game_map);
        int wp_count = map_get_waypoint_count(&g->game_map);

        enemy_update(&g->enemies[i], dt, wps, wp_count);

        if (g->enemies[i].reached_base || g->enemies[i].hp <= 0) {
            if (g->enemies[i].reached_base) {
                g->base_hp--;
            } else {
                g->money += ENEMY_KILL_REWARD;
            }
            g->enemies[i] = g->enemies[g->enemy_count - 1];
            g->enemy_count--;
        } else {
            i++;
        }
    }

    /* ── 抛射物与飘字更新 / Projectile & float text updates ── */
    projectile_update_all(g->projectiles, &g->projectile_count, dt);
    floattext_update_all(g->float_texts, &g->float_text_count, dt);
}

void game_render(Game* g) {
    int i;

    BeginDrawing();
    ClearBackground(BLACK);

    /* 1. 地图 / Map */
    map_draw(&g->game_map);

    /* 2. 建塔悬浮预览 / Tower placement hover preview */
    if (g->current_state == GAME_STATE_PLAYING) {
        int mx = GetMouseX();
        int my = GetMouseY();
        int col = mx / CELL_SIZE;
        int row = my / CELL_SIZE;

        if (row >= 0 && row < MAP_ROWS && col >= 0 && col < MAP_COLS) {
            int can = map_can_place_tower(&g->game_map, row, col);
            Point c = map_get_center(row, col);
            ui_draw_tower_hover(c, TOWER_RANGE, can);
        }
    }

    /* 3. 实体渲染 / Entity rendering */
    for (i = 0; i < g->tower_count; i++)  tower_draw(&g->towers[i]);
    for (i = 0; i < g->enemy_count; i++)  enemy_draw(&g->enemies[i]);

    /* 4. 大本营篝火 / Base bonfire */
    Point base = g->game_map.waypoints[g->game_map.waypoint_count - 1];
    ui_draw_base_bonfire(base);

    /* 5. 抛射物 / Projectiles */
    projectile_draw_all(g->projectiles, g->projectile_count);

    /* 6. UI 状态栏 / Status bar */
    ui_draw_status_bar(g->base_hp, g->money, g->current_wave, TOTAL_WAVES);

    /* 7. 伤害飘字 / Damage float texts */
    floattext_draw_all(g->float_texts, g->float_text_count);

    /* 8. 金币不足警告 / Insufficient gold warning */
    if (g->warning_timer > 0) {
        ui_draw_insufficient_gold_warning(g->warning_x, g->warning_y,
                                          g->warning_timer);
    }

    /* 9. 波次间倒计时 / Between-wave countdown */
    if (!g->is_wave_active &&
        g->current_state == GAME_STATE_PLAYING &&
        g->current_wave < TOTAL_WAVES) {
        ui_draw_wave_countdown(g->wave_delay_timer);
    }

    /* 10. 游戏结果画面 / Game result screen */
    if (g->current_state == GAME_STATE_GAME_OVER) ui_draw_game_over();
    if (g->current_state == GAME_STATE_VICTORY)   ui_draw_victory();

    EndDrawing();
}
