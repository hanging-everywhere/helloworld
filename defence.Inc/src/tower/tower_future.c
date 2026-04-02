/**
 * @file    tower_future.c
 * @brief   科幻未来炮塔更新逻辑：脉冲炮、重力井、传送门、时钟塔、激光塔
 *          Sci-Fi era tower update logic: Pulse, Gravity, Portal, TimeClock, Laser.
 * @version 2.0  迁移自 prototype/v2/Tower.c
 */

#include "tower_internal.h"
#include <math.h>

/* ============================================================
* update_pulse — 脉冲炮
* ============================================================ */
void update_pulse(Tower* t, float dt, TowerCtx* ctx) {
	int i, j, best;
	float dx, dy;
	(void)dt;
	
	if (t->current_cooldown > 0.0f) return;
	
	/* 找第一个目标 */
	best = -1;
	for (i = 0; i < ctx->enemy_count; i++) {
		if (!ctx->enemies[i].active || ctx->enemies[i].hp <= 0) continue;
		if (ctx->enemies[i].is_invisible) continue;
		dx = ctx->enemies[i].x - (float)t->x;
		dy = ctx->enemies[i].y - (float)t->y;
		if (dx*dx + dy*dy <= (float)t->range * t->range) { best = i; break; }
	}
	if (best < 0) return;
	
	/* B2：切割模式——打范围内所有敌人，CD 15秒 */
	if (t->level_b >= 2) {
		for (i = 0; i < ctx->enemy_count; i++) {
			if (!ctx->enemies[i].active || ctx->enemies[i].hp <= 0) continue;
			if (ctx->enemies[i].is_invisible) continue;
			dx = ctx->enemies[i].x - (float)t->x;
			dy = ctx->enemies[i].y - (float)t->y;
			if (dx*dx + dy*dy <= (float)t->range * t->range)
				tower_damage_enemy(&ctx->enemies[i], t->damage, ctx->texts, ctx->text_count);
		}
		t->current_cooldown = 15.0f;
		return;
	}
	
	/* 连锁跳跃（最多4次） */
	int prev_id  = -1;
	int cur_idx  = best;
	float dmg    = (float)t->damage;
	for (j = 0; j < 4; j++) {
		if (cur_idx < 0) break;
		tower_damage_enemy(&ctx->enemies[cur_idx], (int)dmg, ctx->texts, ctx->text_count);
		if (t->level_a >= 2)
			enemy_apply_stun(&ctx->enemies[cur_idx], 1.0f);
		if (*ctx->proj_count < MAX_PROJECTILES) {
			float sx = (j == 0) ? (float)t->x :
			(prev_id >= 0 ? ctx->enemies[prev_id].x : ctx->enemies[cur_idx].x);
			float sy = (j == 0) ? (float)t->y :
			(prev_id >= 0 ? ctx->enemies[prev_id].y : ctx->enemies[cur_idx].y);
			projectile_spawn_full(ctx->projs, ctx->proj_count,
								  sx, sy,
								  ctx->enemies[cur_idx].x, ctx->enemies[cur_idx].y,
								  -1, 0, 1, 0, 0.08f);
		}
		prev_id = cur_idx;
		if (t->level_a < 1) dmg *= 0.5f; /* A1升级后不衰减 */
		
		/* 找下一个跳跃目标（80px范围内） */
		cur_idx = -1;
		for (i = 0; i < ctx->enemy_count; i++) {
			if (i == prev_id || !ctx->enemies[i].active || ctx->enemies[i].hp <= 0) continue;
			float jx = ctx->enemies[i].x - ctx->enemies[prev_id].x;
			float jy = ctx->enemies[i].y - ctx->enemies[prev_id].y;
			if (jx*jx + jy*jy <= 80.0f*80.0f) { cur_idx = i; break; }
		}
	}
	t->current_cooldown = t->cooldown;
	
	/* Pulse clears radiation contamination (green ground) on nearby towers */
	for (i = 0; i < ctx->tower_count; i++) {
		if (ctx->all_towers[i].color_state != TOWER_COLOR_GREEN) continue;
		dx = (float)(ctx->all_towers[i].x - t->x);
		dy = (float)(ctx->all_towers[i].y - t->y);
		if (dx*dx + dy*dy <= (float)t->range * t->range) {
			ctx->all_towers[i].color_state = TOWER_COLOR_NORMAL;
			ctx->all_towers[i].is_frozen   = 0;
			ctx->all_towers[i].frozen_timer= 0.0f;
		}
	}
}

