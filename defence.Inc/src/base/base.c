/**
 * @file    base.c
 * @brief   基地模块实现（迁移自 prototype/v2/Base.c，已转为 snake_case + float）
 *          Base module implementation (migrated from prototype/v2/Base.c, snake_case + float).
 * @version 1.0
 */

#include "base.h"
#include <math.h>

/* ============================================================
* 内部辅助 | Internal Helpers
* ============================================================ */

/* 生成抛射物（内部辅助）/ Spawn projectile (internal helper) */
static void spawn_proj(VisualProjectile* projs, int* count,
					   float sx, float sy, float tx, float ty,
					   int target_id, int dmg, float life) {
	if (*count >= MAX_PROJECTILES) return;
	VisualProjectile* p = &projs[*count];
	p->start_x  = sx;    p->start_y  = sy;
	p->target_x = tx;    p->target_y = ty;
	p->target_id = target_id;
	p->damage   = dmg;
	p->is_hit   = 1;     p->is_crow  = 0;
	p->life     = life;  p->max_life = life;
	(*count)++;
}

/* 生成飘字（内部辅助）/ Spawn float text (internal helper) */
static void spawn_text(FloatingText* texts, int* count, float x, float y, int dmg) {
	if (*count >= MAX_FLOAT_TEXTS) return;
	FloatingText* t = &texts[*count];
	t->x = x;  t->y = y - 20.0f;
	t->damage = dmg;  t->life = 0.8f;
	(*count)++;
}

/* ============================================================
* base_init
* ============================================================ */
void base_init(Base* b, int x, int y, int era) {
	b->x   = x;    b->y   = y;
	b->era = era;
	b->level_a = 0;  b->level_b = 0;
	b->anim_timer = 0.0f;
	
	b->attack_damage   = 0;    b->attack_range    = 0;
	b->attack_cooldown = 2.0f; b->attack_timer    = 2.0f;
	b->free_build_count = 0;   b->trap_shield     = 0;
	b->invincible      = 0;    b->invincible_timer = 0.0f;
	b->invincible_used = 0;
	b->satellite_cooldown = 2.0f; b->satellite_timer = 2.0f;
	
	b->anim_timer  = 0.0f;
	b->anim_frame  = 0;
	b->frame_timer = 0.0f;
	b->texture     = (Texture2D){0};
	
	switch (era) {
		case 0:  b->max_hp = 10; break;
		case 1:  b->max_hp = 15; break;
		default: b->max_hp = 20; break;
	}
	b->hp = b->max_hp;
}

