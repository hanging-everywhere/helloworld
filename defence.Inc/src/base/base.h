/**
 * @file    base.h
 * @brief   基地模块：基地实体初始化、攻击更新、绘制、升级接口
 *          Base module: base entity init, attack update, rendering, upgrade API.
 * @author  主工程师 / Lead Engineer
 * @date    2026-03-14
 * @version 1.0  迁移自 prototype/v2/Base.h，已转为 snake_case + float
 *               Migrated from prototype/v2/Base.h, converted to snake_case + float.
 */

#ifndef BASE_H
#define BASE_H

#include "raylib.h"
#include "../enemy/enemy.h"
#include "../projectile/projectile.h"

/* ============================================================
* 类型定义 | Type Definitions
* ============================================================ */

/**
 * @brief 基地完整状态结构体
 *        Complete state of the base entity.
 */
typedef struct {
	int   x, y;           /* 中心像素坐标 / Center pixel position */
	int   era;            /* 时代（0=石器 1=古代 2=未来）/ Era (0=stone 1=ancient 2=future) */
	int   max_hp;         /* 最大血量 / Max HP */
	int   hp;             /* 当前血量 / Current HP */
	
	int   level_a;        /* A路升级等级 / Level of upgrade path A */
	int   level_b;        /* B路升级等级（预留）/ Level of upgrade path B (reserved) */
	
	/* 石器A2 / 古代A1：自带攻击 / Built-in attack (Stone A2 / Ancient A1) */
	float attack_cooldown;
	float attack_timer;
	int   attack_damage;
	int   attack_range;
	
	/* 古代A1：免费建造次数 / Ancient A1: free build charges */
	int   free_build_count;
	
	/* 古代A2：陷阱盾 / Ancient A2: trap shield charges */
	int   trap_shield;
	
	/* 未来A1：最后一滴血无敌 / Future A1: last-stand invincibility */
	int   invincible;
	float invincible_timer;
	int   invincible_used;
	
	/* 未来A2：卫星轨道炮 / Future A2: satellite orbital cannon */
	float satellite_cooldown;
	float satellite_timer;
	
	float anim_timer;     /* 动画计时器 / Animation timer */
	int   anim_frame;     /* 当前动画帧（0-3）/ Current animation frame */
	float frame_timer;    /* 帧切换计时器 / Frame switch timer */
	
	Texture2D texture;    /* 基地精灵表（64×64，2×2布局）/ Base sprite sheet (64×64, 2×2 layout) */
} Base;

/* ============================================================
* 公开接口 | Public API
* ============================================================ */

/**
 * @brief  在指定位置初始化基地
 *         Initialize the base at the given position.
 * @param  b    指向 Base 的指针 / Pointer to Base struct
 * @param  x    中心像素横坐标 / Center pixel x
 * @param  y    中心像素纵坐标 / Center pixel y
 * @param  era  时代编号（0/1/2）/ Era index (0/1/2)
 */
void base_init(Base* b, int x, int y, int era);

/**
 * @brief  每帧更新基地逻辑（无敌计时、自动攻击、卫星炮）
 *         Update base logic per frame (invincibility, auto-attack, satellite).
 * @param  b            指向 Base 的指针 / Pointer to Base struct
 * @param  dt           帧时差（秒）/ Frame delta time (seconds)
 * @param  enemies      敌人数组 / Enemy array
 * @param  enemy_count  当前敌人数 / Current enemy count
 * @param  f_texts      飘字数组 / Float text array
 * @param  f_text_count 飘字计数指针 / Pointer to float text count
 * @param  projs        抛射物数组 / Projectile array
 * @param  proj_count   抛射物计数指针 / Pointer to projectile count
 */
void base_update(Base* b, float dt,
				 EnemyState* enemies, int enemy_count,
				 FloatingText* f_texts, int* f_text_count,
				 VisualProjectile* projs, int* proj_count);

/**
 * @brief  渲染基地（三个时代有不同外观）
 *         Render the base (different appearance per era).
 * @param  b  指向 Base 的指针（const）/ Const pointer to Base struct
 */
void base_draw(const Base* b);

/**
 * @brief  尝试升级基地
 *         Attempt to upgrade the base.
 * @param  b         指向 Base 的指针 / Pointer to Base struct
 * @param  path      升级路径（0=A路，1=B路）/ Upgrade path (0=A, 1=B)
 * @param  wood      木材指针 / Pointer to wood count
 * @param  stone     石材指针 / Pointer to stone count
 * @param  metal     金属指针 / Pointer to metal count
 * @param  material  未来材料指针 / Pointer to material count
 * @return 0=成功 / success；1=资源不足 / insufficient resources；2=不可用 / unavailable
 */
int base_upgrade(Base* b, int path,
				 int* wood, int* stone, int* metal, int* material);

/**
 * @brief  获取升级路径按钮标签与可用状态
 *         Get upgrade path button label and affordability.
 * @param  b              指向 Base（const）/ Const pointer to Base
 * @param  path           路径（0=A，1=B）/ Path (0=A, 1=B)
 * @param  out_label      输出标签字符串 / Output label string
 * @param  out_affordable 输出是否可负担（1/0）/ Output affordability flag
 * @param  wood,stone,metal,material  当前资源 / Current resources
 */
void base_get_upgrade_label(const Base* b, int path,
							const char** out_label, int* out_affordable,
							int wood, int stone, int metal, int material);

/**
 * @brief  消耗一次免费建造机会（古代A1）
 *         Consume one free build charge (Ancient A1).
 * @return 1=本次建造免费 / free；0=无可用机会 / no charges left
 */
int base_consume_free_build(Base* b);

/**
 * @brief  当敌人到达基地时，尝试陷阱盾拦截（古代A2）
 *         Try trap shield interception when enemy reaches base (Ancient A2).
 * @return 1=伤害被拦截 / blocked；0=无防护 / no shield
 */
int base_consume_trap_shield(Base* b);

/**
 * @brief  当基地血量极低时，尝试触发无敌（未来A1）
 *         Try to trigger last-stand invincibility when HP is critical (Future A1).
 * @return 1=无敌已激活 / activated；0=条件不满足 / not triggered
 */
int base_try_invincible(Base* b);

/**
 * @brief  设置基地精灵表贴图（在 game_load_all_textures 中调用）
 *         Set the base sprite sheet texture (called from game_load_all_textures).
 * @param  b    指向 Base 的指针 / Pointer to Base struct
 * @param  tex  已加载的贴图 / Loaded texture
 */
void base_set_texture(Base* b, Texture2D tex);

#endif /* BASE_H */
