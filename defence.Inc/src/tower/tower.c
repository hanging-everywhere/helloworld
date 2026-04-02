#include "tower_internal.h"
#include <math.h>

void tower_add_float_text(FloatingText* texts, int* count,
						  float x, float y, int dmg) {
	floattext_spawn(texts, count, x, y, dmg);
}

void tower_damage_enemy(EnemyState* e, int dmg,
						FloatingText* texts, int* count) {
	if (e->cause_mark_active) e->cause_damage_accum += dmg;
	enemy_take_damage(e, dmg);
	tower_add_float_text(texts, count, e->x, e->y, dmg);
}

/* ============================================================
* tower_init
* ============================================================ */
void tower_init(Tower* t, int x, int y, int era, int type) {
	t->x=x; t->y=y; t->era=era; t->type=type;
	t->level_a=0; t->level_b=0;
	t->current_cooldown=0.0f;
	t->timer1=0.0f; t->timer2=0.0f;
	t->state=0; t->target_id=-1;
	t->buff_timer=0.0f; t->buff_speed_ratio=1.0f;
	t->paired_index=-1;
	t->current_frame=0; t->frames_counter=0;
	t->frames_speed=15; t->max_frames=1;
	t->frame_rec=(Rectangle){0,0,32,32};
	t->ammo=0;
	t->hit_count_on_target=0; t->last_target_id=-1;
	t->reload_timer=0.0f;
	t->explosive_charges=0; t->explosive_dmg_mult=1.0f;
	t->crow_cooldown=8.0f; t->crow_current_cd=0.0f;
	t->charge_time=0.0f; t->is_charging=0;
	t->gravity_explode_ready=0;
	t->portal_role=0; t->portal_tele_count=0; t->portal_collapse_cd=0.0f;
	t->time_wave_timer=0.0f;
	t->laser_beam_timer=0.0f; t->laser_charges=0; t->laser_pulse_cd=0.0f;
	
	/* 新字段 */
	t->faction      = FACTION_NORMAL;
	t->color_state  = TOWER_COLOR_NORMAL;
	t->is_frozen    = 0;
	t->frozen_timer = 0.0f;
	t->is_betrayed  = 0;
	t->is_buried    = 0;
	t->bury_timer   = 0.0f;
	t->is_bonfire   = 0;
	t->damage_mult  = 1.0f;
	t->hp           = 0;
	t->is_ai_tower  = 0;
	
	switch (type) {
	case TOWER_SLING:
		t->range=150; t->damage=20; t->cooldown=2.0f; t->ammo=12;
		t->max_frames=4; t->paired_index=70;
		break;
	case TOWER_BONESPEAR:
		t->range=40; t->damage=45; t->cooldown=0.5f; t->ammo=3;
		t->timer1=3.0f; t->state=0; t->max_frames=4;
		break;
	case TOWER_TOTEM:
		t->range=180; t->damage=0; t->cooldown=7.0f;
		break;
	case TOWER_FIRE:
		t->range=70; t->damage=15; t->cooldown=2.0f; t->state=1;
		break;
	case TOWER_CROSSBOW:
		t->range=256; t->damage=40; t->cooldown=3.0f;
		t->state=0; t->timer1=3.0f;
		break;
	case TOWER_MINE:
		t->range=48; t->damage=120; t->cooldown=0.0f;
		t->ammo=1; t->state=0; t->reload_timer=0.0f;
		break;
	case TOWER_CAULDRON:
		t->range=160; t->damage=5; t->cooldown=5.0f;
		t->explosive_charges=0; t->explosive_dmg_mult=1.0f;
		break;
	case TOWER_FIRECROW:
		t->range=448; t->damage=85; t->cooldown=5.0f;
		t->crow_cooldown=8.0f; t->crow_current_cd=0.0f;
		break;
	case TOWER_PULSE:
		t->range=300; t->damage=200; t->cooldown=3.0f;
		break;
	case TOWER_GRAVITY:
		t->range=160; t->damage=0; t->cooldown=0.1f;
		break;
	case TOWER_PORTAL:
		t->range=32; t->damage=0; t->cooldown=0.0f;
		t->paired_index=-1; t->state=0;
		break;
	case TOWER_TIMECLOCK:
		t->range=200; t->damage=0; t->cooldown=7.0f;
		break;
	case TOWER_LASER:
		t->range=700; t->damage=25; t->cooldown=0.1f;
		t->target_id=-1; t->laser_beam_timer=0.0f;
		break;
	default:
		t->range=100; t->damage=10; t->cooldown=1.0f;
		break;
	}
}

/* ============================================================
* 石器时代更新函数
* ============================================================ */