/* ============================================================
* base_update
* ============================================================ */
void base_update(Base* b, float dt,
				 EnemyState* enemies, int enemy_count,
				 FloatingText* f_texts, int* f_text_count,
				 VisualProjectile* projs, int* proj_count) {
	int i;
	float dx, dy;
	
	b->anim_timer += dt;
	
	/* 精灵帧动画（每0.18秒切一帧，共4帧循环）*/
	b->frame_timer += dt;
	if (b->frame_timer >= 0.18f) {
		b->frame_timer -= 0.18f;
		b->anim_frame = (b->anim_frame + 1) % 4;
	}
	
	/* 无敌计时（未来A1）/ Invincibility timer (Future A1) */
	if (b->invincible) {
		b->invincible_timer -= dt;
		if (b->invincible_timer <= 0.0f) b->invincible = 0;
	}
	
	/* 自带攻击（石器A2 / 古代A1）/ Built-in attack (Stone A2 / Ancient A1) */
	if (b->attack_damage > 0 && b->attack_range > 0) {
		b->attack_timer -= dt;
		if (b->attack_timer <= 0.0f) {
			int best = -1;
			float best_d2 = (float)b->attack_range * b->attack_range + 1.0f;
			for (i = 0; i < enemy_count; i++) {
				if (!enemies[i].active || enemies[i].hp <= 0) continue;
				dx = enemies[i].x - (float)b->x;
				dy = enemies[i].y - (float)b->y;
				float d2 = dx*dx + dy*dy;
				if (d2 <= (float)b->attack_range * b->attack_range && d2 < best_d2) {
					best_d2 = d2;  best = i;
				}
			}
			if (best >= 0) {
				enemy_take_damage(&enemies[best], b->attack_damage);
				spawn_proj(projs, proj_count,
						   (float)b->x, (float)b->y,
						   enemies[best].x, enemies[best].y,
						   enemies[best].id, b->attack_damage, 0.15f);
				spawn_text(f_texts, f_text_count,
						   enemies[best].x, enemies[best].y, b->attack_damage);
			}
			b->attack_timer = b->attack_cooldown;
		}
	}
	
	/* 卫星轨道炮（未来A2）/ Satellite orbital cannon (Future A2) */
	if (b->era == 2 && b->level_a >= 2) {
		b->satellite_timer -= dt;
		if (b->satellite_timer <= 0.0f) {
			b->satellite_timer = b->satellite_cooldown;
			int best = -1;  int best_hp = -1;
			for (i = 0; i < enemy_count; i++) {
				if (!enemies[i].active || enemies[i].hp <= 0) continue;
				if (enemies[i].hp > best_hp) { best_hp = enemies[i].hp;  best = i; }
			}
			if (best >= 0) {
				enemy_take_damage(&enemies[best], 70);
				spawn_text(f_texts, f_text_count, enemies[best].x, enemies[best].y, 70);
				/* 从画面顶部打下来的激光 / Laser from top of screen */
				spawn_proj(projs, proj_count,
						   enemies[best].x, 0.0f,
						   enemies[best].x, enemies[best].y,
						   -1, 0, 0.25f);
			}
		}
	}
}

