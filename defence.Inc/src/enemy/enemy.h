#ifndef ENEMY_H
#define ENEMY_H

#include "raylib.h"
#include "../map/map.h"

#define MAX_ENEMIES          200
#define ENEMY_HP_BAR_WIDTH    30
#define SKELETON_FRAME_WIDTH  32
#define SKELETON_FRAME_HEIGHT 32
#define SKELETON_WALK_FRAMES   3

#define WOLF_STEALTH_DELAY       4.0f   /* 狼：无攻击N秒后隐身 */
#define SLIME_EXPLODE_TIME       6.0f   /* 脓包怪：受击后自爆倒计时 */
#define RADIATION_PULSE_INTERVAL 4.0f   /* 核辐射：污染脉冲间隔 */

typedef enum {
	ENEMY_SKELETON = 0,
	/* 石器时代 */
	ENEMY_WOLF,
	ENEMY_BOAR,
	ENEMY_SABERTOOTH,
	ENEMY_MAMMOTH,
	/* 古代文明 */
	ENEMY_PEASANT,
	ENEMY_CULT_BOSS,
	ENEMY_RAT,
	ENEMY_SLIME,
	ENEMY_NOBLE_SKELETON,
	ENEMY_WRAITH,
	/* 未来纪元 */
	ENEMY_SANDSTORM,
	ENEMY_SEAWAVE,
	ENEMY_RADIATION,
	ENEMY_TYPE_COUNT
} EnemyType;

typedef struct {
	int       id;
	EnemyType enemy_type;
	float     x, y;
	
	float     base_speed;
	float     current_speed;
	
	int       max_hp, hp;
	int       target_waypoint_index;
	int       active;
	int       reached_base;
	
	/* 动画 */
	float     hit_flash_timer;
	Rectangle frame_rec;
	int       current_frame, frames_counter, frames_speed;
	
	/* 隐身 */
	int       is_invisible;
	int       is_naturally_invisible;
	
	/* Debuff 管线 */
	float     stun_timer;
	float     slow_timer, slow_ratio;
	float     dot_timer, dot_tick_timer;
	int       dot_damage;
	
	/* 传送门 */
	int       portal_count;
	
	/* 时间塔 A2：位置快照 */
	float     saved_x, saved_y;
	int       saved_waypoint_index;
	float     pos_record_timer;
	
	/* 时间塔 B1：因果律 */
	int       cause_mark_active;
	float     cause_timer;
	int       cause_damage_accum;
	
	/* 狼：无攻击计时 */
	float     no_attack_timer;
	
	/* 脓包怪：自爆 */
	int       explode_armed;
	float     explode_timer;
	int       explosion_ready;  /* game_update 读取后处理爆炸效果 */
	
	/* 核辐射：污染脉冲 */
	float     radiation_pulse_cd;
	
	/* 邪教头目：来源塔类型（用于贴图索引）*/
	int       cult_tower_type;
	
} EnemyState;

/* ── 公开 API ── */
void enemy_init        (EnemyState* e, Point start_pos, int id, EnemyType type);
int  enemy_update      (EnemyState* e, float dt,
						const Point* waypoints, int waypoint_count);
void enemy_draw        (const EnemyState* e,
						Texture2D* enemy_textures,
						Texture2D* tower_textures);
void enemy_take_damage (EnemyState* e, int damage);
int  enemy_get_waypoint_index(const EnemyState* e);
void enemy_apply_stun  (EnemyState* e, float duration);
void enemy_apply_slow  (EnemyState* e, float duration, float ratio);
void enemy_apply_dot   (EnemyState* e, float duration, int dmg_tick);
void enemy_reveal      (EnemyState* e);

#endif