void update_sling(Tower* t, float dt, TowerCtx* ctx) {
	int best, i;
	float dx, dy;
	int stone_cost = (t->level_a>=2) ? 10 : (t->level_b>=1 ? 7 : 5);
	(void)dt;
	if (t->current_cooldown>0.0f || t->ammo<=0) return;
	if (*ctx->stone < stone_cost) return;
	best = -1;
	for (i=0; i<ctx->enemy_count; i++) {
		if (!ctx->enemies[i].active || ctx->enemies[i].hp<=0) continue;
		if (ctx->enemies[i].is_invisible) continue;
		dx = ctx->enemies[i].x - (float)t->x;
		dy = ctx->enemies[i].y - (float)t->y;
		if (dx*dx+dy*dy <= (float)t->range*t->range) { best=i; break; }
	}
	if (best < 0) return;
	*ctx->stone -= stone_cost;
	t->ammo--;
	int is_hit = (GetRandomValue(1,100) <= t->paired_index) ? 1 : 0;
	t->current_cooldown = t->cooldown;
	if (*ctx->proj_count < MAX_PROJECTILES) {
		if (is_hit) {
			projectile_spawn_full(ctx->projs, ctx->proj_count,
								  (float)t->x, (float)t->y,
								  ctx->enemies[best].x, ctx->enemies[best].y,
								  ctx->enemies[best].id, t->damage, 1, 0, 0.2f);
		} else {
			float tx = ctx->enemies[best].x + (float)GetRandomValue(-35,35);
			float ty = ctx->enemies[best].y + (float)GetRandomValue(-35,35);
			projectile_spawn_full(ctx->projs, ctx->proj_count,
								  (float)t->x, (float)t->y, tx, ty, -1, 0, 0, 0, 0.2f);
			tower_add_float_text(ctx->texts, ctx->text_count,
								 ctx->enemies[best].x, ctx->enemies[best].y, -1);
		}
	}
}

void update_bonespear(Tower* t, float dt, TowerCtx* ctx) {
	int i, hit, hit_count;
	float dx, dy;
	if (t->ammo<=0) return;
	if (t->state==0) { t->timer1-=dt; if(t->timer1<=0.0f) t->state=1; return; }
	if (t->current_cooldown>0.0f) return;
	hit_count = (t->level_b>=1) ? 3 : 1;
	hit = 0;
	for (i=0; i<ctx->enemy_count && hit<hit_count; i++) {
		if (!ctx->enemies[i].active || ctx->enemies[i].hp<=0) continue;
		/* Only B1 upgrade can reveal; base/A path cannot hit invisible */
		if (ctx->enemies[i].is_invisible && t->level_b<1) continue;
		dx = ctx->enemies[i].x - (float)t->x;
		dy = ctx->enemies[i].y - (float)t->y;
		if (dx*dx+dy*dy > (float)t->range*t->range) continue;
		tower_damage_enemy(&ctx->enemies[i], (int)(t->damage * t->damage_mult), ctx->texts, ctx->text_count);
		if (t->level_b>=1) enemy_reveal(&ctx->enemies[i]);
		if (t->level_a>=1) enemy_apply_slow(&ctx->enemies[i], 2.0f, 0.8f);
		if (t->level_a>=2) enemy_apply_dot (&ctx->enemies[i], 3.0f, 8);
		hit++;
	}
	if (hit>0) { t->ammo--; t->current_cooldown=t->cooldown; }
}

void update_totem(Tower* t, float dt, TowerCtx* ctx) {
	int i; float dx, dy, stun_dur; (void)dt;
	if (t->current_cooldown>0.0f) return;
	if (t->level_a>=1) {
		for (i=0; i<ctx->tower_count; i++) {
			if (ctx->all_towers[i].type==TOWER_TOTEM) continue;
			dx = (float)(ctx->all_towers[i].x - t->x);
			dy = (float)(ctx->all_towers[i].y - t->y);
			if (dx*dx+dy*dy <= (float)t->range*t->range) {
				ctx->all_towers[i].buff_timer=3.0f;
				ctx->all_towers[i].buff_speed_ratio=1.2f;
			}
		}
	} else {
		stun_dur = (t->level_b>=1) ? 5.0f : 3.0f;
		for (i=0; i<ctx->enemy_count; i++) {
			if (!ctx->enemies[i].active || ctx->enemies[i].hp<=0) continue;
			if (ctx->enemies[i].is_invisible) continue;
			dx = ctx->enemies[i].x - (float)t->x;
			dy = ctx->enemies[i].y - (float)t->y;
			if (dx*dx+dy*dy <= (float)t->range*t->range)
				enemy_apply_stun(&ctx->enemies[i], stun_dur);
		}
	}
	if (t->level_b>=2) {
		for (i=0; i<ctx->enemy_count; i++) {
			if (!ctx->enemies[i].active || ctx->enemies[i].hp<=0) continue;
			if (ctx->enemies[i].is_invisible) continue;
			dx = ctx->enemies[i].x - (float)t->x;
			dy = ctx->enemies[i].y - (float)t->y;
			if (dx*dx+dy*dy <= (float)t->range*t->range)
				tower_damage_enemy(&ctx->enemies[i], 30, ctx->texts, ctx->text_count);
		}
	}
	tower_add_float_text(ctx->texts, ctx->text_count, (float)t->x, (float)t->y+10, -3);
	t->timer2=0.5f; t->current_cooldown=t->cooldown;
}