/* ============================================================
* base_draw
* ============================================================ */
void base_draw(const Base* b) {
	float t = b->anim_timer;
	int pulse = (int)(sinf(t * 8.0f) * 6.0f);
	int k;
	
	/* ── 贴图模式：精灵表 64×64，2×2布局，每帧32×32 ── */
	if (b->texture.id != 0) {
		/* frame 0=左上 1=右上 2=左下 3=右下 */
		int fx = (b->anim_frame % 2) * 32;
		int fy = (b->anim_frame / 2) * 32;
		Rectangle src  = { (float)fx, (float)fy, 32.0f, 32.0f };
		/* 放大到64×64显示，居中于基地坐标 */
		Rectangle dest = { (float)(b->x - 32), (float)(b->y - 32), 64.0f, 64.0f };
		Vector2   orig = { 0.0f, 0.0f };
		
		/* 无敌时加青色叠加 */
		Color tint = (b->invincible) ? (Color){100,255,255,220} : WHITE;
		DrawTexturePro(b->texture, src, dest, orig, 0.0f, tint);
		
		/* 升级特效叠加（保留在贴图上方）*/
		if (b->era == 0 && b->level_a >= 2) {
			for (k = 0; k < 4; k++) {
				float angle = t * 1.5f + k * 3.14159f / 2.0f;
				int ffx = b->x + (int)(cosf(angle) * 36);
				int ffy = b->y + (int)(sinf(angle) * 36);
				DrawCircle(ffx, ffy, 4 + (int)(sinf(t * 10.0f + k) * 2), ORANGE);
			}
		}
		if (b->era == 1 && b->level_a >= 1 && b->free_build_count > 0)
			DrawText(TextFormat("FREE x%d", b->free_build_count), b->x-22, b->y+36, 10, GOLD);
		if (b->era == 1 && b->level_a >= 2 && b->trap_shield > 0) {
			for (k = 0; k < b->trap_shield; k++) {
				float angle = (float)(k * 120.0 * 3.14159 / 180.0);
				int sx = b->x + (int)(cosf(angle) * 36);
				int sy = b->y + (int)(sinf(angle) * 36);
				DrawRectangle(sx-4, sy-4, 8, 8, (Color){200,180,40,220});
				DrawRectangleLines(sx-4, sy-4, 8, 8, GOLD);
			}
		}
		if (b->era == 2 && b->invincible) {
			DrawCircleLines(b->x, b->y, 52.0f, Fade(SKYBLUE, 0.7f));
			DrawCircle(b->x, b->y, 50, Fade(SKYBLUE, 0.15f));
			DrawText(TextFormat("SHIELD %.1fs", b->invincible_timer), b->x-28, b->y-60, 10, SKYBLUE);
		}
		if (b->era == 2 && b->level_a >= 2) {
			float sat_angle = t * 0.8f;
			int sx = b->x + (int)(cosf(sat_angle) * 60);
			int sy = b->y + (int)(sinf(sat_angle) * 60);
			DrawCircleLines(b->x, b->y, 60.0f, Fade(YELLOW, 0.3f));
			DrawCircle(sx, sy, 5, YELLOW);
			DrawCircle(sx, sy, 3, WHITE);
		}
		
		/* HP 血条 */
		int bw = 60, bx = b->x - 30, by = b->y + 36;
		int fw = (b->max_hp > 0) ? (int)((float)b->hp / b->max_hp * bw) : 0;
		if (fw < 0) fw = 0;
		DrawRectangle(bx, by, bw, 6, (Color){80,0,0,200});
		DrawRectangle(bx, by, fw, 6, (Color){0,200,60,220});
		DrawRectangleLines(bx, by, bw, 6, (Color){150,150,150,200});
		DrawText(TextFormat("%d/%d", b->hp, b->max_hp), bx, by+8, 10, LIGHTGRAY);
		return;
	}
	
	/* ── Fallback：程序绘制（无贴图时使用）── */
	if (b->era == 0) {
		DrawCircleLines(b->x, b->y, 38 + pulse/2, (Color){40,120,200,180});
		DrawCircleLines(b->x, b->y, 44, (Color){60,160,240,100});
		DrawCircle(b->x, b->y, 26, (Color){80,50,20,255});
		DrawCircle(b->x, b->y, 20, (Color){110,70,30,255});
		DrawRectangle(b->x - 5, b->y - 18, 10, 20, (Color){150,40,10,255});
		DrawCircle(b->x, b->y - 22, 7, (Color){255,140,0,255});
		DrawCircle(b->x, b->y - 26, 4 + pulse/3, (Color){255,220,50,255});
		if (b->level_a >= 2) {
			for (k = 0; k < 4; k++) {
				float angle = t * 1.5f + k * 3.14159f / 2.0f;
				int fx = b->x + (int)(cosf(angle) * 32);
				int fy = b->y + (int)(sinf(angle) * 32);
				DrawCircle(fx, fy, 4 + (int)(sinf(t * 10.0f + k) * 2), ORANGE);
			}
		}
	} else if (b->era == 1) {
		int wo = 32;
		DrawRectangle(b->x-wo-6, b->y-wo-6, 12, 12, (Color){100,90,70,255});
		DrawRectangle(b->x+wo-6, b->y-wo-6, 12, 12, (Color){100,90,70,255});
		DrawRectangle(b->x-wo-6, b->y+wo-6, 12, 12, (Color){100,90,70,255});
		DrawRectangle(b->x+wo-6, b->y+wo-6, 12, 12, (Color){100,90,70,255});
		DrawLine(b->x-wo, b->y-wo, b->x+wo, b->y-wo, (Color){80,70,55,255});
		DrawLine(b->x-wo, b->y+wo, b->x+wo, b->y+wo, (Color){80,70,55,255});
		DrawLine(b->x-wo, b->y-wo, b->x-wo, b->y+wo, (Color){80,70,55,255});
		DrawLine(b->x+wo, b->y-wo, b->x+wo, b->y+wo, (Color){80,70,55,255});
		DrawRectangle(b->x-18, b->y-14, 36, 28, (Color){140,110,60,255});
		DrawTriangle((Vector2){(float)b->x,(float)(b->y-28)},
					 (Vector2){(float)(b->x-22),(float)(b->y-14)},
					 (Vector2){(float)(b->x+22),(float)(b->y-14)},
					 (Color){180,50,30,255});
		if (b->level_a >= 1) {
			float spin = t * 80.0f;
			for (k = 0; k < 8; k++) {
				float angle = spin + k * 45.0f;
				int gx = b->x + (int)(cosf(angle * 3.14159f/180.0f) * 14);
				int gy = b->y + (int)(sinf(angle * 3.14159f/180.0f) * 14);
				DrawCircle(gx, gy, 3, (Color){220,180,50,255});
			}
			if (b->free_build_count > 0)
				DrawText(TextFormat("FREE x%d", b->free_build_count), b->x-22, b->y+wo+10, 10, GOLD);
		}
		if (b->level_a >= 2 && b->trap_shield > 0) {
			for (k = 0; k < b->trap_shield; k++) {
				float angle = (float)(k * 120.0 * 3.14159 / 180.0);
				int sx = b->x + (int)(cosf(angle) * 28);
				int sy = b->y + (int)(sinf(angle) * 28);
				DrawRectangle(sx-4, sy-4, 8, 8, (Color){200,180,40,220});
				DrawRectangleLines(sx-4, sy-4, 8, 8, GOLD);
			}
		}
	} else {
		Color fc = b->invincible ? (Color){50,255,255,200} : (Color){30,120,255,150};
		DrawCircleLines(b->x, b->y, (float)(44 + pulse/2), fc);
		DrawCircleLines(b->x, b->y, 40.0f, Fade(fc, 0.5f));
		for (k = 0; k < 3; k++) {
			float angle = t * 1.2f + k * 3.14159f * 2.0f / 3.0f;
			int rx = b->x + (int)(cosf(angle) * 34);
			int ry = b->y + (int)(sinf(angle) * 34);
			DrawCircle(rx, ry, 5, (Color){80,200,255,220});
		}
		DrawCircle(b->x, b->y, 20, (Color){10,20,60,255});
		DrawCircle(b->x, b->y, 14, (Color){30,100,220,255});
		DrawCircle(b->x, b->y, 8,  (Color){150,220,255,255});
		if (b->invincible) {
			DrawCircle(b->x, b->y, 44, Fade(SKYBLUE, 0.25f));
			DrawCircleLines(b->x, b->y, 48.0f, SKYBLUE);
			DrawText(TextFormat("SHIELD %.1fs", b->invincible_timer), b->x-28, b->y-60, 10, SKYBLUE);
		}
		if (b->level_a >= 2) {
			float sat_angle = t * 0.8f;
			int sx = b->x + (int)(cosf(sat_angle) * 55);
			int sy = b->y + (int)(sinf(sat_angle) * 55);
			DrawCircleLines(b->x, b->y, 55.0f, Fade(YELLOW, 0.3f));
			DrawCircle(sx, sy, 5, YELLOW);
			DrawCircle(sx, sy, 3, WHITE);
		}
	}
	
	/* HP 血条 / HP bar */
	int bw = 60, bx = b->x - 30, by = b->y + 52;
	int fw = (b->max_hp > 0) ? (int)((float)b->hp / b->max_hp * bw) : 0;
	if (fw < 0) fw = 0;
	DrawRectangle(bx, by, bw, 6, (Color){80,0,0,200});
	DrawRectangle(bx, by, fw, 6, (Color){0,200,60,220});
	DrawRectangleLines(bx, by, bw, 6, (Color){150,150,150,200});
	DrawText(TextFormat("%d/%d", b->hp, b->max_hp), bx, by+8, 10, LIGHTGRAY);
}

