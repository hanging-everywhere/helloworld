#include "enemy.h"
#include <math.h>

/* frame_count: sprite sheet column count, same format as towers (32x32 horizontal strip)*/
typedef struct { int hp; float speed; int naturally_invisible; int frame_count; } EnemyDef;

static const EnemyDef ENEMY_DEFS[ENEMY_TYPE_COUNT] = {
	/* SKELETON       */ {100,  60.0f, 0, 3},  /* Skeleton: 3 frames */
	/* WOLF           */ {170,  70.0f, 0, 4},
	/* BOAR           */ {300,  30.0f, 0, 4},
	/* SABERTOOTH     */ {230,  40.0f, 0, 4},
	/* MAMMOTH        */ {350,  45.0f, 0, 4},
	/* PEASANT        */ {300,  25.0f, 0, 3},  /* placeholder skeleton texture, 3 frames */
	/* CULT_BOSS      */ {1500, 20.0f, 0, 1},  /* runtime uses betrayed tower texture, 1 frame */
	/* RAT            */ {300,  45.0f, 1, 4},
	/* SLIME          */ {250,  50.0f, 0, 4},
	/* NOBLE_SKELETON */ {200,  60.0f, 0, 4},
	/* WRAITH         */ {200,  70.0f, 1, 4},
	/* SANDSTORM      */ {600,  40.0f, 0, 2},
	/* SEAWAVE        */ {300,  40.0f, 0, 2},
	/* RADIATION      */ {1200, 20.0f, 0, 4},
};

/* ============================================================
* enemy_init
* ============================================================ */
void enemy_init(EnemyState* e, Point start_pos, int id, EnemyType type) {
	const EnemyDef* d = &ENEMY_DEFS[type];
	
	e->id         = id;
	e->enemy_type = type;
	e->x          = (float)start_pos.x;
	e->y          = (float)start_pos.y;
	
	e->base_speed    = d->speed;
	e->current_speed = d->speed;
	e->max_hp        = d->hp;
	e->hp            = d->hp;
	
	e->target_waypoint_index  = 1;
	e->active       = 1;
	e->reached_base = 0;
	
	e->hit_flash_timer        = 0.0f;
	e->is_invisible           = d->naturally_invisible;
	e->is_naturally_invisible = d->naturally_invisible;
	
	e->current_frame  = 0;
	e->frames_counter = 0;
	e->frames_speed   = 12;   /* Animation speed (slightly faster than tower's 15) */
	e->frame_rec      = (Rectangle){0, 0, SKELETON_FRAME_WIDTH, SKELETON_FRAME_HEIGHT};
	
	e->stun_timer     = 0.0f;
	e->slow_timer     = 0.0f;
	e->slow_ratio     = 1.0f;
	e->dot_timer      = 0.0f;
	e->dot_tick_timer = 0.0f;
	e->dot_damage     = 0;
	
	e->portal_count           = 0;
	e->saved_x                = (float)start_pos.x;
	e->saved_y                = (float)start_pos.y;
	e->saved_waypoint_index   = 1;
	e->pos_record_timer       = 0.0f;
	
	e->cause_mark_active  = 0;
	e->cause_timer        = 0.0f;
	e->cause_damage_accum = 0;
	
	e->no_attack_timer    = 0.0f;
	e->explode_armed      = 0;
	e->explode_timer      = 0.0f;
	e->explosion_ready    = 0;
	e->radiation_pulse_cd = 0.0f;
	e->cult_tower_type    = 5; /* default: crossbow type */
}