/* ============================================================
* update_gravity — 重力井
* ============================================================ */
void update_gravity(Tower* t, float dt, TowerCtx* ctx) {
	int i;
	float dx, dy;
	float g_range = (t->level_a >= 1) ? t->range * 1.3f : (float)t->range;
	
	/* 基础：每帧持续减速50%范围内敌人 */
	for (i = 0; i < ctx->enemy_count; i++) {
		if (!ctx->enemies[i].active || ctx->enemies[i].hp <= 0) continue;
		if (ctx->enemies[i].is_invisible) continue;
		dx = ctx->enemies[i].x - (float)t->x;
		dy = ctx->enemies[i].y - (float)t->y;
		if (dx*dx + dy*dy <= g_range * g_range)
			enemy_apply_slow(&ctx->enemies[i], 0.15f, 0.5f);
	}
	
	/* A2：手动引爆（CD 15秒，由 gravityExplodeReady 标记触发） */
	if (t->gravity_explode_ready && t->level_a >= 2 && t->current_cooldown <= 0.0f) {
		for (i = 0; i < ctx->enemy_count; i++) {
			if (!ctx->enemies[i].active || ctx->enemies[i].hp <= 0) continue;
			if (ctx->enemies[i].is_invisible) continue;
			dx = ctx->enemies[i].x - (float)t->x;
			dy = ctx->enemies[i].y - (float)t->y;
			if (dx*dx + dy*dy <= g_range * g_range) {
				tower_damage_enemy(&ctx->enemies[i], 180, ctx->texts, ctx->text_count);
				enemy_apply_stun(&ctx->enemies[i], 2.0f);
			}
		}
		t->gravity_explode_ready = 0;
		t->current_cooldown = 15.0f;
	}
	
	/* B1：反重力悬浮 / B2：黑洞吞噬 */
	if (t->level_b >= 1) {
		float b_range = 224.0f; /* 7格 */
		if (t->timer2 > 0.0f) {
			/* 反重力持续中：持续刷新stunTimer */
			t->timer2 -= dt;
			for (i = 0; i < ctx->enemy_count; i++) {
				if (!ctx->enemies[i].active || ctx->enemies[i].hp <= 0) continue;
				dx = ctx->enemies[i].x - (float)t->x;
				dy = ctx->enemies[i].y - (float)t->y;
				if (dx*dx + dy*dy > b_range * b_range) continue;
				enemy_apply_stun(&ctx->enemies[i], 0.15f);
				/* B2：黑洞吞噬 */
				if (t->level_b >= 2 && t->charge_time <= 0.0f) {
					ctx->enemies[i].hp = 0;
					tower_add_float_text(ctx->texts, ctx->text_count,
										 ctx->enemies[i].x, ctx->enemies[i].y, -4);
				}
			}
		} else {
			/* 反重力冷却中 */
			if (t->current_cooldown <= 0.0f) {
				t->timer2 = 4.0f;
				t->current_cooldown = (t->level_b >= 2) ? 20.0f : 8.0f;
				if (t->level_b >= 2) t->charge_time = 4.0f;
			}
		}
		if (t->charge_time > 0.0f) t->charge_time -= dt;
	}
}

