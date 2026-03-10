/**
 * @file    game.h
 * @brief   核心游戏模块：游戏状态机、主循环调度、输入处理、波次管理
 *          Core game module: game state machine, main loop dispatch, input handling, wave management.
 * @author  主工程师 / Lead Engineer
 * @date    2026-03-10
 * @version 1.0
 */

#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "../map/map.h"
#include "../enemy/enemy.h"
#include "../tower/tower.h"
#include "../projectile/projectile.h"

/* ============================================================
 * 常量定义 | Constants
 * ============================================================ */

/* 核心游戏数值（来自 GAME_DESIGN.md）
   Core game values (from GAME_DESIGN.md) */
#define INITIAL_GOLD            150
#define BASE_INITIAL_HP         10
#define TOTAL_WAVES             3
#define WAVE_DELAY_SECONDS      5.0f    /* 波次间等待时间（秒）/ Delay between waves (seconds) */
#define FIRST_WAVE_DELAY_SEC    3.0f    /* 第一波开始前等待（秒）/ Delay before first wave (seconds) */

/* 波次敌人配置（来自 GAME_DESIGN.md）
   Wave enemy configuration (from GAME_DESIGN.md) */
#define WAVE1_ENEMY_COUNT       5
#define WAVE1_SPAWN_INTERVAL    2.0f
#define WAVE2_ENEMY_COUNT       10
#define WAVE2_SPAWN_INTERVAL    1.5f
#define WAVE3_ENEMY_COUNT       15
#define WAVE3_SPAWN_INTERVAL    1.0f

/* ============================================================
 * 类型定义 | Type Definitions
 * ============================================================ */

/**
 * @brief 游戏状态枚举
 *        Game state enumeration.
 */
typedef enum {
    GAME_STATE_MENU,        /* 主菜单（暂未实现）/ Main menu (not yet implemented) */
    GAME_STATE_PLAYING,     /* 游戏进行中 / Game in progress */
    GAME_STATE_GAME_OVER,   /* 游戏失败 / Game over (player lost) */
    GAME_STATE_VICTORY      /* 游戏胜利 / Victory (player won) */
} GameState;

/**
 * @brief 游戏主状态结构体（包含所有游戏实体与状态变量）
 *        Main game state structure (contains all game entities and state variables).
 */
typedef struct {
    GameState current_state;    /* 当前游戏状态 / Current game state */
    Map       game_map;         /* 地图数据 / Map data */

    /* 实体数组 / Entity arrays */
    EnemyState       enemies[MAX_ENEMIES];
    int              enemy_count;
    Tower            towers[MAX_TOWERS];
    int              tower_count;
    VisualProjectile projectiles[MAX_PROJECTILES];
    int              projectile_count;
    FloatingText     float_texts[MAX_FLOAT_TEXTS];
    int              float_text_count;

    /* 游戏状态变量 / Game state variables */
    int   base_hp;              /* 当前基地血量 / Current base HP */
    int   money;                /* 当前金币数量 / Current gold amount */
    int   current_wave;         /* 当前波次编号（0 表示尚未开始）/ Current wave (0 = not started) */

    /* 波次控制 / Wave control */
    int   enemies_spawned;      /* 当前波次已生成敌人数 / Enemies spawned this wave */
    int   enemies_to_spawn;     /* 当前波次需生成总数 / Total enemies to spawn this wave */
    int   is_wave_active;       /* 1 = 波次进行中 / wave active；0 = 波次间隔 / between waves */
    float spawn_timer;          /* 生成间隔计时器（秒）/ Spawn interval timer (seconds) */
    float spawn_interval;       /* 当前波次生成间隔（秒）/ Current wave spawn interval (seconds) */
    float wave_delay_timer;     /* 下波开始倒计时（秒）/ Countdown to next wave start (seconds) */

    /* UI 辅助状态 / UI helper state */
    float warning_timer;        /* 金币不足警告倒计时（秒）/ Insufficient gold warning timer (seconds) */
    int   warning_x;            /* 警告显示位置 x / Warning display x */
    int   warning_y;            /* 警告显示位置 y / Warning display y */
} Game;

/* ============================================================
 * 公开接口 | Public API
 * ============================================================ */

/**
 * @brief  初始化游戏状态（新游戏开始时调用）
 *         Initialize game state (call when starting a new game).
 * @param  g  指向游戏状态结构体的指针，不得为 NULL / Pointer to Game struct, must not be NULL
 */
void game_init(Game* g);

/**
 * @brief  处理玩家输入（每帧调用）
 *         Process player input (call every frame).
 * @param  g  指向游戏状态结构体的指针 / Pointer to Game struct
 */
void game_process_input(Game* g);

/**
 * @brief  更新所有游戏逻辑（每帧调用）
 *         Update all game logic (call every frame).
 * @param  g   指向游戏状态结构体的指针 / Pointer to Game struct
 * @param  dt  本帧时间差（秒）/ Frame delta time in seconds
 */
void game_update(Game* g, float dt);

/**
 * @brief  渲染所有游戏画面（每帧调用，内部调用 BeginDrawing/EndDrawing）
 *         Render all game visuals (call every frame, calls BeginDrawing/EndDrawing internally).
 * @param  g  指向游戏状态结构体的指针 / Pointer to Game struct
 */
void game_render(Game* g);

/**
 * @brief  启动下一波次
 *         Start the next wave.
 * @param  g  指向游戏状态结构体的指针 / Pointer to Game struct
 */
void game_start_next_wave(Game* g);

#endif /* GAME_H */
