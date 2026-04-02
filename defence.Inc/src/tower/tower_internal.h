#ifndef TOWER_INTERNAL_H
#define TOWER_INTERNAL_H

#include "tower.h"

/* 内部更新上下文（减少参数传递）*/
typedef struct {
	EnemyState*       enemies;
	int               enemy_count;
	Tower*            all_towers;
	int               tower_count;
	VisualProjectile* projs;
	int*              proj_count;
	FloatingText*     texts;
	int*              text_count;
	int*              wood;
	int*              stone;
	int*              metal;
	int*              material;
	BurnZone*         burn_zones;
	int*              burn_zone_count;
} TowerCtx;

void tower_add_float_text(FloatingText* texts, int* count,
						  float x, float y, int dmg);
void tower_damage_enemy  (EnemyState* e, int dmg,
						  FloatingText* texts, int* count);

/* 石器时代 */
void update_sling    (Tower* t, float dt, TowerCtx* ctx);
void update_bonespear(Tower* t, float dt, TowerCtx* ctx);
void update_totem    (Tower* t, float dt, TowerCtx* ctx);
void update_fire     (Tower* t, float dt, TowerCtx* ctx);

/* 古代文明 */
void update_crossbow (Tower* t, float dt, TowerCtx* ctx);
void update_mine     (Tower* t, float dt, TowerCtx* ctx);
void update_cauldron (Tower* t, float dt, TowerCtx* ctx);
void update_firecrow (Tower* t, float dt, TowerCtx* ctx);

/* 科幻未来 */
void update_pulse    (Tower* t, float dt, TowerCtx* ctx);
void update_gravity  (Tower* t, float dt, TowerCtx* ctx);
void update_portal   (Tower* t, float dt, TowerCtx* ctx);
void update_timeclock(Tower* t, float dt, TowerCtx* ctx);
void update_laser    (Tower* t, float dt, TowerCtx* ctx);

#endif