/* ============================================================
* update_portal — 传送门
* ============================================================ */
void update_portal(Tower* t, float dt, TowerCtx* ctx) {
	int i, j;
	float dx, dy;
	
	/* 只有已激活且已配对的 B 门（role=1）才检测敌人 */
	if (t->state != 1 || t->paired_index < 0) return;
	if (t->portal_role != 1) return;
	
	if (t->portal_collapse_cd > 0.0f) {
		t->portal_collapse_cd -= dt;
		return;
	}
	
	for (i = 0; i < ctx->enemy_count; i++) {
		if (!ctx->enemies[i].active || ctx->enemies[i].hp <= 0) continue;
		dx = ctx->enemies[i].x - (float)t->x;
		dy = ctx->enemies[i].y - (float)t->y;
		if (dx*dx + dy*dy > 32.0f*32.0f) continue;
		
		/* 传送次数上限 */
		Tower* gate_a = &ctx->all_towers[t->paired_index];
		int max_tele  = (gate_a->level_b >= 1) ? 2 : 1;
		if (ctx->enemies[i].portal_count >= max_tele) continue;
		
		/* 传送到 A 门位置 */
		ctx->enemies[i].x = (float)gate_a->x;
		ctx->enemies[i].y = (float)gate_a->y;
		enemy_apply_stun(&ctx->enemies[i], 1.0f);
		ctx->enemies[i].portal_count++;
		
		/* A1：空间切割伤害 */
		if (gate_a->level_a >= 1)
			tower_damage_enemy(&ctx->enemies[i], 240, ctx->texts, ctx->text_count);
		
		t->portal_tele_count++;
		gate_a->portal_tele_count++;
		
		/* B2：15个敌人后坍缩 */
		if (gate_a->level_b >= 2 && t->portal_tele_count >= 15) {
			float col_r = 300.0f;
			for (j = 0; j < ctx->enemy_count; j++) {
				if (!ctx->enemies[j].active) continue;
				dx = ctx->enemies[j].x - (float)t->x;
				dy = ctx->enemies[j].y - (float)t->y;
				if (dx*dx + dy*dy <= col_r * col_r) {
					ctx->enemies[j].hp = 0;
					tower_add_float_text(ctx->texts, ctx->text_count,
										 ctx->enemies[j].x, ctx->enemies[j].y, -4);
				}
			}
			t->portal_tele_count   = 0;
			gate_a->portal_tele_count = 0;
			t->portal_collapse_cd  = 30.0f;
		}
	}
}

/* ============================================================
* update_timeclock — 时钟塔
* ============================================================ */
void update_timeclock(Tower* t, float dt, TowerCtx* ctx) {
	int i;
	float dx, dy;
	
	if (t->current_cooldown <= 0.0f) {
		/* 时间波刚触发：处理 A2/B1 一次性效果 */
		for (i = 0; i < ctx->enemy_count; i++) {
			if (!ctx->enemies[i].active || ctx->enemies[i].hp <= 0) continue;
			if (ctx->enemies[i].is_invisible) continue;
			dx = ctx->enemies[i].x - (float)t->x;
			dy = ctx->enemies[i].y - (float)t->y;
			if (dx*dx + dy*dy > (float)t->range * t->range) continue;
			
			/* A2：时间裂隙——仅在触发瞬间回溯 */
			if (t->level_a >= 2) {
				ctx->enemies[i].x = ctx->enemies[i].saved_x;
				ctx->enemies[i].y = ctx->enemies[i].saved_y;
				ctx->enemies[i].target_waypoint_index = ctx->enemies[i].saved_waypoint_index;
			}
			/* B1：因果律标记 */
			if (t->level_b >= 1 && !ctx->enemies[i].cause_mark_active) {
				ctx->enemies[i].cause_mark_active  = 1;
				ctx->enemies[i].cause_timer        = 3.0f;
				ctx->enemies[i].cause_damage_accum = 0;
			}
		}
		t->time_wave_timer = 2.0f;
		t->current_cooldown = t->cooldown;
	}
	
	/* 时间波持续期：A1静止 / 基础减速（每帧刷新） */
	if (t->time_wave_timer > 0.0f) {
		t->time_wave_timer -= dt;
		for (i = 0; i < ctx->enemy_count; i++) {
			if (!ctx->enemies[i].active || ctx->enemies[i].hp <= 0) continue;
			if (ctx->enemies[i].is_invisible) continue;
			dx = ctx->enemies[i].x - (float)t->x;
			dy = ctx->enemies[i].y - (float)t->y;
			if (dx*dx + dy*dy > (float)t->range * t->range) continue;
			if (t->level_a >= 1)
				enemy_apply_stun(&ctx->enemies[i], 0.15f);
			else
				enemy_apply_slow(&ctx->enemies[i], 0.15f, 0.6f);
		}
	}
}