void update_fire(Tower* t, float dt, TowerCtx* ctx) {
	int i, j, triggered; float dx, dy;
	if (t->level_a<2) {
		t->timer1+=dt;
		if (t->timer1>=1.0f) {
			t->timer1-=1.0f;
			if (*ctx->wood>=1) { (*ctx->wood)--; if(t->state==0) t->state=1; }
			else t->state=0;
		}
	} else { t->state=2; }
	if (t->timer2>0.0f) t->timer2-=dt;
	if ((t->state==1||t->state==2) && t->current_cooldown<=0.0f) {
		triggered=0;
		for (i=0; i<ctx->enemy_count; i++) {
			if (!ctx->enemies[i].active || ctx->enemies[i].hp<=0) continue;
			dx = ctx->enemies[i].x - (float)t->x;
			dy = ctx->enemies[i].y - (float)t->y;
			if (dx*dx+dy*dy > (float)t->range*t->range) continue;
			tower_damage_enemy(&ctx->enemies[i], (int)(t->damage * t->damage_mult), ctx->texts, ctx->text_count);
			enemy_apply_dot(&ctx->enemies[i], 3.0f, 5);
			enemy_reveal(&ctx->enemies[i]);
			if (t->level_a>=1) {
				for (j=0; j<ctx->enemy_count; j++) {
					if (j==i || !ctx->enemies[j].active) continue;
					float ddx=ctx->enemies[j].x-ctx->enemies[i].x;
					float ddy=ctx->enemies[j].y-ctx->enemies[i].y;
					if (ddx*ddx+ddy*ddy<=50.0f*50.0f)
						enemy_apply_dot(&ctx->enemies[j], 3.0f, 5);
				}
			}
			triggered=1;
		}
		if (triggered) { t->current_cooldown=t->cooldown; t->timer2=0.3f; }
	}
}

/* ============================================================
* tower_update — 主调度
* ============================================================ */
void tower_update(Tower* t, float dt,
				  EnemyState* enemies, int enemy_count,
				  Tower* all_towers, int tower_count,
				  VisualProjectile* projs, int* proj_count,
				  FloatingText* texts, int* text_count,
				  int* wood, int* stone, int* metal, int* material,
				  BurnZone* burn_zones, int* burn_zone_count) {
	
	/* 叛变/掩埋时跳过所有逻辑 */
	if (t->is_betrayed || t->is_buried) return;
	
	/* 冻结：倒计时，期间不攻击 */
	if (t->is_frozen) {
		t->frozen_timer -= dt;
		if (t->frozen_timer <= 0.0f) { t->is_frozen=0; t->frozen_timer=0.0f; }
		return;
	}
	
	TowerCtx ctx;
	ctx.enemies=enemies;       ctx.enemy_count   =enemy_count;
	ctx.all_towers=all_towers; ctx.tower_count   =tower_count;
	ctx.projs=projs;           ctx.proj_count    =proj_count;
	ctx.texts=texts;           ctx.text_count    =text_count;
	ctx.wood=wood;             ctx.stone         =stone;
	ctx.metal=metal;           ctx.material      =material;
	ctx.burn_zones=burn_zones; ctx.burn_zone_count=burn_zone_count;
	
	/* 序列帧动画 */
	if (t->max_frames>1) {
		t->frames_counter++;
		if (t->frames_counter>=t->frames_speed) {
			t->frames_counter=0;
			t->current_frame=(t->current_frame+1)%t->max_frames;
			t->frame_rec.x=(float)t->current_frame*t->frame_rec.width;
		}
	}
	
	/* Buff */
	if (t->buff_timer>0.0f) {
		t->buff_timer-=dt;
		if (t->buff_timer<=0.0f) t->buff_speed_ratio=1.0f;
	}
	if (t->current_cooldown>0.0f)
		t->current_cooldown -= dt * t->buff_speed_ratio;
	
	switch (t->type) {
		case TOWER_SLING:     update_sling    (t,dt,&ctx); break;
		case TOWER_BONESPEAR: update_bonespear(t,dt,&ctx); break;
		case TOWER_TOTEM:     update_totem    (t,dt,&ctx); break;
		case TOWER_FIRE:      update_fire     (t,dt,&ctx); break;
		case TOWER_CROSSBOW:  update_crossbow (t,dt,&ctx); break;
		case TOWER_MINE:      update_mine     (t,dt,&ctx); break;
		case TOWER_CAULDRON:  update_cauldron (t,dt,&ctx); break;
		case TOWER_FIRECROW:  update_firecrow (t,dt,&ctx); break;
		case TOWER_PULSE:     update_pulse    (t,dt,&ctx); break;
		case TOWER_GRAVITY:   update_gravity  (t,dt,&ctx); break;
		case TOWER_PORTAL:    update_portal   (t,dt,&ctx); break;
		case TOWER_TIMECLOCK: update_timeclock(t,dt,&ctx); break;
		case TOWER_LASER:     update_laser    (t,dt,&ctx); break;
		default: break;
	}
}
