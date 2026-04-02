#include "tower_internal.h"
#include <math.h>

void tower_draw(const Tower* t, Texture2D* tower_textures) {
	Rectangle dest_rec = {(float)(t->x-16),(float)(t->y-16),32.0f,32.0f};
	Rectangle src_rec  = {(float)(t->current_frame*32),0.0f,32.0f,32.0f};
	Vector2   origin   = {0.0f,0.0f};
	
	/* ── 阵营地面颜色圈（塔贴图下方）── */
	if (!t->is_bonfire) {
		Color ground_col;
		int   draw_ground = 1;
		switch (t->color_state) {
			case TOWER_COLOR_BLUE:   ground_col=(Color){ 60,100,220,180}; break;
			case TOWER_COLOR_GREEN:  ground_col=(Color){ 50,200, 80,180}; break;
			case TOWER_COLOR_PURPLE: ground_col=(Color){140, 40,200,180}; break;
			case TOWER_COLOR_RED:    ground_col=(Color){200, 40, 40,180}; break;
		default:
			if (t->faction==FACTION_NOBLE)
				ground_col=(Color){220,180, 40,140};
			else if (t->faction==FACTION_PEOPLE && t->color_state != TOWER_COLOR_NORMAL)
				ground_col=(Color){ 80,160, 80,100};
			else draw_ground=0;
			break;
		}
		if (draw_ground)
			DrawCircle(t->x, t->y+14, 14, ground_col);
	}
	
	/* Bonfire: special appearance (no texture) */
	if (t->is_bonfire) {
		float t_val = (float)GetTime();
		int pulse = (int)(sinf(t_val*8.0f)*4);
		DrawCircle(t->x, t->y, 10+pulse, (Color){220,100, 20,255});
		DrawCircle(t->x, t->y,  6+pulse, (Color){255,200, 40,255});
		DrawCircle(t->x, t->y,  3,       (Color){255,255,200,255});
		DrawText("Bonfire", t->x-18, t->y-22, 9, ORANGE);
		return;
	}
	
	/* ── 贴图绘制 ── */
	if (t->type>=TOWER_SLING && t->type<=TOWER_LASER) {
		Texture2D tx = tower_textures[t->type];
		Color tint   = WHITE;
		
		if (t->is_betrayed)                                tint = (Color){180, 60,220,255};
		else if (t->is_buried)                             tint = Fade(DARKBROWN,0.6f);
		else if (t->is_frozen)                             tint = Fade(SKYBLUE,0.8f);
		else if (t->type==TOWER_BONESPEAR&&t->ammo<=0)     tint = Fade(DARKGRAY,0.45f);
		else if (t->type==TOWER_BONESPEAR&&t->ammo>0&&t->state==0) tint=Fade(WHITE,0.55f);
		else if (t->type==TOWER_MINE&&t->ammo<=0)          tint = Fade(DARKGRAY,0.50f);
		else if (t->type==TOWER_PORTAL&&t->state==0)       tint = Fade(WHITE,0.5f);
		else if (t->type==TOWER_FIRE&&t->state==0)         tint = Fade(WHITE,0.35f);
		
		DrawTexturePro(tx, src_rec, dest_rec, origin, 0.0f, tint);
	}
	
	/* ── 冰封覆盖层 ── */
	if (t->is_frozen) {
		DrawRectangle(t->x-16, t->y-16, 32, 32, (Color){100,200,255,80});
		DrawRectangleLines(t->x-16, t->y-16, 32, 32, (Color){150,220,255,200});
		DrawText(TextFormat("%.0f", t->frozen_timer), t->x-6, t->y-24, 10, SKYBLUE);
	}
	
	/* ── 掩埋覆盖层 ── */
	if (t->is_buried) {
		DrawRectangle(t->x-16, t->y-8, 32, 24, (Color){150,110,60,200});
		DrawText("Buried", t->x-16, t->y-22, 9, (Color){180,140,80,255});
		return; /* 掩埋后不显示状态 */
	}
	
	/* ── 叛变标记 ── */
	if (t->is_betrayed) {
		DrawText("BETRAYED", t->x-22, t->y-24, 9, (Color){200,60,255,255});
		return;
	}
	
	/* ── 升级标签 ── */
	if (t->level_a>0) DrawText("A", t->x+10, t->y-20, 10, GREEN);
	if (t->level_b>0) DrawText("B", t->x+18, t->y-20, 10, SKYBLUE);
	
	/* ── 阵营标签 ── */
	if (t->faction==FACTION_PEOPLE)
		DrawText("P", t->x-18, t->y-20, 9, (Color){ 80,200, 80,220});
	else if (t->faction==FACTION_NOBLE)
		DrawText("N", t->x-18, t->y-20, 9, (Color){220,180, 40,220});
	
	/* ── 各塔专属状态（与原版相同）── */
	if (t->type==TOWER_SLING&&t->ammo<=0)
		DrawText("Empty",t->x-15,t->y-22,10,RED);
	if (t->type==TOWER_BONESPEAR&&t->ammo<=0)
		DrawText("Rebuild",t->x-16,t->y-22,10,(Color){200,150,100,255});
	if (t->type==TOWER_FIRE&&t->state==0)
		DrawText("No Wood",t->x-18,t->y-22,10,GRAY);
	if (t->type==TOWER_CROSSBOW&&t->state==0) {
		float prog=(t->timer1>0.0f)?1.0f-(t->timer1/3.0f):1.0f;
		DrawRectangle(t->x-12,t->y+16,24,3,DARKGRAY);
		DrawRectangle(t->x-12,t->y+16,(int)(24.0f*prog),3,GOLD);
	}
	if (t->type==TOWER_MINE) {
		float blast_r = (t->level_a >= 1) ? t->range * 1.7f : (float)t->range;
		
		/* ── 1. 待机状态：绿色脉冲圈，表示地雷已就绪 ── */
		if (t->ammo > 0 && t->timer1 <= 0.0f) {
			float pulse = (float)fmod(GetTime() * 1.8, 1.0);
			DrawCircleLines(t->x, t->y, (float)t->range * (0.6f + 0.4f * pulse),
							Fade((Color){80,220,80,255}, 0.5f * (1.0f - pulse)));
		}
		
		/* ── 2. 爆炸光效（timer1 > 0）：主爆橙色冲击波，二爆红色余波 ──
		timer1 由 mine_explode 设置：主爆 0.45s，二爆 0.30s
		用剩余比例驱动扩散半径和透明度，让大圈快速扩散消失 */
		if (t->timer1 > 0.0f) {
			/* 主爆光效基准时长 0.45s，二爆 0.30s；统一用 0.45 归一，
			timer1 > 0.30 说明刚触发主爆，否则可能是二爆余波 */
			float max_t  = (t->timer1 > 0.30f) ? 0.45f : 0.30f;
			float ratio  = t->timer1 / max_t;          /* 1→0 随时间递减 */
			float expand = 1.0f - ratio;               /* 0→1 扩散进度   */
			
			/* 外圈：快速扩散到 blast_r，橙色 */
			float outer_r = blast_r * expand;
			DrawCircle(t->x, t->y, outer_r,
					   Fade((Color){255,160,30,255}, ratio * 0.35f));
			DrawCircleLines(t->x, t->y, outer_r,
							Fade((Color){255,200,60,255}, ratio * 0.9f));
			
			/* 内圈：扩散到 blast_r*0.55，白热核心，消散更快 */
			float inner_r = blast_r * 0.55f * expand;
			DrawCircle(t->x, t->y, inner_r,
					   Fade((Color){255,240,200,255}, ratio * 0.6f));
			
			/* A2 二次爆炸倒计时中（timer2 > 0）：显示红色预警圈 */
			if (t->timer2 > 0.0f) {
				float warn = 1.0f - (t->timer2 / 0.30f);   /* 0→1 */
				DrawCircleLines(t->x, t->y, blast_r * warn,
								Fade((Color){255,60,60,255}, t->timer2 / 0.30f));
				DrawText("2nd!", t->x - 12, t->y - 26, 9,
						 (Color){255,100,100,255});
			}
		}
		
		/* ── 3. A2 二次爆炸独立倒计时（主爆光效已消散后仍可能在等待）── */
		if (t->timer1 <= 0.0f && t->timer2 > 0.0f) {
			float warn = 1.0f - (t->timer2 / 0.30f);
			DrawCircleLines(t->x, t->y, blast_r * warn,
							Fade((Color){255,60,60,255}, t->timer2 / 0.30f));
			DrawText("2nd!", t->x - 12, t->y - 26, 9,
					 (Color){255,100,100,255});
		}
		
		/* ── 4. 已引爆（ammo==0）的贴图变暗 ── */
		/* 贴图变暗已在上方 tint 段处理，此处仅补充文字/进度条 */
		
		/* ── 5. B1 填充进度条（橙色） ── */
		if (t->ammo <= 0 && t->level_b >= 1 && t->reload_timer > 0.0f) {
			float prog = t->reload_timer / 10.0f;
			DrawRectangle(t->x-12, t->y+16, 24, 3, DARKGRAY);
			DrawRectangle(t->x-12, t->y+16, (int)(24.0f*prog), 3, ORANGE);
			/* 剩余秒数标注 */
			DrawText(TextFormat("%.0fs", 10.0f - t->reload_timer),
					 t->x - 8, t->y - 24, 9, ORANGE);
		}
		
		/* ── 6. 彻底耗尽（无B升级，永久空）── */
		if (t->ammo <= 0 && t->level_b == 0 && t->timer1 <= 0.0f && t->timer2 <= 0.0f)
			DrawText("Empty", t->x-14, t->y-24, 9, (Color){160,160,160,255});
	}
	if (t->type==TOWER_CAULDRON&&t->level_b>=1)
		DrawText(TextFormat("%d",t->explosive_charges),t->x-4,t->y-22,12,ORANGE);
	if (t->type==TOWER_FIRECROW&&t->crow_current_cd>0.0f) {
		float prog=1.0f-(t->crow_current_cd/t->crow_cooldown);
		DrawRectangle(t->x-12,t->y+16,24,3,DARKGRAY);
		DrawRectangle(t->x-12,t->y+16,(int)(24.0f*prog),3,ORANGE);
	}
	if (t->type==TOWER_PULSE&&t->current_cooldown>0.0f) {
		float prog=1.0f-(t->current_cooldown/t->cooldown);
		DrawCircleLines(t->x,t->y,8.0f+prog*12.0f,Fade(SKYBLUE,prog));
	}
	if (t->type==TOWER_GRAVITY) {
		float wave=(float)fmod(GetTime()*1.5,1.0);
		DrawCircleLines(t->x,t->y,t->range*wave,Fade(PURPLE,1.0f-wave));
	}
	if (t->type==TOWER_PORTAL) {
		DrawText(t->portal_role==0?"A":"B",t->x-4,t->y-6,12,WHITE);
		if (t->state==0) DrawText("Wait",t->x-14,t->y+20,10,PURPLE);
		if (t->portal_collapse_cd>0.0f)
			DrawText(TextFormat("CD%.0f",t->portal_collapse_cd),t->x-12,t->y-22,9,RED);
	}
	if (t->type==TOWER_TIMECLOCK) {
		if (t->time_wave_timer>0.0f) {
			float prog=1.0f-(t->time_wave_timer/2.0f);
			DrawCircleLines(t->x,t->y,t->range*prog,Fade(SKYBLUE,1.0f-prog));
		}
		if (t->current_cooldown>0.0f) {
			float prog=1.0f-(t->current_cooldown/t->cooldown);
			DrawRectangle(t->x-12,t->y+18,24,3,DARKGRAY);
			DrawRectangle(t->x-12,t->y+18,(int)(24.0f*prog),3,SKYBLUE);
		}
	}
	if (t->type==TOWER_LASER) {
		if (t->laser_beam_timer>0.0f) {
			float inten=t->laser_beam_timer/6.0f;
			DrawCircle(t->x,t->y-10,5.0f+t->laser_beam_timer,Fade(RED,inten));
		}
		/* A path: show charge count and super-pulse CD */
		if (t->level_b==0) {
			int max_charges = (t->level_a>=1) ? 2 : 0;
			if (max_charges>0) {
				/* Charge pips */
				int k;
				for (k=0;k<max_charges;k++) {
					Color pip_col = (k<t->laser_charges) ? (Color){255,200,50,255} : DARKGRAY;
					DrawRectangle(t->x-8+k*9, t->y+14, 7, 5, pip_col);
				}
			}
			/* A2: super-pulse CD bar */
			if (t->level_a>=2 && t->laser_pulse_cd>0.0f) {
				float prog=1.0f-(t->laser_pulse_cd/6.0f);
				DrawRectangle(t->x-12,t->y+20,24,3,DARKGRAY);
				DrawRectangle(t->x-12,t->y+20,(int)(24.0f*prog),3,(Color){255,200,50,255});
				DrawText("PULSE",t->x-12,t->y+24,8,(Color){255,200,50,200});
			}
		}
		/* B path: show per-shot CD bar */
		if (t->level_b>=1 && t->current_cooldown>0.0f) {
			float prog=1.0f-(t->current_cooldown/1.5f);
			if (prog<0.0f) prog=0.0f;
			DrawRectangle(t->x-12,t->y+14,24,3,DARKGRAY);
			DrawRectangle(t->x-12,t->y+14,(int)(24.0f*prog),3,(Color){220,80,255,255});
			/* B2: chain indicator */
			if (t->level_b>=2)
				DrawText("x3",t->x+6,t->y+18,8,(Color){220,80,255,200});
		}
	}
	if (t->buff_timer>0.0f) {
		DrawCircleLines(t->x,t->y-25,4.0f,ORANGE);
		DrawCircleLines(t->x,t->y-25,6.0f,Fade(ORANGE,0.5f));
	}
	
	/* ── 塔血条（Future W2 战斗模式）── */
	if (t->hp > 0) {
		int bw=28, bx=t->x-14, by=t->y+20;
		int fw=(int)(28.0f*(float)t->hp/4000.0f);
		if (fw<0) fw=0;
		DrawRectangle(bx,   by, bw, 4, (Color){80,0,0,200});
		DrawRectangle(bx,   by, fw, 4, (Color){0,200,60,220});
		DrawRectangleLines(bx, by, bw, 4, (Color){120,120,120,180});
	}
}

void tower_draw_laser_beam(const Tower* t,
						   const EnemyState* enemies, int enemy_count) {
	int i;
	if (t->type!=TOWER_LASER||t->target_id<0) return;
	for (i=0;i<enemy_count;i++) {
		if (enemies[i].id!=t->target_id||!enemies[i].active) continue;
		DrawLineEx((Vector2){(float)t->x,(float)(t->y-16)},
				   (Vector2){enemies[i].x,enemies[i].y},
				   3.0f,Fade((Color){220,30,30,255},0.9f));
		DrawLineEx((Vector2){(float)t->x,(float)(t->y-16)},
				   (Vector2){enemies[i].x,enemies[i].y},
				   1.0f,(Color){255,180,180,255});
		break;
	}
}