/* ============================================================
* base_upgrade
* ============================================================ */
int base_upgrade(Base* b, int path,
				 int* wood, int* stone, int* metal, int* material) {
	(void)stone; /* 石器时代仅用 wood / Stone era only uses wood */
	if (path != 0) return 2;
	
	if (b->era == 0) {
		if (b->level_a == 0) {
			if (*wood < 60) return 1;
			*wood -= 60;  b->level_a = 1;  return 0;
		} else if (b->level_a == 1) {
			if (*wood < 100) return 1;
			*wood -= 100;  b->level_a = 2;
			b->attack_damage = 25;  b->attack_range = 160;
			b->attack_cooldown = 2.0f;  b->attack_timer = 2.0f;
			return 0;
		}
		return 2;
	}
	
	if (b->era == 1) {
		if (b->level_a == 0) {
			if (*metal < 150) return 1;
			*metal -= 150;  b->level_a = 1;
			b->free_build_count = 3;
			b->attack_damage = 50;  b->attack_range = 200;
			b->attack_cooldown = 2.0f;  b->attack_timer = 2.0f;
			return 0;
		} else if (b->level_a == 1) {
			if (*metal < 250) return 1;
			*metal -= 250;  b->level_a = 2;  b->trap_shield = 3;
			return 0;
		}
		return 2;
	}
	
	if (b->era == 2) {
		if (b->level_a == 0) {
			if (*material < 300) return 1;
			*material -= 300;  b->level_a = 1;  b->invincible_used = 0;
			return 0;
		} else if (b->level_a == 1) {
			if (*material < 500) return 1;
			*material -= 500;  b->level_a = 2;
			b->satellite_cooldown = 2.0f;  b->satellite_timer = 2.0f;
			return 0;
		}
		return 2;
	}
	return 2;
}

