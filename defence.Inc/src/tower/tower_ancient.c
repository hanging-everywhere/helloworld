/**
 * @file    tower_ancient.c
 * @brief   古代文明炮塔更新逻辑：诸葛连弩、震天雷、炼丹炉、飞鸦炮
 *          Ancient era tower update logic: Crossbow, Mine, Cauldron, Firecrow.
 * @version 2.0  迁移自 prototype/v2/Tower.c
 */

#include "tower_internal.h"
#include <math.h>

/* ============================================================
* update_crossbow — 诸葛连弩
* ============================================================ */
void update_crossbow(Tower* t, float dt, TowerCtx* ctx) {
	int i, j;
	float dx, dy;
	int cost_wood, cost_metal;
	
	/* 上弦阶段 */
	if (t->state == 0) {
		t->timer1 -= dt;
		if (t->timer1 <= 0.0f) { t->state = 1; t->timer1 = 0.0f; }
		return;
	}
	if (t->current_cooldown > 0.0f) return;
	
	/* 检查资源 */
	cost_wood  = 6;
	cost_metal = 10;
	if (t->level_b >= 1) { cost_wood *= 3; cost_metal *= 3; }
	if (*ctx->wood < cost_wood || *ctx->metal < cost_metal) return;
	
	/* 找第一个目标 */
	int best = -1;
	for (i = 0; i < ctx->enemy_count; i++) {
		if (!ctx->enemies[i].active || ctx->enemies[i].hp <= 0) continue;
		if (ctx->enemies[i].is_invisible) continue;
		dx = ctx->enemies[i].x - (float)t->x;
		dy = ctx->enemies[i].y - (float)t->y;
		if (dx*dx + dy*dy <= (float)t->range * t->range) { best = i; break; }
	}
	if (best < 0) return;
	
	*ctx->wood  -= cost_wood;
	*ctx->metal -= cost_metal;
	
	if (t->level_a >= 1) {
		/* A1：穿透，打射程内所有敌人 */
		for (i = 0; i < ctx->enemy_count; i++) {
			if (!ctx->enemies[i].active || ctx->enemies[i].hp <= 0) continue;
			if (ctx->enemies[i].is_invisible) continue;
			dx = ctx->enemies[i].x - (float)t->x;
			dy = ctx->enemies[i].y - (float)t->y;
			if (dx*dx + dy*dy > (float)t->range * t->range) continue;
			tower_damage_enemy(&ctx->enemies[i], (int)(t->damage * t->damage_mult), ctx->texts, ctx->text_count);
			if (t->level_a >= 2)
				enemy_apply_dot(&ctx->enemies[i], 3.0f, 10);
		}
	} else if (t->level_b >= 1) {
		/* B1：扇形散射3个最近目标 */
		int shots = 0;
		for (i = 0; i < ctx->enemy_count && shots < 3; i++) {
			if (!ctx->enemies[i].active || ctx->enemies[i].hp <= 0) continue;
			if (ctx->enemies[i].is_invisible) continue;
			dx = ctx->enemies[i].x - (float)t->x;
			dy = ctx->enemies[i].y - (float)t->y;
			if (dx*dx + dy*dy > (float)t->range * t->range) continue;
			if (t->level_b >= 2) {
				/* B2：连发5次，每次-5% */
				for (j = 0; j < 5; j++) {
					int d = (int)(t->damage * (1.0f - 0.05f * j));
					tower_damage_enemy(&ctx->enemies[i], d, ctx->texts, ctx->text_count);
				}
			} else {
				tower_damage_enemy(&ctx->enemies[i], (int)(t->damage * t->damage_mult), ctx->texts, ctx->text_count);
			}
			shots++;
		}
	} else {
		/* 基础单发 */
		tower_damage_enemy(&ctx->enemies[best], (int)(t->damage * t->damage_mult), ctx->texts, ctx->text_count);
	}
	
	/* 视觉子弹（伤害已直接结算，targetId=-1） */
	if (*ctx->proj_count < MAX_PROJECTILES) {
		projectile_spawn_full(ctx->projs, ctx->proj_count,
							  (float)t->x, (float)t->y,
							  ctx->enemies[best].x, ctx->enemies[best].y,
							  -1, 0, 1, 0, 0.15f);
	}
	
	t->current_cooldown = t->cooldown;
	t->state  = 0;
	t->timer1 = 3.0f; /* re-cock crossbow */
}

