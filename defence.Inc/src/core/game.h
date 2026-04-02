#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "audio.h"
#include "../map/map.h"
#include "../enemy/enemy.h"
#include "../tower/tower.h"
#include "../projectile/projectile.h"
#include "../base/base.h"
#include "../utils/save.h"
#include "../utils/dataload.h"
#include "../narrative/narrative.h"

/* ============================================================
* 常量
* ============================================================ */
#define MAX_TRAPS             100
#define MAX_COMIC_PAGES        20
#define MAX_WAVE_EVENTS         8
#define WAVE_DELAY_SEC         5.0f
#define FIRST_WAVE_DELAY_SEC   3.0f

/* ============================================================
* 枚举
* ============================================================ */
typedef enum { ERA_STONE=0, ERA_ANCIENT=1, ERA_FUTURE=2 } Era;

typedef enum {
	GAME_STATE_MENU,
	GAME_STATE_SETTINGS,
	GAME_STATE_PLAYING_L1,
	GAME_STATE_PLAYING_L2,
	GAME_STATE_PLAYING_L3,
	GAME_STATE_TRANSITION,
	GAME_STATE_COMIC,
	GAME_STATE_PAUSED,
	GAME_STATE_GAME_OVER,
	GAME_STATE_VICTORY,
	GAME_STATE_CHOICE      /* 未来第3波多结局选择 */
} GameState;

typedef enum {
	WAVE_EVENT_NONE = 0,
	WAVE_EVENT_LIGHTNING,         /* 石器W2：闪电摧毁随机塔 */
	WAVE_EVENT_FEAR_DEBUFF,       /* 石器W2：全塔攻速-20% */
	WAVE_EVENT_FREEZE_TOWERS,     /* 石器W3：全塔冻结10秒 */
	WAVE_EVENT_PEASANT_CONVERT,   /* 古代W1：收服农民（30s后） */
	WAVE_EVENT_RELIGION_PHASE1,   /* 古代W2：宗教加持 */
	WAVE_EVENT_CULT_SPAWN,        /* 古代W2：绿塔变邪教头目（30s后） */
	WAVE_EVENT_PLAGUE,            /* 古代W3：鼠疫 */
	WAVE_EVENT_REVOLUTION,        /* 古代W4：人民革命 */
	WAVE_EVENT_AI_BETRAY,         /* 未来W2：AI叛变（10s后） */
	WAVE_EVENT_SANDSTORM_BURY,    /* 未来W1：沙暴掩埋2塔 */
	WAVE_EVENT_PLAYER_CHOICE,     /* 未来W3：进入多结局选择 */
} WaveEventType;

typedef struct {
	WaveEventType type;
	float  trigger_time;  /* 波次开始后多少秒触发（0=立即）*/
	int    triggered;
	float  duration;      /* 持续时间（0=瞬间）*/
	float  timer;
	int    param1;
} WaveEvent;

typedef struct {
	float x, y, life;
	int   active;
} Trap;

/* ============================================================
* Game 主结构体
* ============================================================ */
typedef struct Game {
	GameState current_state;
	Era       current_era;
	int       total_waves_this_level;
	
	Map         game_map;
	Base        base;
	NarrSystem  narr;
	
	EnemyState       enemies[MAX_ENEMIES];
	int              enemy_count;
	Tower            towers[MAX_TOWERS];
	int              tower_count;
	VisualProjectile projectiles[MAX_PROJECTILES];
	int              projectile_count;
	FloatingText     float_texts[MAX_FLOAT_TEXTS];
	int              float_text_count;
	BurnZone         burn_zones[MAX_BURN_ZONES];
	int              burn_zone_count;
	Trap             traps[MAX_TRAPS];
	
	int   wood, stone, metal, material;
	float resource_timer;
	
	int   current_wave;
	int   enemies_spawned, enemies_to_spawn;
	int   is_wave_active;
	float spawn_timer, spawn_interval;
	float wave_delay_timer;
	float wave_elapsed;
	
	WaveEvent wave_events[MAX_WAVE_EVENTS];
	int       wave_event_count;
	
	/* 全局减益/增益状态 */
	int   plague_active;
	float plague_resource_drain;
	int   fear_active;
	int   revolution_active;
	int   peasant_bonus_active;   /* Ancient W1: resource rate x1.7 after peasant convert */
	int   peasant_intro_done;     /* Ancient W1: 1 = intro narr finished, spawn may begin */
	int   manual_tower_built;     /* Ancient: count of manually built faction towers (excl. MINE/BONESPEAR) */
	int   religion_intro_done;    /* Ancient W2: 1 = intro narr finished, spawn+colour may begin */
	int   cult_spawn_pending;     /* Ancient W2: 1 = all peasants dead, waiting for narr then cult spawns */
	int   cult_bosses_to_spawn;              /* Ancient W2: queued cult bosses waiting to be released */
	int   cult_boss_types[MAX_TOWERS];       /* tower type for each queued cult boss */
	
	int   current_selected_tower;
	int   selected_tower_index;
	int   base_selected;
	int   portal_pending_index;
	float warning_timer;
	int   warning_x, warning_y;
	
	int   choice_selected;    /* 未来W3结局：0=展示 1=隐藏 2=战斗 */
	
	Texture2D bg_map_textures[3];
	Texture2D base_textures[3];       /* 基地精灵表：[0]=石器 [1]=古代 [2]=未来 */
	Texture2D skeleton_texture;
	Texture2D enemy_textures[ENEMY_TYPE_COUNT];
	Texture2D tower_textures[14];
	
	Texture2D comic_pages[MAX_COMIC_PAGES];
	int       comic_page_count;
	int       comic_current_page;
	int       comic_next_state;
	int       comic_group4_loaded;
	int       comic_group5_loaded;
	
	int       menu_selected;
	int       should_quit;
	GameState state_before_pause;
	
	AudioSystem audio;
	float master_volume;
	int   sfx_enabled;
	
	int   max_level_cleared;
	
	TowerDef tower_defs[14];
	WaveDef  wave_defs[12];
	int      tower_def_count;
	int      wave_def_count;
	
	/* Lightning visual effect */
	int   lightning_active;
	float lightning_timer;
	float lightning_target_x;
	float lightning_target_y;
	
	/* Slime explosion visual effect */
	int   slime_explosion_active;
	float slime_explosion_timer;
	float slime_explosion_x;
	float slime_explosion_y;
	
	/* Pending level transition: wait for narrative to finish before advancing */
	int   pending_ancient_end;
	
	/* Future era W1-W3: phased spawn control */
	int   future_phase;         /* 0=idle 1=seawave 2/4/6=sandstorm 3/5/7=wait 8=radiation 9=wait-clear */
	float future_phase_timer;
	int   future_phase_spawned;
	
	/* Future W2: AI betrayal phase */
	/* 0=idle, 1=narr1 done->build AI towers, 2=narr2 done->betray, 3=battle */
	int   ai_betray_phase;
	float ai_laser_cd;        /* cooldown for AI laser attacking player towers */
} Game;

/* ============================================================
* 公开 API
* ============================================================ */
void      game_init               (Game* g, int start_level);
void      game_load_all_textures  (Game* g);
void      game_unload_all_textures(Game* g);
void      game_process_input      (Game* g);
void      game_update             (Game* g, float dt);
void      game_render             (Game* g);
void      game_start_next_wave    (Game* g);
void      game_upgrade_tower      (Game* g, int path);
void      game_reset_to_level     (Game* g, int level);
EnemyType game_get_spawn_type     (int level, int wave, int spawn_index);

#endif