/* ============================================================
* enemy_update
* ============================================================ */
int enemy_update(EnemyState* e, float dt,
				 const Point* waypoints, int waypoint_count) {
	float dx, dy, dist, move_dist;
	
	if (!e->active) return 0;
	
	if (e->hit_flash_timer > 0.0f) e->hit_flash_timer -= dt;
	
	/* Causality countdown */
	if (e->cause_mark_active) {
		e->cause_timer -= dt;
		if (e->cause_timer <= 0.0f) {
			e->cause_mark_active  = 0;
			e->hp                -= e->cause_damage_accum;
			e->cause_damage_accum = 0;
			e->hit_flash_timer    = 0.15f;
		}
	}
	
	/* Wolf: no-attack timer -> stealth + double speed */
	if (e->enemy_type == ENEMY_WOLF) {
		e->no_attack_timer += dt;
		if (e->no_attack_timer >= WOLF_STEALTH_DELAY && !e->is_invisible) {
			e->is_invisible  = 1;
			e->current_speed = e->base_speed * 2.0f;
		}
	}
	
	/* Slime: self-destruct countdown */
	if (e->enemy_type == ENEMY_SLIME && e->explode_armed) {
		e->explode_timer -= dt;
		if (e->explode_timer <= 0.0f) {
			e->explosion_ready = 1;
			e->active          = 0;
			return 0;
		}
	}
	
	/* Radiation: contamination pulse cooldown */
	if (e->enemy_type == ENEMY_RADIATION && e->radiation_pulse_cd > 0.0f)
		e->radiation_pulse_cd -= dt;
	
	/* Stunned: skip movement */
	if (e->stun_timer > 0.0f) {
		e->stun_timer -= dt;
		goto update_anim;
	}
	
	/* Slow */
	if (e->slow_timer > 0.0f) {
		e->slow_timer -= dt;
		if (!(e->enemy_type == ENEMY_WOLF && e->is_invisible))
			e->current_speed = e->base_speed * e->slow_ratio;
	} else {
		if (e->enemy_type == ENEMY_WOLF && e->is_invisible)
			e->current_speed = e->base_speed * 2.0f;
		else
			e->current_speed = e->base_speed;
	}
	
	/* DoT */
	if (e->dot_timer > 0.0f) {
		e->dot_timer      -= dt;
		e->dot_tick_timer -= dt;
		if (e->dot_tick_timer <= 0.0f) {
			if (e->cause_mark_active) e->cause_damage_accum += e->dot_damage;
			e->hp            -= e->dot_damage;
			e->hit_flash_timer = 0.1f;
			e->dot_tick_timer  = 1.0f;
		}
	}
	
	/* Position snapshot (TimeClock A2)*/
	e->pos_record_timer += dt;
	if (e->pos_record_timer >= 3.0f) {
		e->pos_record_timer     = 0.0f;
		e->saved_x              = e->x;
		e->saved_y              = e->y;
		e->saved_waypoint_index = e->target_waypoint_index;
	}
	
	/* Path movement */
	if (e->target_waypoint_index < waypoint_count) {
		float tx = (float)waypoints[e->target_waypoint_index].x;
		float ty = (float)waypoints[e->target_waypoint_index].y;
		dx   = tx - e->x;
		dy   = ty - e->y;
		dist = sqrtf(dx*dx + dy*dy);
		if (dist < 2.0f) {
			e->target_waypoint_index++;
		} else {
			move_dist = e->current_speed * dt;
			if (move_dist > dist) move_dist = dist;
			e->x += (dx / dist) * move_dist;
			e->y += (dy / dist) * move_dist;
		}
	} else {
		/* Environmental enemies vanish at destination without dealing base damage */
		if (e->enemy_type == ENEMY_SEAWAVE   ||
			e->enemy_type == ENEMY_SANDSTORM ||
			e->enemy_type == ENEMY_RADIATION) {
			e->active = 0;
			return 0;
		}
		e->active      = 0;
		e->reached_base = 1;
		return 1;
	}
	
	update_anim:
	e->frames_counter++;
	if (e->frames_counter >= e->frames_speed) {
		e->frames_counter = 0;
		e->current_frame++;
		if (e->current_frame >= ENEMY_DEFS[(int)e->enemy_type].frame_count) e->current_frame = 0;
		e->frame_rec.x = (float)e->current_frame * SKELETON_FRAME_WIDTH;
	}
	return 0;
}