/* ============================================================
* update_mine — 震天雷
* ============================================================ */
/* 内部辅助：执行单次地雷爆炸伤害，正确处理隐身破除与狼速重置
同时将 timer1 设为爆炸光效持续时间（绘制层读取）：
主爆   → 0.45s（较长，让玩家看清范围）
A2二爆 → 0.30s（稍短，区别于主爆）
调用方传入 fx_duration 控制时长；若新值更大则覆盖（多重爆炸取最长）*/
static void mine_explode(Tower* t, TowerCtx* ctx, float blast_r, float fx_duration) {
	int j;
	for (j = 0; j < ctx->enemy_count; j++) {
		if (!ctx->enemies[j].active || ctx->enemies[j].hp <= 0) continue;
		float bx = ctx->enemies[j].x - (float)t->x;
		float by = ctx->enemies[j].y - (float)t->y;
		if (bx*bx + by*by > blast_r * blast_r) continue;
		
		tower_damage_enemy(&ctx->enemies[j], (int)(t->damage * t->damage_mult),
						   ctx->texts, ctx->text_count);
		if (ctx->enemies[j].is_invisible) {
			ctx->enemies[j].is_invisible = 0;
			if (ctx->enemies[j].enemy_type == ENEMY_WOLF)
				ctx->enemies[j].current_speed = ctx->enemies[j].base_speed;
		}
	}
	/* 爆炸光效计时：timer1 存储剩余显示时间，取较大值防止被更短的覆盖 */
	if (fx_duration > t->timer1)
		t->timer1 = fx_duration;
}

void update_mine(Tower* t, float dt, TowerCtx* ctx) {
	int i, j;
	float dx, dy;
	
	/* 每帧递减爆炸光效计时（timer1），与逻辑无关，纯视觉 */
	if (t->timer1 > 0.0f) {
		t->timer1 -= dt;
		if (t->timer1 < 0.0f) t->timer1 = 0.0f;
	}
	
	if (t->ammo <= 0) {
		/* B1：自动填充 */
		if (t->level_b >= 1) {
			t->reload_timer += dt;
			if (t->reload_timer >= 10.0f) {
				if (*ctx->metal >= 50) {
					*ctx->metal -= 50;
					t->ammo = 1;
					t->reload_timer = 0.0f;
					t->state = 0;
				}
			}
		}
		/* A2 二次爆炸倒计时（ammo==0 时仍需处理）*/
		if (t->timer2 > 0.0f) {
			t->timer2 -= dt;
			if (t->timer2 <= 0.0f) {
				float blast_r = (t->level_a >= 1) ? t->range * 1.7f : (float)t->range;
				mine_explode(t, ctx, blast_r, 0.30f);
			}
		}
		return;
	}
	
	/* A2 二次爆炸倒计时（ammo>0 时也可能在计时，例如连环引爆链中）*/
	if (t->timer2 > 0.0f) {
		t->timer2 -= dt;
		if (t->timer2 <= 0.0f) {
			float blast_r = (t->level_a >= 1) ? t->range * 1.7f : (float)t->range;
			mine_explode(t, ctx, blast_r, 0.30f);
		}
	}
	
	/* 检测敌人触发（隐身敌人同样可以踩雷触发） */
	for (i = 0; i < ctx->enemy_count; i++) {
		if (!ctx->enemies[i].active || ctx->enemies[i].hp <= 0) continue;
		dx = ctx->enemies[i].x - (float)t->x;
		dy = ctx->enemies[i].y - (float)t->y;
		if (dx*dx + dy*dy > (float)t->range * t->range) continue;
		
		/* 主爆炸，光效 0.45s */
		float blast_r = (t->level_a >= 1) ? t->range * 1.7f : (float)t->range;
		mine_explode(t, ctx, blast_r, 0.45f);
		
		/* A2：0.3 秒后二次爆炸 */
		if (t->level_a >= 2) t->timer2 = 0.3f;
		
		/* B2：连环爆，直接引爆邻近地雷 */
		if (t->level_b >= 2) {
			for (j = 0; j < ctx->tower_count; j++) {
				Tower* nb = &ctx->all_towers[j];
				if (nb->type != TOWER_MINE || nb == t || nb->ammo <= 0) continue;
				float mx = (float)(nb->x - t->x);
				float my = (float)(nb->y - t->y);
				if (mx*mx + my*my > 200.0f*200.0f) continue;
				float nb_blast_r = (nb->level_a >= 1) ? nb->range * 1.7f : (float)nb->range;
				mine_explode(nb, ctx, nb_blast_r, 0.45f);
				if (nb->level_a >= 2) nb->timer2 = 0.3f;
				nb->ammo  = 0;
				nb->state = 1;
			}
		}
		
		t->ammo  = 0;
		t->state = 1;
		break;
	}
}