/* ============================================================
* base_get_upgrade_label
* ============================================================ */
void base_get_upgrade_label(const Base* b, int path,
							const char** out_label, int* out_affordable,
							int wood, int stone, int metal, int material) {
	(void)stone;
	*out_label = "";  *out_affordable = 0;
	if (path != 0) { *out_label = "(N/A)"; return; }
	
	if (b->era == 0) {
		if (b->level_a == 0) { *out_label = "A1:Prod+50% -60W";  *out_affordable = (wood >= 60); }
		else if (b->level_a == 1) { *out_label = "A2:FireAtk25 -100W"; *out_affordable = (wood >= 100); }
		else { *out_label = "A:MAX"; }
	} else if (b->era == 1) {
		if (b->level_a == 0) { *out_label = "A1:FreeBuild x3 -150M"; *out_affordable = (metal >= 150); }
		else if (b->level_a == 1) { *out_label = "A2:TrapShield x3 -250M"; *out_affordable = (metal >= 250); }
		else { *out_label = "A:MAX"; }
	} else if (b->era == 2) {
		if (b->level_a == 0) { *out_label = "A1:LastStand 5s -300"; *out_affordable = (material >= 300); }
		else if (b->level_a == 1) { *out_label = "A2:Satellite 70dmg -500"; *out_affordable = (material >= 500); }
		else { *out_label = "A:MAX"; }
	}
}

/* ============================================================
* 辅助函数 | Helper Functions
* ============================================================ */

int base_consume_free_build(Base* b) {
	if (b->era == 1 && b->level_a >= 1 && b->free_build_count > 0) {
		b->free_build_count--;  return 1;
	}
	return 0;
}

int base_consume_trap_shield(Base* b) {
	if (b->era == 1 && b->level_a >= 2 && b->trap_shield > 0) {
		b->trap_shield--;  return 1;
	}
	return 0;
}

int base_try_invincible(Base* b) {
	if (b->era == 2 && b->level_a >= 1 && !b->invincible_used && b->hp <= 1) {
		b->invincible       = 1;
		b->invincible_timer = 5.0f;
		b->invincible_used  = 1;
		return 1;
	}
	return 0;
}

/* ============================================================
* base_set_texture
* ============================================================ */
void base_set_texture(Base* b, Texture2D tex) {
	b->texture = tex;
}