/* ============================================================
* enemy_draw
* ============================================================ */
void enemy_draw(const EnemyState* e,
				Texture2D* enemy_textures,
				Texture2D* tower_textures) {
	int bw, cur_bar;
	Color draw_color;
	Vector2 origin;
	Rectangle dest_rec;
	Texture2D tex;
	
	if (!e->active) return;
	
	draw_color = WHITE;
	if (e->slow_timer       > 0.0f && e->hit_flash_timer <= 0.0f) draw_color = SKYBLUE;
	if (e->cause_mark_active && e->hit_flash_timer  <= 0.0f)      draw_color = PURPLE;
	if (e->hit_flash_timer  > 0.0f)                               draw_color = RED;
	
	/* Slime flashes orange when about to explode */
	if (e->enemy_type == ENEMY_SLIME && e->explode_armed &&
		e->hit_flash_timer <= 0.0f) {
		if ((int)(e->explode_timer * 4.0f) % 2 == 0) draw_color = ORANGE;
	}
	
	/* Cult boss uses betrayed tower texture */
	if (e->enemy_type == ENEMY_CULT_BOSS)
		tex = tower_textures[e->cult_tower_type];
	else
		tex = enemy_textures[e->enemy_type];
	
	origin   = (Vector2){ SKELETON_FRAME_WIDTH  / 2.0f,
		SKELETON_FRAME_HEIGHT / 2.0f };
	dest_rec = (Rectangle){ e->x, e->y,
		SKELETON_FRAME_WIDTH, SKELETON_FRAME_HEIGHT };
	
	/* Invisible: render semi-transparent */
	if (e->is_invisible) draw_color = Fade(draw_color, 0.35f);
	
	if (tex.id != 0) {
		DrawTexturePro(tex, e->frame_rec, dest_rec, origin, 0.0f, draw_color);
	} else {
		/* Fallback shape when texture is missing */
		Color shape = draw_color;
		switch (e->enemy_type) {
			case ENEMY_WOLF:          shape = (Color){180,180,180,255}; break;
			case ENEMY_BOAR:          shape = (Color){120, 80, 40,255}; break;
			case ENEMY_SABERTOOTH:    shape = (Color){220,160, 60,255}; break;
			case ENEMY_MAMMOTH:       shape = (Color){150,130,110,255}; break;
			case ENEMY_RAT:           shape = (Color){160,140,120,200}; break;
			case ENEMY_SLIME:         shape = (Color){ 80,200, 80,255}; break;
			case ENEMY_SANDSTORM:     shape = (Color){220,180, 80,200}; break;
			case ENEMY_SEAWAVE:       shape = (Color){ 40,120,220,200}; break;
			case ENEMY_RADIATION:     shape = (Color){ 80,220, 80,200}; break;
			default:                  shape = (Color){200, 60, 60,255}; break;
		}
		if (e->is_invisible) shape = Fade(shape, 0.35f);
		DrawCircle((int)e->x, (int)e->y, 12, shape);
	}
	
	/* HP bar (hidden when invisible)*/
	if (!e->is_invisible) {
		bw      = ENEMY_HP_BAR_WIDTH;
		cur_bar = (e->max_hp > 0) ? (int)((float)e->hp / e->max_hp * bw) : 0;
		if (cur_bar < 0) cur_bar = 0;
		DrawRectangle((int)e->x-15, (int)e->y+20, bw,      4, (Color){100,0,0,255});
		DrawRectangle((int)e->x-15, (int)e->y+20, cur_bar, 4, (Color){0,200,0,255});
	}
	
	/* Status indicators */
	if (e->stun_timer > 0.0f)
		DrawText("Zzz",  (int)e->x-12, (int)e->y-35, 14, YELLOW);
	if (e->cause_mark_active)
		DrawText("FATE", (int)e->x-14, (int)e->y-48, 10, PURPLE);
	if (e->enemy_type == ENEMY_SLIME && e->explode_armed)
		DrawText(TextFormat("%.0f!", e->explode_timer),
				 (int)e->x-10, (int)e->y-36, 12, ORANGE);
	if (e->enemy_type == ENEMY_RADIATION && e->radiation_pulse_cd <= 0.0f)
		DrawCircleLines((int)e->x, (int)e->y, 40.0f, Fade(GREEN, 0.6f));
}

/* ============================================================
* API helpers
* ============================================================ */
void enemy_take_damage(EnemyState* e, int damage) {
	if (e->cause_mark_active) e->cause_damage_accum += damage;
	e->hp            -= damage;
	e->hit_flash_timer = 0.1f;
	
	/* Taking damage resets wolf stealth */
	if (e->enemy_type == ENEMY_WOLF) {
		e->no_attack_timer = 0.0f;
		if (e->is_invisible) {
			e->is_invisible  = 0;
			e->current_speed = e->base_speed;
		}
	}
	/* First hit arms the slime self-destruct timer */
	if (e->enemy_type == ENEMY_SLIME && !e->explode_armed) {
		e->explode_armed = 1;
		e->explode_timer = SLIME_EXPLODE_TIME;
	}
}

int  enemy_get_waypoint_index(const EnemyState* e) { return e->target_waypoint_index; }

void enemy_apply_stun(EnemyState* e, float dur) {
	if (dur > e->stun_timer) e->stun_timer = dur;
}

void enemy_apply_slow(EnemyState* e, float dur, float ratio) {
	e->slow_timer = dur;
	e->slow_ratio = ratio;
}

void enemy_apply_dot(EnemyState* e, float dur, int dmg_tick) {
	e->dot_timer      = dur;
	e->dot_tick_timer = 1.0f;
	e->dot_damage     = dmg_tick;
}

void enemy_reveal(EnemyState* e) {
	if (!e->is_naturally_invisible)
		e->is_invisible = 0;
	if (e->enemy_type == ENEMY_WOLF) {
		e->no_attack_timer = 0.0f;
		e->current_speed   = e->base_speed;
	}
}