/* ============================================================
* update_cauldron — 炼丹炉
* ============================================================ */
void update_cauldron(Tower* t, float dt, TowerCtx* ctx) {
	int i;
	float dx, dy;
	(void)dt;
	
	if (t->current_cooldown > 0.0f) return;
	
	if (t->level_b >= 1 && t->explosive_charges > 0) {
		/* B1：炸药配方（有限次爆炸）*/
		for (i = 0; i < ctx->enemy_count; i++) {
			if (!ctx->enemies[i].active || ctx->enemies[i].hp <= 0) continue;
			if (ctx->enemies[i].is_invisible) continue;
			dx = ctx->enemies[i].x - (float)t->x;
			dy = ctx->enemies[i].y - (float)t->y;
			if (dx*dx + dy*dy > (float)t->range * t->range) continue;
			int dmg = (int)(40.0f * t->explosive_dmg_mult);
			tower_damage_enemy(&ctx->enemies[i], dmg, ctx->texts, ctx->text_count);
			/* B2：硝化——命中附加爆炸20%额外伤害 */
			if (t->level_b >= 2)
				tower_damage_enemy(&ctx->enemies[i], (int)(dmg * 0.2f), ctx->texts, ctx->text_count);
		}
		t->explosive_dmg_mult *= 1.4f;
		t->explosive_charges--;
		t->current_cooldown = t->cooldown;
	} else if (t->level_a >= 1) {
		/* A1：剧毒——给范围内敌人上 DoT；A2破隐，所以A2不过滤隐身 */
		for (i = 0; i < ctx->enemy_count; i++) {
			if (!ctx->enemies[i].active || ctx->enemies[i].hp <= 0) continue;
			if (ctx->enemies[i].is_invisible && t->level_a < 2) continue;
			dx = ctx->enemies[i].x - (float)t->x;
			dy = ctx->enemies[i].y - (float)t->y;
			if (dx*dx + dy*dy > (float)t->range * t->range) continue;
			enemy_apply_dot(&ctx->enemies[i], 5.0f, 8);
			/* A2：毒雾破隐身 */
			if (t->level_a >= 2) {
				enemy_reveal(&ctx->enemies[i]);
				tower_damage_enemy(&ctx->enemies[i], 20, ctx->texts, ctx->text_count);
			}
		}
		t->current_cooldown = t->cooldown;
	} else {
		/* 基础：沸水低伤害 */
		for (i = 0; i < ctx->enemy_count; i++) {
			if (!ctx->enemies[i].active || ctx->enemies[i].hp <= 0) continue;
			if (ctx->enemies[i].is_invisible) continue;
			dx = ctx->enemies[i].x - (float)t->x;
			dy = ctx->enemies[i].y - (float)t->y;
			if (dx*dx + dy*dy > (float)t->range * t->range) continue;
			tower_damage_enemy(&ctx->enemies[i], (int)(t->damage * t->damage_mult), ctx->texts, ctx->text_count);
		}
		t->current_cooldown = t->cooldown;
	}
	
	/* Cleanse: restore purple (plague-miasma) people towers in range back to blue */
	for (i = 0; i < ctx->tower_count; i++) {
		if (ctx->all_towers[i].faction != FACTION_PEOPLE) continue;
		if (ctx->all_towers[i].color_state != TOWER_COLOR_PURPLE) continue;
		dx = (float)(ctx->all_towers[i].x - t->x);
		dy = (float)(ctx->all_towers[i].y - t->y);
		if (dx*dx + dy*dy <= (float)t->range * t->range) {
			ctx->all_towers[i].color_state     = TOWER_COLOR_BLUE;
			ctx->all_towers[i].buff_speed_ratio = 1.15f;
			ctx->all_towers[i].buff_timer       = 0.5f;
			ctx->all_towers[i].damage_mult      = 1.1f;
		}
	}
}