/* ============================================================
* update_laser — 激光塔
* ============================================================ */
void update_laser(Tower* t, float dt, TowerCtx* ctx) {
	int i, j, found;
	float dx, dy;
	
	if (t->laser_pulse_cd > 0.0f) {
		t->laser_pulse_cd -= dt;
		return;
	}
	
	/* B1：单发模式（1.5秒/发） */
	if (t->level_b >= 1) {
		if (t->current_cooldown > 0.0f) return;
		int best = -1;
		for (i = 0; i < ctx->enemy_count; i++) {
			if (!ctx->enemies[i].active || ctx->enemies[i].hp <= 0) continue;
			if (ctx->enemies[i].is_invisible) continue;
			dx = ctx->enemies[i].x - (float)t->x;
			dy = ctx->enemies[i].y - (float)t->y;
			if (dx*dx + dy*dy <= (float)t->range * t->range) { best = i; break; }
		}
		if (best < 0) return;
		tower_damage_enemy(&ctx->enemies[best], 150, ctx->texts, ctx->text_count);
		/* B2：切割链，跳3次 */
		if (t->level_b >= 2) {
			int prev = best;
			for (j = 0; j < 3; j++) {
				int next = -1;
				for (i = 0; i < ctx->enemy_count; i++) {
					if (i == prev || !ctx->enemies[i].active || ctx->enemies[i].hp <= 0) continue;
					float jx = ctx->enemies[i].x - ctx->enemies[prev].x;
					float jy = ctx->enemies[i].y - ctx->enemies[prev].y;
					if (jx*jx + jy*jy <= 100.0f*100.0f) { next = i; break; }
				}
				if (next < 0) break;
				tower_damage_enemy(&ctx->enemies[next], 150, ctx->texts, ctx->text_count);
				prev = next;
			}
		}
		t->current_cooldown = 1.5f;
		return;
	}
	
	/* A2：超级脉冲（需要2储能） */
	if (t->level_a >= 2 && t->laser_charges >= 2) {
		int best = -1;
		for (i = 0; i < ctx->enemy_count; i++) {
			if (!ctx->enemies[i].active || ctx->enemies[i].hp <= 0) continue;
			if (ctx->enemies[i].is_invisible) continue;
			dx = ctx->enemies[i].x - (float)t->x;
			dy = ctx->enemies[i].y - (float)t->y;
			if (dx*dx + dy*dy <= (float)t->range * t->range) { best = i; break; }
		}
		if (best >= 0) {
			tower_damage_enemy(&ctx->enemies[best], 800, ctx->texts, ctx->text_count);
			t->laser_charges    = 0;
			t->laser_pulse_cd   = 6.0f;
			t->laser_beam_timer = 0.3f;
		}
		return;
	}
	
	/* 基础/A1：持续照射递增伤害 */
	if (t->target_id >= 0) {
		/* 验证目标仍有效 */
		found = 0;
		for (i = 0; i < ctx->enemy_count; i++) {
			if (ctx->enemies[i].id != t->target_id) continue;
			if (!ctx->enemies[i].active || ctx->enemies[i].hp <= 0) break;
			dx = ctx->enemies[i].x - (float)t->x;
			dy = ctx->enemies[i].y - (float)t->y;
			if (dx*dx + dy*dy <= (float)t->range * t->range) { found = 1; break; }
		}
		if (!found) {
			t->target_id      = -1;
			t->laser_beam_timer = 0.0f;
			t->timer1           = 0.0f;
		}
	}
	
	/* 寻找新目标 */
	if (t->target_id < 0) {
		for (i = 0; i < ctx->enemy_count; i++) {
			if (!ctx->enemies[i].active || ctx->enemies[i].hp <= 0) continue;
			if (ctx->enemies[i].is_invisible) continue;
			dx = ctx->enemies[i].x - (float)t->x;
			dy = ctx->enemies[i].y - (float)t->y;
			if (dx*dx + dy*dy <= (float)t->range * t->range) {
				t->target_id      = ctx->enemies[i].id;
				t->timer1           = 0.0f;
				t->laser_beam_timer = (t->level_a >= 1) ? 4.0f : 6.0f;
				break;
			}
		}
	}
	
	/* 持续照射结算（每1秒结算一次） */
	if (t->target_id >= 0) {
		t->current_cooldown -= dt;
		if (t->current_cooldown <= 0.0f) {
			t->current_cooldown = 1.0f;
			t->timer1 += 1.0f;
			float dmg_mult = 1.0f;
			for (j = 1; j < (int)t->timer1; j++) dmg_mult *= 2.0f;
			int dmg = (int)(t->damage * dmg_mult);
			for (i = 0; i < ctx->enemy_count; i++) {
				if (ctx->enemies[i].id != t->target_id) continue;
				tower_damage_enemy(&ctx->enemies[i], dmg, ctx->texts, ctx->text_count);
				break;
			}
			/* A1：储能计数 */
			if (t->level_a >= 1 && t->laser_charges < 2)
				t->laser_charges++;
		}
		t->laser_beam_timer -= dt;
		if (t->laser_beam_timer <= 0.0f) {
			t->target_id        = -1;
			t->timer1           = 0.0f;
			t->current_cooldown = 3.0f;
			t->laser_beam_timer = 0.0f;
		}
	}
}
