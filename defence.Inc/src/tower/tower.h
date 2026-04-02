#ifndef TOWER_H
#define TOWER_H

#include "raylib.h"
#include "../map/map.h"
#include "../enemy/enemy.h"
#include "../projectile/projectile.h"

#define MAX_TOWERS 100

/* 阵营常量 */
#define FACTION_NORMAL  0
#define FACTION_PEOPLE  1
#define FACTION_NOBLE   2

/* 塔地面颜色状态 */
#define TOWER_COLOR_NORMAL  0
#define TOWER_COLOR_BLUE    1   /* religion buff: +15% fire rate, +10% damage */
#define TOWER_COLOR_GREEN   2   /* about to betray: -20% fire rate */
#define TOWER_COLOR_PURPLE  3   /* miasma: -30% fire rate */
#define TOWER_COLOR_RED     4   /* revolution: +50% fire rate */

typedef enum {
	TOWER_NONE=0,
	TOWER_SLING=1, TOWER_BONESPEAR=2, TOWER_TOTEM=3, TOWER_FIRE=4,
	TOWER_CROSSBOW=5, TOWER_MINE=6, TOWER_CAULDRON=7, TOWER_FIRECROW=8,
	TOWER_PULSE=9, TOWER_GRAVITY=10, TOWER_PORTAL=11,
	TOWER_TIMECLOCK=12, TOWER_LASER=13
} TowerType;

typedef struct {
	int   x, y;
	int   era, type;
	int   level_a, level_b;
	int   range, damage;
	float cooldown, current_cooldown;
	int   ammo, state;
	float timer1, timer2;
	int   target_id;
	int   paired_index;
	float buff_timer, buff_speed_ratio;
	Rectangle frame_rec;
	int   current_frame, frames_counter, frames_speed, max_frames;
	
	int   hit_count_on_target, last_target_id;
	float reload_timer;
	int   explosive_charges;
	float explosive_dmg_mult;
	float crow_cooldown, crow_current_cd;
	float charge_time;
	int   is_charging;
	int   gravity_explode_ready;
	int   portal_role, portal_tele_count;
	float portal_collapse_cd;
	float time_wave_timer;
	float laser_beam_timer;
	int   laser_charges;
	float laser_pulse_cd;
	
	/* 新增：阵营与状态字段 */
	int   faction;          /* FACTION_NORMAL / PEOPLE / NOBLE */
	int   color_state;      /* TOWER_COLOR_* */
	int   is_frozen;        /* 1=frozen, cannot attack */
	float frozen_timer;     /* remaining freeze duration */
	int   is_betrayed;      /* 1=AI betrayed, stops attacking, renders purple */
	int   is_buried;        /* 1=sandstorm buried, cannot attack, needs resources to clear */
	float bury_timer;       /* accumulated damage from betrayed towers (HP tracker) */
	int   is_bonfire;       /* 1=converted to bonfire by lightning (reveals invisible) */
	float damage_mult;      /* damage multiplier: 1.0=normal, 1.1=people/blue-religion buff */
	int   hp;               /* tower HP for Future W2 AI battle (0=not in HP mode) */
	int   is_ai_tower;      /* 1=built by AI, will betray and spawn as enemy */
} Tower;

void tower_init(Tower* t, int x, int y, int era, int type);

void tower_update(Tower* t, float dt,
				  EnemyState* enemies, int enemy_count,
				  Tower* all_towers, int tower_count,
				  VisualProjectile* projs, int* proj_count,
				  FloatingText* texts, int* text_count,
				  int* wood, int* stone, int* metal, int* material,
				  BurnZone* burn_zones, int* burn_zone_count);

void tower_draw           (const Tower* t, Texture2D* tower_textures);
void tower_draw_laser_beam(const Tower* t,
						   const EnemyState* enemies, int enemy_count);

#endif