/* ============================================================
* update_firecrow — 飞鸦炮
* ============================================================ */
void update_firecrow(Tower* t, float dt, TowerCtx* ctx) {
	int i, best;
	float dx, dy;
	
	/* --- 火枪 --- */
	if (t->current_cooldown <= 0.0f && *ctx->metal >= 10) {
		best = -1;
		for (i = 0; i < ctx->enemy_count; i++) {
			if (!ctx->enemies[i].active || ctx->enemies[i].hp <= 0) continue;
			if (ctx->enemies[i].is_invisible) continue;
			dx = ctx->enemies[i].x - (float)t->x;
			dy = ctx->enemies[i].y - (float)t->y;
			if (dx*dx + dy*dy <= (float)t->range * t->range) { best = i; break; }
		}
		if (best >= 0) {
			*ctx->metal -= 10;
			tower_damage_enemy(&ctx->enemies[best], (int)(t->damage * t->damage_mult), ctx->texts, ctx->text_count);
			if (*ctx->proj_count < MAX_PROJECTILES) {
				projectile_spawn_full(ctx->projs, ctx->proj_count,
									  (float)t->x, (float)t->y,
									  ctx->enemies[best].x, ctx->enemies[best].y,
									  -1, 0, 1, 0, 0.1f);
			}
			t->current_cooldown = (t->level_a >= 1) ? 3.0f : t->cooldown;
		}
	}
	
	/* --- 飞鸦 --- */
	if (t->crow_current_cd > 0.0f)
		t->crow_current_cd -= dt * t->buff_speed_ratio;
	
	if (t->crow_current_cd <= 0.0f && *ctx->wood >= 30) {
		int crow_range = 320;
		best = -1;
		for (i = 0; i < ctx->enemy_count; i++) {
			if (!ctx->enemies[i].active || ctx->enemies[i].hp <= 0) continue;
			if (ctx->enemies[i].is_invisible) continue;
			dx = ctx->enemies[i].x - (float)t->x;
			dy = ctx->enemies[i].y - (float)t->y;
			if (dx*dx + dy*dy <= (float)crow_range * crow_range) { best = i; break; }
		}
		if (best >= 0) {
			*ctx->wood -= 30;
			/* B1：飞行途中伤害50 */
			if (t->level_b >= 1)
				tower_damage_enemy(&ctx->enemies[best], 50, ctx->texts, ctx->text_count);
			if (*ctx->proj_count < MAX_PROJECTILES) {
				projectile_spawn_full(ctx->projs, ctx->proj_count,
									  (float)t->x, (float)t->y,
									  ctx->enemies[best].x, ctx->enemies[best].y,
									  ctx->enemies[best].id, 70, 1, 1, 0.5f);
			}
			t->crow_current_cd = (t->level_a >= 1) ? 3.0f : t->crow_cooldown;
		}
	}
}
