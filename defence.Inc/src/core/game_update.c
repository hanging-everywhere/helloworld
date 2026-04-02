#include "game.h"
#include "scale.h"
#include "../ui/menu.h"
#include "../ui/settings.h"
#include <math.h>
#include <stddef.h>  /* NULL */

/* 前向声明 */
EnemyType game_get_spawn_type(int level, int wave, int spawn_index);

/* ============================================================
* 内部：波次事件处理
* ============================================================ */

/* Lightning: randomly destroy one normal tower, turn it into a bonfire */
static void do_lightning(Game* g) {
	int count=0, candidates[MAX_TOWERS], i;
	for (i=0;i<g->tower_count;i++) {
		if (!g->towers[i].is_bonfire && !g->towers[i].is_betrayed &&
			g->towers[i].type!=TOWER_NONE)
			candidates[count++]=i;
	}
	if (count==0) return;
	int idx=candidates[GetRandomValue(0,count-1)];
	g->towers[idx].is_bonfire=1;
	
	/* Record target position for lightning visual */
	g->lightning_active   = 1;
	g->lightning_timer    = 0.6f;
	g->lightning_target_x = (float)g->towers[idx].x;
	g->lightning_target_y = (float)g->towers[idx].y;
	
	/* Three-line fullscreen narrative, one line per Space press */
	narr_push_fullscreen(&g->narr, (Color){255,220,80,255},
						 "A bolt of lightning strikes from the sky.",
						 "A tower has been destroyed — it becomes a bonfire.",
						 "The people are terrified.",
						 "All towers: -20% fire rate.",
						 NULL);
}

/* 恐惧减益：全塔攻速-20%（通过 buff_speed_ratio 实现）*/
static void apply_fear(Game* g, int active) {
	int i;
	g->fear_active=active;
	for (i=0;i<g->tower_count;i++) {
		if (active) {
			if (g->towers[i].buff_speed_ratio>0.8f)
				g->towers[i].buff_speed_ratio=0.8f;
		} else {
			/* 恐惧解除时恢复（若没有图腾buff就重置为1）*/
			if (g->towers[i].buff_timer<=0.0f)
				g->towers[i].buff_speed_ratio=1.0f;
		}
	}
}

/* Stone W3: freeze all towers and extinguish half the bonfires */
static void do_freeze_all_towers(Game* g, float duration) {
	int i;
	for (i=0;i<g->tower_count;i++) {
		if (!g->towers[i].is_bonfire && !g->towers[i].is_betrayed) {
			g->towers[i].is_frozen    = 1;
			g->towers[i].frozen_timer = duration;
		}
	}
	narr_push_fullscreen(&g->narr, (Color){180,220,255,255},
						 "The Ice Age has arrived.",
						 "All towers are frozen for 10 seconds.",
						 "Humanity faces an unprecedented trial.",
						 "Half the fires have been extinguished.",
						 "Survive.",
						 NULL);
}

/* 农民收服 */
static void do_peasant_convert(Game* g) {
	int i, row, col;
	/* Collect all buildable cells */
	typedef struct { int r; int c; } RC;
	RC cells[MAP_ROWS*MAP_COLS]; int cell_n=0;
	for (row=0;row<MAP_ROWS;row++)
		for (col=0;col<MAP_COLS;col++)
			if (map_can_place_tower(&g->game_map,row,col))
			{ cells[cell_n].r=row; cells[cell_n].c=col; cell_n++; }
	/* Fisher-Yates shuffle */
	for (i=cell_n-1;i>0;i--) {
		int j=GetRandomValue(0,i);
		RC tmp=cells[i]; cells[i]=cells[j]; cells[j]=tmp;
	}
	/* Place 4 people crossbow towers on random buildable cells */
	int placed=0;
	for (i=0;i<cell_n&&placed<4&&g->tower_count<MAX_TOWERS;i++) {
		int r=cells[i].r, c=cells[i].c;
		if (!map_can_place_tower(&g->game_map,r,c)) continue;
		Point center=map_get_center(r,c);
		tower_init(&g->towers[g->tower_count],center.x,center.y,
				   (int)ERA_ANCIENT,TOWER_CROSSBOW);
		g->towers[g->tower_count].faction=FACTION_PEOPLE;
		g->towers[g->tower_count].damage_mult=1.1f;
		g->towers[g->tower_count].buff_speed_ratio=1.2f;
		g->towers[g->tower_count].buff_timer=999.0f;
		map_place_tower(&g->game_map,r,c);
		g->tower_count++; placed++;
	}
	/* Enable 70%% resource bonus */
	g->peasant_bonus_active = 1;
	/* Unlock 40 enemies to spawn */
	g->enemies_to_spawn = 40;
	g->spawn_timer      = g->spawn_interval;
	g->peasant_intro_done = 1;
	audio_play(&g->audio,"upgrade");
}

/* 宗教事件第一阶段：随机3个人民塔变蓝，1个变绿 */
static void do_religion_phase1(Game* g) {
	int i, people_indices[MAX_TOWERS], people_count=0;
	for (i=0;i<g->tower_count;i++) {
		if (g->towers[i].faction==FACTION_PEOPLE) people_indices[people_count++]=i;
	}
	if (people_count==0) return;
	
	/* Assign in repeating groups of 4: positions 0,1,2=blue; 3=green */
	for (i=0;i<people_count;i++) {
		int pos = i % 4;
		g->towers[people_indices[i]].color_state = (pos==3) ? TOWER_COLOR_GREEN : TOWER_COLOR_BLUE;
	}
	
	/* Resources -30% */
	g->metal=(int)(g->metal*0.7f);
	g->wood =(int)(g->wood *0.7f);
}

/* Cult boss spawn: ALL green towers betray, each spawns a boss from path start */
static void do_cult_spawn(Game* g) {
	int i, j, tmp;
	int green_list[MAX_TOWERS], green_n = 0;
	for (i=0;i<g->tower_count;i++)
		if (g->towers[i].color_state == TOWER_COLOR_GREEN)
			green_list[green_n++] = i;
	
	/* Sort descending for safe swap-remove */
	for (i=0;i<green_n-1;i++)
		for (j=i+1;j<green_n;j++)
			if (green_list[j]>green_list[i]){ tmp=green_list[i]; green_list[i]=green_list[j]; green_list[j]=tmp; }
	
	/* Queue bosses to be released by the normal spawn loop */
	g->cult_bosses_to_spawn = green_n;
	for (i=0;i<green_n;i++)
		g->cult_boss_types[i] = g->towers[green_list[i]].type;
	
	/* Remove green towers */
	for (i=0;i<green_n;i++) {
		int ti = green_list[i];
		int col = g->towers[ti].x/CELL_SIZE, row = g->towers[ti].y/CELL_SIZE;
		map_restore_cell(&g->game_map, row, col);
		g->towers[ti] = g->towers[g->tower_count-1];
		g->tower_count--;
	}
}
/* 鼠疫减益 */
static void do_plague_start(Game* g) {
	g->plague_active=1;
	narr_push_fullscreen(&g->narr,(Color){180,255,180,255},
						 "The plague spreads.",
						 "Resources slow. Afflicted towers lose damage and fire rate.",
						 "In the face of death, people abandon their faith in the gods.",
						 "What the gods withheld, the furnace provided.",
						 "The Cauldron can cleanse towers afflicted by the plague.",
						 "The Mine reveals invisible enemies.",
						 NULL);
}

/* 革命：全塔变红，攻速+50%，资源收入×2 */
static void do_revolution(Game* g) {
	int i;
	g->revolution_active=1;
	for (i=0;i<g->tower_count;i++) {
		g->towers[i].color_state=TOWER_COLOR_RED;
		g->towers[i].buff_speed_ratio=1.5f;
		g->towers[i].buff_timer=999.0f;
	}
	/* 完整革命四行宣言 */
	narr_push_fullscreen(&g->narr,(Color){255,60,60,255},
						 "\"When plague came, the nobles hid. We died below. Why?\"",
						 "\"The preacher betrayed us. The nobles called it faith. We paid. Why?\"",
						 "\"We saved the city. But whose city is it?\"",
						 "The people have risen!",
						 NULL);
	/* 结语在 W4 全部敌人消灭后由 game_update 推送 */
}

/* AI betrayal: mark up to 5 most recently built laser towers as betrayed */
static void do_ai_betray(Game* g) {
	int i, betrayed=0;
	/* AI bonus: +3000 material before betrayal */
	g->material += 3000;
	/* Search from tail: most recently built towers are at the back */
	for (i=g->tower_count-1;i>=0&&betrayed<5;i--) {
		if (g->towers[i].type==TOWER_LASER && !g->towers[i].is_betrayed) {
			g->towers[i].is_betrayed=1;
			betrayed++;
		}
	}
	narr_push_fullscreen(&g->narr,(Color){180,80,255,255},
						 "AI has optimised the attack sequence...",
						 "AI has predicted the next wave's path...",
						 "In 10 seconds — AI betrayal! Laser towers turn purple and stop serving you!",
						 NULL);
}

/* 沙暴：随机掩埋2座塔 */
static void do_sandstorm_bury(Game* g) {
	int i, buried=0;
	if (g->tower_count==0) return;
	/* Collect eligible towers (exclude portal, bonfire, betrayed, already buried) */
	int eligible[MAX_TOWERS], elig_n=0;
	for (i=0;i<g->tower_count;i++) {
		if (g->towers[i].is_buried || g->towers[i].is_bonfire ||
			g->towers[i].is_betrayed || g->towers[i].type==TOWER_PORTAL) continue;
		eligible[elig_n++]=i;
	}
	/* Fisher-Yates shuffle, take first 2 */
	for (i=elig_n-1;i>0;i--) {
		int r=GetRandomValue(0,i);
		int tmp=eligible[i]; eligible[i]=eligible[r]; eligible[r]=tmp;
	}
	for (i=0;i<elig_n&&buried<2;i++) {
		g->towers[eligible[i]].is_buried=1;
		buried++;
	}
}

/* ── 未来时期 W1-W3 分阶段出怪 ──
* phase 1 : 15 sea waves (interval 2s)
* phase 2 : sandstorm batch 1 (5), then bury -> phase 3
* phase 3 : wait 8s -> phase 4
* phase 4 : sandstorm batch 2 (5), then bury -> phase 5
* phase 5 : wait 8s -> phase 6
* phase 6 : sandstorm batch 3 (5), then bury -> phase 7
* phase 7 : wait 8s -> phase 8
* phase 8 : 15 radiation (interval 3s) -> phase 9
* phase 9 : wait for all enemies cleared -> wave ends
*/
static void update_future_phase_spawn(Game* g, float dt) {
	if (g->current_state!=GAME_STATE_PLAYING_L3) return;
	if (g->current_wave<1||g->current_wave>3) return;
	if (g->future_phase==0) return;
	
	switch (g->future_phase) {
		
		case -1: /* W1: wait for opening narrative to finish */
		if (!g->narr.active) {
			g->future_phase       = 1;
			g->future_phase_timer = 2.0f;
		}
		break;
		
		case 1: /* 15 sea waves */
		g->future_phase_timer-=dt;
		if (g->future_phase_timer<=0.0f && g->enemy_count<MAX_ENEMIES) {
			int id=1000+g->current_wave*100+g->future_phase_spawned;
			enemy_init(&g->enemies[g->enemy_count],
					   g->game_map.waypoints[0], id, ENEMY_SEAWAVE);
			g->enemy_count++;
			g->future_phase_spawned++;
			g->future_phase_timer=2.0f;
		}
		if (g->future_phase_spawned>=15) {
			g->future_phase=2;
			g->future_phase_spawned=0;
			g->future_phase_timer=6.0f;
			/* W1 only: warn player before first sandstorm batch */
			if (g->current_wave==1) {
				narr_push_fullscreen(&g->narr, (Color){220,180,80,255},
									 "Towers will be buried by the sandstorm.",
									 "Press [C] and spend resources to clear them.",
									 NULL);
			}
		}
		break;
		
		case 2: case 4: case 6: /* sandstorm batch (5 each) */
		g->future_phase_timer-=dt;
		if (g->future_phase_timer<=0.0f && g->enemy_count<MAX_ENEMIES) {
			int batch_off=(g->future_phase==2)?0:(g->future_phase==4)?5:10;
			int id=2000+g->current_wave*100+batch_off+g->future_phase_spawned;
			enemy_init(&g->enemies[g->enemy_count],
					   g->game_map.waypoints[0], id, ENEMY_SANDSTORM);
			g->enemy_count++;
			g->future_phase_spawned++;
			g->future_phase_timer=1.5f;
		}
		if (g->future_phase_spawned>=5) {
			do_sandstorm_bury(g);
			g->future_phase++;
			g->future_phase_spawned=0;
			g->future_phase_timer=8.0f;
		}
		break;
		
		case 3: case 5: case 7: /* wait between batches */
		g->future_phase_timer-=dt;
		if (g->future_phase_timer<=0.0f) {
			g->future_phase++;
			g->future_phase_spawned=0;
			g->future_phase_timer=1.5f;
			/* W1 only: warn player before radiation when entering phase 8 */
			if (g->future_phase==8 && g->current_wave==1) {
				narr_push_fullscreen(&g->narr, (Color){80,220,80,255},
									 "Nuclear radiation can paralyse your towers.",
									 "The Pulse tower can cleanse the contamination.",
									 NULL);
			}
		}
		break;
		
		case 8: /* 15 radiation */
		g->future_phase_timer-=dt;
		if (g->future_phase_timer<=0.0f && g->enemy_count<MAX_ENEMIES) {
			int id=3000+g->current_wave*100+g->future_phase_spawned;
			enemy_init(&g->enemies[g->enemy_count],
					   g->game_map.waypoints[0], id, ENEMY_RADIATION);
			g->enemy_count++;
			g->future_phase_spawned++;
			g->future_phase_timer=3.0f;
		}
		if (g->future_phase_spawned>=15) {
			g->future_phase=9;
		}
		break;
		
		case 9: /* wait for all enemies cleared */
		if (g->enemy_count==0) {
			g->future_phase=0;
			g->is_wave_active=0;
			g->wave_delay_timer=WAVE_DELAY_SEC;
		}
		break;
	}
}

/* ── Future W2: AI Betrayal Phase System ──
* phase 1: narr1 playing -> when done, build 5 AI laser towers + set all tower hp=4000
* phase 2: narr2 playing -> when done, spawn AI towers as enemies, start battle
* phase 3: battle — AI enemies attack player towers; wave ends when all AI enemies dead
*/
static void update_ai_betray_phase(Game* g, float dt) {
	int i, j;
	float dx, dy;
	if (g->current_state != GAME_STATE_PLAYING_L3) return;
	if (g->current_wave != 2) return;
	if (g->ai_betray_phase == 0) return;
	
	/* Phase 1: wait for narr1 to finish, then build 5 AI laser towers */
	if (g->ai_betray_phase == 1) {
		if (!g->narr.active) {
			/* Set all existing towers hp = 4000 */
			for (i = 0; i < g->tower_count; i++)
				g->towers[i].hp = 4000;
			
			/* Build 5 AI laser towers on random buildable cells */
			int built = 0, row, col;
			int tries = 0;
			while (built < 5 && tries < 500) {
				tries++;
				row = GetRandomValue(0, MAP_ROWS-1);
				col = GetRandomValue(0, MAP_COLS-1);
				if (!map_can_place_tower(&g->game_map, row, col)) continue;
				Point center = map_get_center(row, col);
				tower_init(&g->towers[g->tower_count], center.x, center.y,
						   (int)ERA_FUTURE, TOWER_LASER);
				g->towers[g->tower_count].hp           = 4000;
				g->towers[g->tower_count].is_ai_tower  = 1;
				g->towers[g->tower_count].is_betrayed  = 1; /* renders purple */
				g->towers[g->tower_count].color_state  = TOWER_COLOR_GREEN;
				map_place_tower(&g->game_map, row, col);
				g->tower_count++;
				built++;
			}
			
			/* Push second narrative */
			narr_push_fullscreen(&g->narr, (Color){180,80,255,255},
								 "AI attempts to challenge humanity.",
								 NULL);
			g->ai_betray_phase = 2;
		}
		return;
	}
	
	/* Phase 2: wait for narr2 to finish, then spawn AI towers as enemies one by one */
	if (g->ai_betray_phase == 2) {
		if (!g->narr.active) {
			/* First entry: collect AI towers into spawn queue and remove from map */
			if (g->future_phase_spawned == 0 && g->ai_laser_cd == 0.0f) {
				/* Remove all AI towers from map, store count */
				for (i = g->tower_count-1; i >= 0; i--) {
					if (!g->towers[i].is_ai_tower) continue;
					int col2 = g->towers[i].x / CELL_SIZE;
					int row2 = g->towers[i].y / CELL_SIZE;
					map_restore_cell(&g->game_map, row2, col2);
					g->towers[i] = g->towers[g->tower_count-1];
					g->tower_count--;
				}
				g->future_phase_spawned = 5; /* 5 AI towers to spawn */
				g->ai_laser_cd = 0.5f;       /* first spawn after 0.5s */
				g->is_wave_active = 1;
			}
			/* Spawn one AI enemy per interval */
			if (g->future_phase_spawned > 0) {
				g->ai_laser_cd -= dt;
				if (g->ai_laser_cd <= 0.0f && g->enemy_count < MAX_ENEMIES) {
					Point spawn = g->game_map.waypoints[0];
					enemy_init(&g->enemies[g->enemy_count], spawn,
							   5000 + g->enemy_count, ENEMY_CULT_BOSS);
					g->enemies[g->enemy_count].hp              = 4000;
					g->enemies[g->enemy_count].max_hp          = 4000;
					g->enemies[g->enemy_count].cult_tower_type = TOWER_LASER;
					g->enemy_count++;
					g->future_phase_spawned--;
					g->ai_laser_cd = (g->future_phase_spawned > 0) ? 1.2f : 0.0f;
				}
			}
			/* All spawned: enter battle phase */
			if (g->future_phase_spawned == 0 && g->ai_laser_cd == 0.0f &&
				g->enemy_count > 0) {
				g->ai_betray_phase      = 3;
				g->ai_laser_cd          = 2.0f;
				g->future_phase_spawned = 0;
			}
		}
		return;
	}
	
	/* Phase 3: battle — AI laser enemies attack player towers */
	if (g->ai_betray_phase == 3) {
		g->ai_laser_cd -= dt;
		if (g->ai_laser_cd <= 0.0f) {
			g->ai_laser_cd = 2.0f; /* AI fires every 2 seconds */
			/* Each active CULT_BOSS attacks nearest non-AI tower */
			for (i = 0; i < g->enemy_count; i++) {
				if (!g->enemies[i].active) continue;
				if (g->enemies[i].enemy_type != ENEMY_CULT_BOSS) continue;
				/* Find nearest player tower */
				int nearest = -1;
				float best_d2 = 700.0f * 700.0f; /* laser range */
				for (j = 0; j < g->tower_count; j++) {
					if (g->towers[j].is_ai_tower) continue;
					dx = g->enemies[i].x - (float)g->towers[j].x;
					dy = g->enemies[i].y - (float)g->towers[j].y;
					float d2 = dx*dx + dy*dy;
					if (d2 < best_d2) { best_d2 = d2; nearest = j; }
				}
				if (nearest >= 0) {
					g->towers[nearest].hp -= 150; /* laser base damage */
					/* Visual: float text on tower */
					if (g->float_text_count < MAX_FLOAT_TEXTS) {
						g->float_texts[g->float_text_count].x = (float)g->towers[nearest].x;
						g->float_texts[g->float_text_count].y = (float)g->towers[nearest].y - 20.0f;
						g->float_texts[g->float_text_count].damage = 150;
						g->float_texts[g->float_text_count].life   = 0.8f;
						g->float_text_count++;
					}
				}
			}
		}
		
		/* Destroy player towers with hp <= 0 (swap-remove, descending) */
		for (i = g->tower_count-1; i >= 0; i--) {
			if (g->towers[i].is_ai_tower) continue;
			if (g->towers[i].hp > 0) continue; /* still alive */
			/* hp <= 0 and in HP mode (W2 battle): destroy */
			int col2 = g->towers[i].x / CELL_SIZE;
			int row2 = g->towers[i].y / CELL_SIZE;
			map_restore_cell(&g->game_map, row2, col2);
			g->towers[i] = g->towers[g->tower_count-1];
			g->tower_count--;
		}
		
		/* Wave ends when all CULT_BOSS enemies are dead */
		int ai_alive = 0;
		for (i = 0; i < g->enemy_count; i++) {
			if (g->enemies[i].active &&
				g->enemies[i].enemy_type == ENEMY_CULT_BOSS) {
				ai_alive = 1; break;
			}
		}
		if (!ai_alive && g->enemy_count == 0) {
			/* Reset tower hp to 0 (exit HP mode) */
			for (i = 0; i < g->tower_count; i++)
				g->towers[i].hp = 0;
			g->ai_betray_phase = 0;
			g->is_wave_active  = 0;
			g->wave_delay_timer = WAVE_DELAY_SEC;
		}
	}
}

static void process_wave_events(Game* g, float dt) {
	int i;
	g->wave_elapsed+=dt;
	
	for (i=0;i<g->wave_event_count;i++) {
		WaveEvent* ev=&g->wave_events[i];
		if (ev->triggered && ev->duration<=0.0f) continue;
		
		/* 触发检查 */
		if (!ev->triggered && g->wave_elapsed>=ev->trigger_time) {
			ev->triggered=1;
			ev->timer=ev->duration;
			
			switch (ev->type) {
			case WAVE_EVENT_LIGHTNING:
				do_lightning(g);
				audio_play(&g->audio,"explosion");
				break;
			case WAVE_EVENT_FEAR_DEBUFF:
				apply_fear(g,1);
				break;
			case WAVE_EVENT_FREEZE_TOWERS:
				do_freeze_all_towers(g,ev->duration);
				audio_play(&g->audio,"freeze");
				/* Stone W3: randomly remove half the bonfires from the tower list */
				{ int j;
					int bf[MAX_TOWERS], bf_n=0;
					for (j=0;j<g->tower_count;j++)
						if (g->towers[j].is_bonfire || g->towers[j].type==TOWER_FIRE) bf[bf_n++]=j;
					/* Fisher-Yates shuffle for random selection */
					for (j=bf_n-1;j>0;j--) {
						int r=GetRandomValue(0,j);
						int tmp=bf[j]; bf[j]=bf[r]; bf[r]=tmp;
					}
					/* sort removal indices descending so swap-remove stays valid */
					int remove_n=(bf_n+1)/2, k, l;
					for (k=0;k<remove_n-1;k++)
						for (l=k+1;l<remove_n;l++)
							if (bf[k]<bf[l]){ int t=bf[k];bf[k]=bf[l];bf[l]=t; }
					for (k=0;k<remove_n;k++) {
						int idx=bf[k];
						/* restore map cell so the spot can be rebuilt on */
						int cell_col=g->towers[idx].x/CELL_SIZE;
						int cell_row=g->towers[idx].y/CELL_SIZE;
						map_restore_cell(&g->game_map, cell_row, cell_col);
						g->towers[idx]=g->towers[g->tower_count-1];
						g->tower_count--;
					}
				}
				break;
			case WAVE_EVENT_PEASANT_CONVERT:
				/* handled by game_update after intro narr clears */
				break;
			case WAVE_EVENT_RELIGION_PHASE1:
				/* handled by game_update logic, not wave events */
				break;
			case WAVE_EVENT_CULT_SPAWN:
				/* handled by game_update logic, not wave events */
				break;
			case WAVE_EVENT_PLAGUE:
				do_plague_start(g);
				break;
			case WAVE_EVENT_REVOLUTION:
				do_revolution(g);
				break;
			case WAVE_EVENT_AI_BETRAY:
				do_ai_betray(g);
				break;
			case WAVE_EVENT_SANDSTORM_BURY:
				do_sandstorm_bury(g);
				break;
			case WAVE_EVENT_PLAYER_CHOICE:
				g->current_state=GAME_STATE_CHOICE;
				break;
				default: break;
			}
		}
		
		/* 持续事件倒计时 */
		if (ev->triggered && ev->duration>0.0f) {
			ev->timer-=dt;
			if (ev->timer<=0.0f) {
				ev->duration=0.0f;
				/* 到期解除 */
				if (ev->type==WAVE_EVENT_FEAR_DEBUFF) apply_fear(g,0);
			}
		}
	}
}

/* ── 鼠疫资源持续扣减 ── */
static void update_plague(Game* g, float dt) {
	if (!g->plague_active) return;
	g->plague_resource_drain+=dt;
	if (g->plague_resource_drain>=3.0f) {
		g->plague_resource_drain-=3.0f;
		g->metal=(g->metal>5)?g->metal-5:0;
		g->wood =(g->wood >5)?g->wood -5:0;
	}
}

/* ── 海浪/核辐射对塔的持续影响 ── */
static void update_enemy_tower_effects(Game* g) {
	int i, j;
	float dx, dy;
	for (i=0;i<g->enemy_count;i++) {
		if (!g->enemies[i].active) continue;
		
		/* 海浪：周围塔攻速-40% */
		if (g->enemies[i].enemy_type==ENEMY_SEAWAVE) {
			for (j=0;j<g->tower_count;j++) {
				dx=g->enemies[i].x-(float)g->towers[j].x;
				dy=g->enemies[i].y-(float)g->towers[j].y;
				if (dx*dx+dy*dy<=80.0f*80.0f) {
					g->towers[j].buff_speed_ratio=0.6f;
					g->towers[j].buff_timer=0.5f;
				}
			}
		}
		
		/* 核辐射：每4秒对周围随机一座塔释放污染（瘫痪）*/
		if (g->enemies[i].enemy_type==ENEMY_RADIATION) {
			if (g->enemies[i].radiation_pulse_cd<=0.0f) {
				g->enemies[i].radiation_pulse_cd=RADIATION_PULSE_INTERVAL;
				/* 收集200px内的塔，随机选一座 */
				int candidates_r[MAX_TOWERS], cand_n=0;
				for (j=0;j<g->tower_count;j++) {
					dx=g->enemies[i].x-(float)g->towers[j].x;
					dy=g->enemies[i].y-(float)g->towers[j].y;
					if (dx*dx+dy*dy<=200.0f*200.0f) candidates_r[cand_n++]=j;
				}
				if (cand_n>0) {
					int pick=candidates_r[GetRandomValue(0,cand_n-1)];
					g->towers[pick].is_frozen   =1;
					g->towers[pick].frozen_timer=3.0f;
					g->towers[pick].color_state =TOWER_COLOR_GREEN;
				}
			}
		}
		
		/* Miasma (Ancient W3): travels along the path at speed 80,
		towers within 48px with FACTION_PEOPLE get -30% fire rate and turn purple.
		Reuse ENEMY_RAT enemy type as the miasma carrier (naturally invisible). */
		if (g->enemies[i].enemy_type==ENEMY_RAT &&
			g->current_era==ERA_ANCIENT &&
			g->current_wave==3) {
			for (j=0;j<g->tower_count;j++) {
				if (g->towers[j].faction!=FACTION_PEOPLE) continue;
				float mdx=g->enemies[i].x-(float)g->towers[j].x;
				float mdy=g->enemies[i].y-(float)g->towers[j].y;
				if (mdx*mdx+mdy*mdy<=96.0f*96.0f) {
					g->towers[j].color_state   =TOWER_COLOR_PURPLE;
					g->towers[j].buff_speed_ratio=0.7f;
					g->towers[j].buff_timer     =0.5f;
				}
			}
		}
	}
}

/* ── 脓包怪爆炸处理 ── */
static void handle_slime_explosions(Game* g) {
	int i, j;
	float dx, dy;
	for (i=0;i<g->enemy_count;i++) {
		if (g->enemies[i].enemy_type!=ENEMY_SLIME) continue;
		if (!g->enemies[i].explosion_ready) continue;
		g->enemies[i].explosion_ready=0;
		
		float ex=g->enemies[i].x, ey=g->enemies[i].y;
		
		/* 眩晕周围所有塔3秒（复用 is_frozen）*/
		for (j=0;j<g->tower_count;j++) {
			dx=(float)g->towers[j].x-ex;
			dy=(float)g->towers[j].y-ey;
			if (dx*dx+dy*dy<=80.0f*80.0f) {
				g->towers[j].is_frozen   =1;
				g->towers[j].frozen_timer=3.0f;
			}
		}
		
		/* 触发爆炸视觉特效 */
		g->slime_explosion_active = 1;
		g->slime_explosion_timer  = 0.6f;
		g->slime_explosion_x      = ex;
		g->slime_explosion_y      = ey;
		
		/* 飘字 */
		if (g->float_text_count<MAX_FLOAT_TEXTS) {
			g->float_texts[g->float_text_count].x=(float)ex;
			g->float_texts[g->float_text_count].y=(float)ey-20;
			g->float_texts[g->float_text_count].damage=-3;
			g->float_texts[g->float_text_count].life=1.0f;
			g->float_text_count++;
		}
		audio_play(&g->audio,"explosion");
	}
}

/* ============================================================
* game_process_input
* ============================================================ */
void game_process_input(Game* g) {
	int mx, my, col, row, i;
	Point center;
	float dx, dy;
	
	if (g->current_state==GAME_STATE_MENU)     { menu_process_input(g);     return; }
	if (g->current_state==GAME_STATE_SETTINGS) { settings_process_input(g); return; }
	
	/* 叙事全屏：Space 推进 */
	if (narr_is_blocking(&g->narr)) {
		if (IsKeyPressed(KEY_SPACE)||IsKeyPressed(KEY_ENTER))
			narr_advance(&g->narr);
		return;
	}
	
	/* ── 多结局选择：先推叙事，再切换状态 ── */
	if (g->current_state==GAME_STATE_CHOICE) {
		if (IsKeyPressed(KEY_SIX)) {
			g->choice_selected=0;
			narr_push_fullscreen(&g->narr, GOLD,
								 "You revealed your civilisation——",
								 "Your technology, your resources, and your dignity as humans.",
								 NULL);
			narr_push_fullscreen(&g->narr,(Color){180,240,255,255},
								 "The aliens fell silent, then withdrew.",
								 "\"Worth preserving.\"",
								 "\"We are leaving. Perhaps one day you will come find us.\"",
								 NULL);
			g->current_state=GAME_STATE_VICTORY;
			audio_stop_bgm(&g->audio);
			audio_play(&g->audio,"victory");
		}
		if (IsKeyPressed(KEY_SEVEN)) {
			g->choice_selected=1;
			narr_push_fullscreen(&g->narr,LIGHTGRAY,
								 "You chose silence, hiding behind your defences.",
								 NULL);
			narr_push_fullscreen(&g->narr,(Color){180,180,180,255},
								 "\"Too weak.\"",
								 "\"Survive. Grow stronger. We will return.\"",
								 NULL);
			g->current_state=GAME_STATE_VICTORY;
			audio_stop_bgm(&g->audio);
			audio_play(&g->audio,"victory");
		}
		if (IsKeyPressed(KEY_EIGHT)) {
			g->choice_selected=2;
			g->comic_group5_loaded=1;   /* use battle narrative instead of comic 5 */
			g->current_state    = GAME_STATE_PLAYING_L3;
			g->is_wave_active   = 1;
			g->enemies_to_spawn = 15;
			g->enemies_spawned  = 0;
			g->spawn_interval   = 1.5f;
			g->spawn_timer      = g->spawn_interval;
		}
		return;
	}
	
	if (g->current_state==GAME_STATE_GAME_OVER ||
		g->current_state==GAME_STATE_VICTORY) {
		if (IsKeyPressed(KEY_R)) { game_reset_to_level(g,1); game_start_next_wave(g); }
		if (IsKeyPressed(KEY_M)) { g->current_state=GAME_STATE_MENU; g->menu_selected=0; audio_play_bgm(&g->audio,"menu"); }
		return;
	}
	if (g->current_state==GAME_STATE_PAUSED) {
		if (IsKeyPressed(KEY_P)) g->current_state=g->state_before_pause;
		return;
	}
	if ((g->current_state==GAME_STATE_PLAYING_L1||
		 g->current_state==GAME_STATE_PLAYING_L2||
		 g->current_state==GAME_STATE_PLAYING_L3)&&IsKeyPressed(KEY_P)) {
		g->state_before_pause=g->current_state;
		g->current_state=GAME_STATE_PAUSED; return;
	}
	
	/* 漫画翻页 */
	if (g->current_state==GAME_STATE_COMIC) {
		if (IsKeyPressed(KEY_SPACE)||IsKeyPressed(KEY_RIGHT)||
			IsKeyPressed(KEY_ENTER)||IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			if (g->comic_current_page<g->comic_page_count-1) g->comic_current_page++;
			else {
				if (g->comic_next_state==3) { g->current_state=GAME_STATE_PLAYING_L2; game_start_next_wave(g); }
				else g->current_state=GAME_STATE_VICTORY;
			}
		}
		if (IsKeyPressed(KEY_LEFT)&&g->comic_current_page>0) g->comic_current_page--;
		if (IsKeyPressed(KEY_ESCAPE)) g->comic_current_page=g->comic_page_count-1;
		return;
	}
	
	if (g->current_state!=GAME_STATE_PLAYING_L1&&
		g->current_state!=GAME_STATE_PLAYING_L2&&
		g->current_state!=GAME_STATE_PLAYING_L3) return;
	
	/* 彩蛋密码 */
	{ static const char code[]="ruixuezhaofengnian";
		static int cp=0; int k;
		for (k=KEY_A;k<=KEY_Z;k++) {
			if (IsKeyPressed(k)) {
				char ch=(char)('a'+(k-KEY_A));
				if (ch==code[cp]) { cp++; if(code[cp]=='\0'){g->wood+=1000;g->stone+=1000;g->metal+=1000;g->material+=1000;audio_play(&g->audio,"upgrade");cp=0;} }
				else cp=(ch==code[0])?1:0;
				break;
			}
		}
	}
	
	/* 清理掩埋的塔（按 C 消耗资源）*/
	if (IsKeyPressed(KEY_C)) {
		for (i=0;i<g->tower_count;i++) {
			if (g->towers[i].is_buried) {
				int cost = (g->current_era==ERA_FUTURE)?300:50;
				if (g->material>=cost||g->metal>=cost||g->wood>=cost) {
					if (g->current_era==ERA_FUTURE) g->material-=cost;
					else if (g->current_era==ERA_ANCIENT) g->metal-=cost;
					else g->wood-=cost;
					g->towers[i].is_buried=0;
				}
			}
		}
	}
	
	if (IsKeyPressed(KEY_ESCAPE)) {
		g->selected_tower_index=-1; g->base_selected=0;
		if (g->portal_pending_index>=0) {
			Tower* pa=&g->towers[g->portal_pending_index];
			int c2=pa->x/CELL_SIZE,r2=pa->y/CELL_SIZE;
			map_restore_cell(&g->game_map,r2,c2);
			g->towers[g->portal_pending_index]=g->towers[g->tower_count-1];
			g->tower_count--; g->portal_pending_index=-1;
		}
	}
	
	/* 快捷键选塔 */
	if (g->current_era==ERA_STONE) {
		if (IsKeyPressed(KEY_ONE))   g->current_selected_tower=TOWER_SLING;
		if (IsKeyPressed(KEY_TWO))   g->current_selected_tower=TOWER_BONESPEAR;
		if (IsKeyPressed(KEY_THREE)) g->current_selected_tower=TOWER_TOTEM;
		if (IsKeyPressed(KEY_FOUR))  g->current_selected_tower=TOWER_FIRE;
	} else if (g->current_era==ERA_ANCIENT) {
		if (IsKeyPressed(KEY_ONE))   g->current_selected_tower=TOWER_CROSSBOW;
		if (IsKeyPressed(KEY_TWO))   g->current_selected_tower=TOWER_MINE;
		if (IsKeyPressed(KEY_THREE)) g->current_selected_tower=TOWER_CAULDRON;
		if (IsKeyPressed(KEY_FOUR))  g->current_selected_tower=TOWER_FIRECROW;
	} else {
		if (IsKeyPressed(KEY_ONE))   g->current_selected_tower=TOWER_PULSE;
		if (IsKeyPressed(KEY_TWO))   g->current_selected_tower=TOWER_GRAVITY;
		if (IsKeyPressed(KEY_THREE)) g->current_selected_tower=TOWER_PORTAL;
		if (IsKeyPressed(KEY_FOUR))  g->current_selected_tower=TOWER_TIMECLOCK;
		if (IsKeyPressed(KEY_FIVE))  g->current_selected_tower=TOWER_LASER;
	}
	
	if (!IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) return;
	mx=MOUSE_LX(); my=MOUSE_LY();
	
	/* 升级面板（塔）*/
	if (g->selected_tower_index>=0) {
		Tower* sel=&g->towers[g->selected_tower_index];
		int pw=170,ph=115,px=sel->x+24,py=sel->y-60;
		if (px+pw>1024)px=sel->x-pw-8;if(py<65)py=65;if(py+ph>768)py=768-ph-4;
		Rectangle bA={(float)(px+5),(float)(py+44),(float)(pw-10),22.0f};
		Rectangle bB={(float)(px+5),(float)(py+72),(float)(pw-10),22.0f};
		if (CheckCollisionPointRec((Vector2){(float)mx,(float)my},bA)){game_upgrade_tower(g,0);return;}
		if (CheckCollisionPointRec((Vector2){(float)mx,(float)my},bB)){game_upgrade_tower(g,1);return;}
		if (sel->type==TOWER_GRAVITY&&sel->level_a>=2) {
			Rectangle bEx={(float)(px+5),(float)(py+96),(float)(pw-10),16.0f};
			if (CheckCollisionPointRec((Vector2){(float)mx,(float)my},bEx)){sel->gravity_explode_ready=1;return;}
		}
	}
	/* 升级面板（基地）*/
	if (g->base_selected) {
		int px=g->base.x+50,py=g->base.y-70,pw=175,ph=115;
		if (px+pw>1024)px=g->base.x-pw-20;if(py<65)py=65;if(py+ph>768)py=768-ph-4;
		Rectangle bA={(float)(px+5),(float)(py+44),(float)(pw-10),22.0f};
		Rectangle bB={(float)(px+5),(float)(py+72),(float)(pw-10),22.0f};
		if (CheckCollisionPointRec((Vector2){(float)mx,(float)my},bA)){int r=base_upgrade(&g->base,0,&g->wood,&g->stone,&g->metal,&g->material);if(r==1)g->warning_timer=1.0f;return;}
		if (CheckCollisionPointRec((Vector2){(float)mx,(float)my},bB)){int r=base_upgrade(&g->base,1,&g->wood,&g->stone,&g->metal,&g->material);if(r==1)g->warning_timer=1.0f;return;}
	}
	
	col=mx/CELL_SIZE; row=my/CELL_SIZE;
	dx=(float)(mx-g->base.x); dy=(float)(my-g->base.y);
	if (dx*dx+dy*dy<50.0f*50.0f){g->base_selected=!g->base_selected;g->selected_tower_index=-1;return;}
	
	for (i=0;i<g->tower_count;i++) {
		float ddx=g->towers[i].x-(float)(col*CELL_SIZE+CELL_SIZE/2);
		float ddy=g->towers[i].y-(float)(row*CELL_SIZE+CELL_SIZE/2);
		if (ddx*ddx+ddy*ddy<20.0f*20.0f) {
			g->selected_tower_index=(g->selected_tower_index==i)?-1:i;
			g->base_selected=0; return;
		}
	}
	
	g->selected_tower_index=-1; g->base_selected=0;
	
	/* 传送门第二步 */
	if (g->current_selected_tower==TOWER_PORTAL&&g->portal_pending_index>=0) {
		if (row>=0&&row<MAP_ROWS&&col>=0&&col<MAP_COLS&&map_is_path(&g->game_map,row,col)) {
			center=map_get_center(row,col);
			Tower* a=&g->towers[g->portal_pending_index];
			float dd=(center.x-a->x)*(center.x-a->x)+(center.y-a->y)*(center.y-a->y);
			float dist=sqrtf(dd);
			if (dist<3*CELL_SIZE||dist>15*CELL_SIZE){g->warning_timer=1.0f;g->warning_x=mx;g->warning_y=my;}
			else if (g->material>=500&&g->tower_count<MAX_TOWERS) {
				g->material-=500;
				tower_init(&g->towers[g->tower_count],center.x,center.y,(int)g->current_era,TOWER_PORTAL);
				g->towers[g->tower_count].portal_role=1;
				g->towers[g->tower_count].state=1;
				g->towers[g->tower_count].paired_index=g->portal_pending_index;
				a->paired_index=g->tower_count; a->state=1;
				g->tower_count++; g->portal_pending_index=-1;
			} else {g->warning_timer=1.0f;g->warning_x=mx;g->warning_y=my;}
		}
		return;
	}
	
	if (row<0||row>=MAP_ROWS||col<0||col>=MAP_COLS) return;
	int need_path=(g->current_selected_tower==TOWER_BONESPEAR||
				   g->current_selected_tower==TOWER_MINE||
				   g->current_selected_tower==TOWER_PORTAL);
	if (need_path?!map_is_path(&g->game_map,row,col):!map_can_place_tower(&g->game_map,row,col)) return;
	
	int cw=0,cs=0,cm=0,cmat=0;
	switch (g->current_selected_tower) {
		case TOWER_SLING:    cw=20;            break;
		case TOWER_BONESPEAR:cw=20;cs=30;      break;
		case TOWER_TOTEM:    cw=40;            break;
		case TOWER_CROSSBOW: cw=50;            break;
		case TOWER_MINE:     cm=100;           break;
		case TOWER_CAULDRON: cm=150;           break;
		case TOWER_FIRECROW: cs=200;cw=100;cm=120; break;
		case TOWER_PULSE:    cmat=420;         break;
		case TOWER_GRAVITY:  cmat=375;         break;
		case TOWER_TIMECLOCK:cmat=600;         break;
		case TOWER_LASER:    cmat=500;         break;
		default: break;
	}
	
	/* 古代A1：免费建造 */
	int is_free=0;
	if (g->current_era==ERA_ANCIENT) is_free=base_consume_free_build(&g->base);
	
	if (!is_free&&(g->wood<cw||g->stone<cs||g->metal<cm||g->material<cmat)) {
		g->warning_timer=1.0f;g->warning_x=mx;g->warning_y=my;return;
	}
	
	if (g->current_selected_tower==TOWER_PORTAL) {
		if (g->tower_count>=MAX_TOWERS) return;
		center=map_get_center(row,col);
		for (i=0;i<g->tower_count;i++){float d2=(g->towers[i].x-center.x)*(g->towers[i].x-center.x)+(g->towers[i].y-center.y)*(g->towers[i].y-center.y);if(d2<100.0f)return;}
		tower_init(&g->towers[g->tower_count],center.x,center.y,(int)g->current_era,TOWER_PORTAL);
		g->towers[g->tower_count].portal_role=0; g->towers[g->tower_count].state=0;
		g->portal_pending_index=g->tower_count++;
		return;
	}
	
	center=map_get_center(row,col);
	int occ=0,replace_idx=-1;
	for (i=0;i<g->tower_count;i++) {
		float d2=(g->towers[i].x-center.x)*(g->towers[i].x-center.x)+(g->towers[i].y-center.y)*(g->towers[i].y-center.y);
		if (d2<100.0f){if(g->towers[i].type==TOWER_BONESPEAR&&g->towers[i].ammo<=0)replace_idx=i;else occ=1;}
	}
	if (occ||g->tower_count>=MAX_TOWERS) return;
	
	if (!is_free){g->wood-=cw;g->stone-=cs;g->metal-=cm;g->material-=cmat;}
	audio_play(&g->audio,"place");
	if (g->current_selected_tower!=TOWER_BONESPEAR&&g->current_selected_tower!=TOWER_MINE)
		map_place_tower(&g->game_map,row,col);
	
	/* 确定新塔阵营（古代：震天雷/骨矛不计入；第1座手建=贵族，之后每4座人民后1座贵族）*/
	int new_faction=FACTION_NORMAL;
	if (g->current_era==ERA_ANCIENT &&
		g->current_selected_tower!=TOWER_MINE &&
		g->current_selected_tower!=TOWER_BONESPEAR) {
		/* manual_tower_built: 0=第1座→贵族, 1/2/3/4→人民, 5→贵族, … */
		new_faction = (g->manual_tower_built % 5 == 0) ? FACTION_NOBLE : FACTION_PEOPLE;
		g->manual_tower_built++;
	}
	
	/* Determine colour for new people tower based on current wave */
	int new_color = TOWER_COLOR_NORMAL;
	if (new_faction == FACTION_PEOPLE && g->current_era == ERA_ANCIENT) {
		if (g->revolution_active) {
			new_color = TOWER_COLOR_RED;    /* W4 revolution -> red */
		} else if (g->current_wave >= 3 && g->plague_active) {
			new_color = TOWER_COLOR_PURPLE; /* W3+: plague active -> purple */
		} else if (g->cult_spawn_pending == 2) {
			new_color = TOWER_COLOR_BLUE;   /* W2 ended, cult cleared -> blue */
		} else if (g->religion_intro_done) {
			/* W2 in progress: assign blue/green by position in people-tower sequence */
			int pcount = 0, pi;
			for (pi=0;pi<g->tower_count;pi++)
				if (g->towers[pi].faction==FACTION_PEOPLE) pcount++;
			new_color = (pcount % 4 == 3) ? TOWER_COLOR_GREEN : TOWER_COLOR_BLUE;
		}
	}
	
	if (replace_idx>=0) {
		tower_init(&g->towers[replace_idx],center.x,center.y,(int)g->current_era,g->current_selected_tower);
		g->towers[replace_idx].faction=new_faction;
		g->towers[replace_idx].color_state=new_color;
		g->towers[replace_idx].damage_mult=(new_faction==FACTION_PEOPLE)?1.1f:1.0f;
	} else {
		tower_init(&g->towers[g->tower_count],center.x,center.y,(int)g->current_era,g->current_selected_tower);
		g->towers[g->tower_count].faction=new_faction;
		g->towers[g->tower_count].color_state=new_color;
		g->towers[g->tower_count].damage_mult=(new_faction==FACTION_PEOPLE)?1.1f:1.0f;
		g->tower_count++;
	}
}

/* ============================================================
* game_update
* ============================================================ */
void game_update(Game* g, float dt) {
	int i, j;
	float dx, dy, rm;
	
	/* 叙事系统更新（可能阻塞游戏逻辑）*/
	int blocking = narr_update(&g->narr, dt);
	
	if (g->current_state==GAME_STATE_GAME_OVER||
		g->current_state==GAME_STATE_VICTORY   ||
		g->current_state==GAME_STATE_MENU      ||
		g->current_state==GAME_STATE_SETTINGS  ||
		g->current_state==GAME_STATE_PAUSED    ||
		g->current_state==GAME_STATE_TRANSITION||
		g->current_state==GAME_STATE_COMIC     ||
		g->current_state==GAME_STATE_CHOICE) return;
	
	/* 叙事全屏阻塞时游戏逻辑暂停 */
	if (blocking) return;
	
	/* Ancient W1: narr just finished — place towers and unlock spawning */
	if (g->current_state == GAME_STATE_PLAYING_L2 &&
		g->current_wave == 1 &&
		g->is_wave_active &&
		!g->peasant_intro_done) {
		do_peasant_convert(g);
	}
	
	/* Ancient W2 step-1: intro narr finished — colour assignment + unlock 60 peasants */
	if (g->current_state == GAME_STATE_PLAYING_L2 &&
		g->current_wave == 2 &&
		g->is_wave_active &&
		!g->religion_intro_done) {
		do_religion_phase1(g);
		g->enemies_to_spawn  = 60;
		g->spawn_timer       = g->spawn_interval;
		g->religion_intro_done = 1;
	}
	
	/* Ancient W2 step-2: all 60 peasants dead — push narr then spawn cult boss */
	if (g->current_state == GAME_STATE_PLAYING_L2 &&
		g->current_wave == 2 &&
		g->is_wave_active &&
		g->religion_intro_done &&
		!g->cult_spawn_pending &&
		g->enemies_spawned >= 60 &&
		g->enemy_count == 0) {
		g->cult_spawn_pending = 1;
		narr_push_fullscreen(&g->narr, (Color){200,80,255,255},
							 "The cult heretic believes the throne itself is the true heretic.",
							 NULL);
	}
	
	/* Ancient W2 step-3: narr cleared — spawn cult boss from path start */
	if (g->current_state == GAME_STATE_PLAYING_L2 &&
		g->current_wave == 2 &&
		g->cult_spawn_pending == 1 &&
		!narr_is_blocking(&g->narr)) {
		do_cult_spawn(g);
		g->cult_spawn_pending = 2;   /* mark done so we don't re-trigger */
	}
	
	if (g->base.hp<=0) {
		g->current_state=GAME_STATE_GAME_OVER;
		audio_stop_bgm(&g->audio);
		if (g->choice_selected==2 && g->current_era==ERA_FUTURE) {
			narr_push_fullscreen(&g->narr, BLACK,
								 "Human civilisation endured for",
								 "7,982 years.",
								 NULL);
		} else {
			audio_play(&g->audio,"gameover");
		}
		return;
	}
	if (g->warning_timer>0.0f) g->warning_timer-=dt;
	
	/* Lightning visual effect countdown */
	if (g->lightning_active) {
		g->lightning_timer -= dt;
		if (g->lightning_timer <= 0.0f) g->lightning_active = 0;
	}
	
	/* Slime explosion visual effect countdown */
	if (g->slime_explosion_active) {
		g->slime_explosion_timer -= dt;
		if (g->slime_explosion_timer <= 0.0f) g->slime_explosion_active = 0;
	}
	
	/* Ancient W4 end: advance to comic once the closing narrative finishes */
	if (g->pending_ancient_end && !narr_is_blocking(&g->narr) && !g->narr.active) {
		g->pending_ancient_end = 0;
		game_start_next_wave(g);
	}
	
	/* 资源增长 */
	rm=(g->current_era==ERA_STONE&&g->base.level_a>=1)?1.5f:1.0f;
	if (g->revolution_active) rm*=2.0f;
	g->resource_timer+=dt;
	if (g->current_era==ERA_STONE) {
		if (g->resource_timer>=5.0f){g->wood+=(int)(3.0f*rm);g->stone+=(int)(3.0f*rm);g->resource_timer-=5.0f;}
	} else if (g->current_era==ERA_ANCIENT) {
		if (g->peasant_bonus_active) rm*=1.7f;
		if (g->resource_timer>=1.0f){g->wood+=(int)(2.0f*rm);g->metal+=(int)(2.0f*rm);g->resource_timer-=1.0f;}
	} else {
		if (g->resource_timer>=1.0f){g->material+=7;g->resource_timer-=1.0f;}
	}
	
	/* 鼠疫扣减 */
	update_plague(g, dt);
	
	/* 波次事件 */
	if (g->is_wave_active) process_wave_events(g, dt);
	
	/* Future era W1-W3: phased spawn (sea waves -> sandstorm batches -> radiation) */
	if (g->is_wave_active) update_future_phase_spawn(g, dt);
	
	/* Future W2: AI betrayal phase system */
	update_ai_betray_phase(g, dt);
	
	/* 刷怪 */
	if (g->is_wave_active) {
		if (g->enemies_spawned<g->enemies_to_spawn) {
			g->spawn_timer-=dt;
			if (g->spawn_timer<=0.0f&&g->enemy_count<MAX_ENEMIES) {
				int cur_level=(g->current_state==GAME_STATE_PLAYING_L1)?1:
				(g->current_state==GAME_STATE_PLAYING_L2)?2:3;
				EnemyType etype=game_get_spawn_type(cur_level,g->current_wave,g->enemies_spawned);
				enemy_init(&g->enemies[g->enemy_count],
						   g->game_map.waypoints[0],
						   g->enemies_spawned, etype);
				/* 石器W2头狼HP翻倍 */
				if (cur_level==1&&g->current_wave==2&&g->enemies_spawned==0)
					g->enemies[g->enemy_count].hp*=2;
				g->enemy_count++; g->enemies_spawned++;
				g->spawn_timer=g->spawn_interval;
			}
		} else if (g->cult_bosses_to_spawn > 0) {
			/* Cult bosses drain from queue at normal spawn interval */
			g->spawn_timer-=dt;
			if (g->spawn_timer<=0.0f&&g->enemy_count<MAX_ENEMIES) {
				int boss_idx = g->cult_bosses_to_spawn - 1;
				Point spawn_pos = g->game_map.waypoints[0];
				enemy_init(&g->enemies[g->enemy_count], spawn_pos,
						   1000+g->enemy_count, ENEMY_CULT_BOSS);
				g->enemies[g->enemy_count].cult_tower_type = g->cult_boss_types[boss_idx];
				g->enemy_count++;
				g->cult_bosses_to_spawn--;
				g->spawn_timer=g->spawn_interval;
			}
		} else if (g->enemy_count==0) {
			/* L2W2: hold wave open until all cult bosses spawned AND defeated */
			int hold_for_cult = (g->current_state==GAME_STATE_PLAYING_L2 &&
								 g->current_wave==2 &&
								 (g->cult_spawn_pending != 2 || g->cult_bosses_to_spawn > 0));
			/* Future W1-W3: wave end controlled by phase 9, not here */
			int hold_for_future = (g->current_state==GAME_STATE_PLAYING_L3 &&
								   g->current_wave>=1 && g->current_wave<=3 &&
								   g->future_phase!=0);
			/* Future W2: wave end controlled by ai_betray_phase */
			int hold_for_ai = (g->current_state==GAME_STATE_PLAYING_L3 &&
							   g->current_wave==2 &&
							   g->ai_betray_phase!=0);
			if (!hold_for_cult && !hold_for_future && !hold_for_ai) {
				/* ── 波次结束：按关卡/波次推送结尾叙事 ── */
				int cur_level2=(g->current_state==GAME_STATE_PLAYING_L1)?1:
				(g->current_state==GAME_STATE_PLAYING_L2)?2:3;
				if (cur_level2==1 && g->current_wave==3) {
					narr_push_fullscreen(&g->narr, LIGHTGRAY,
										 "Nature is not the enemy.",
										 "It is the merciless backdrop.",
										 NULL);
				}
				if (cur_level2==2 && g->current_wave==1) {
					narr_push_fullscreen(&g->narr, (Color){220,220,180,255},
										 "They stayed.",
										 NULL);
				}
				if (cur_level2==2 && g->current_wave==4) {
					narr_push_fullscreen(&g->narr, GOLD,
										 "One person cannot hold a city. But many can.",
										 "Not because of numbers —",
										 "because they stand on the same side.",
										 NULL);
					g->pending_ancient_end = 1;
				}
				g->is_wave_active=0;
				/* For ancient W4 end, suppress auto-advance; pending_ancient_end handles it */
				g->wave_delay_timer = g->pending_ancient_end ? 9999.0f : WAVE_DELAY_SEC;
				if (cur_level2==3 && g->current_wave==3 && g->choice_selected==2) {
					narr_push_fullscreen(&g->narr, GOLD,
										 "The alien fleet has crashed.",
										 "Their ships held vast archives——",
										 "Recording the rise and fall of countless civilisations.",
										 "The final entry reads:",
										 "\"This civilisation chose to fight. Like the 127 before it.\"",
										 NULL);
				}
			}
		}
	} else {
		g->wave_delay_timer-=dt;
		if (g->wave_delay_timer<=0.0f) game_start_next_wave(g);
	}
	
	/* 鼠疫减益对塔伤害（古代W3）*/
	if (g->plague_active) {
		/* 通过降低 damage 字段实现，每次更新前检查 */
		/* 这里用一个简单方式：记录为 buff（不修改 damage，在伤害计算时再乘）*/
		/* 实际项目中可加一个 damage_mult 字段到 Tower；这里用 buff_speed_ratio 近似 */
	}
	
	/* Tower update */
	for (i=0;i<g->tower_count;i++) {
		/* Stone A2: base attack speed buff */
		if (g->current_era==ERA_STONE&&g->base.level_a>=2&&
			g->towers[i].buff_speed_ratio<1.3f) {
			g->towers[i].buff_speed_ratio=1.3f;
			g->towers[i].buff_timer=0.5f;
		}
		/* Ancient: apply colour_state speed/damage modifiers each frame. */
		if (g->current_era==ERA_ANCIENT) {
			switch (g->towers[i].color_state) {
			case TOWER_COLOR_BLUE:
				/* Blue: +15% fire rate, +10% damage */
				g->towers[i].buff_speed_ratio = 1.15f;
				g->towers[i].buff_timer       = 0.5f;
				g->towers[i].damage_mult      = 1.1f;
				break;
			case TOWER_COLOR_GREEN:
				/* Green: -20% fire rate, normal damage */
				g->towers[i].buff_speed_ratio = 0.8f;
				g->towers[i].buff_timer       = 0.5f;
				g->towers[i].damage_mult      = 1.0f;
				break;
			case TOWER_COLOR_PURPLE:
				/* Purple (miasma): -30% fire rate, -15% damage */
				g->towers[i].buff_speed_ratio = 0.7f;
				g->towers[i].buff_timer       = 0.5f;
				g->towers[i].damage_mult      = 0.85f;
				break;
			case TOWER_COLOR_RED:
				/* Red (revolution): +50% fire rate */
				g->towers[i].buff_speed_ratio = 1.5f;
				g->towers[i].buff_timer       = 0.5f;
				g->towers[i].damage_mult      = (g->towers[i].faction==FACTION_PEOPLE) ? 1.1f : 1.0f;
				break;
			default:
				/* NORMAL: restore base damage_mult */
				if (g->towers[i].color_state == TOWER_COLOR_NORMAL)
					g->towers[i].damage_mult = (g->towers[i].faction==FACTION_PEOPLE) ? 1.1f : 1.0f;
				break;
			}
		}
		tower_update(&g->towers[i],dt,
					 g->enemies,g->enemy_count,
					 g->towers,g->tower_count,
					 g->projectiles,&g->projectile_count,
					 g->float_texts,&g->float_text_count,
					 &g->wood,&g->stone,&g->metal,&g->material,
					 g->burn_zones,&g->burn_zone_count);
	}
	
	/* Betrayed tower logic: attack nearest non-betrayed tower every 2s.
	Destroyed towers also turn betrayed (absorbed into AI). */
	if (g->current_era==ERA_FUTURE) {
		static float betray_atk_timer=0.0f;
		betray_atk_timer+=dt;
		if (betray_atk_timer>=2.0f) {
			betray_atk_timer=0.0f;
			for (i=0;i<g->tower_count;i++) {
				if (!g->towers[i].is_betrayed) continue;
				/* Find nearest non-betrayed, non-buried tower */
				int target=-1; float best_d2=400.0f*400.0f;
				for (j=0;j<g->tower_count;j++) {
					if (g->towers[j].is_betrayed||g->towers[j].is_buried) continue;
					float ddx=(float)(g->towers[j].x-g->towers[i].x);
					float ddy=(float)(g->towers[j].y-g->towers[i].y);
					float d2=ddx*ddx+ddy*ddy;
					if (d2<best_d2){best_d2=d2;target=j;}
				}
				if (target<0) continue;
				/* Deal 300 damage to target tower (simulate via freeze as proxy,
				since towers have no HP field; mark destroyed if hit 10 times).
				Use a simple counter stored in bury_timer as hit-point tracker. */
				g->towers[target].bury_timer+=300.0f;
				floattext_spawn(g->float_texts,&g->float_text_count,
								(float)g->towers[target].x,(float)g->towers[target].y,300);
				projectile_spawn_full(g->projectiles,&g->projectile_count,
									  (float)g->towers[i].x,(float)g->towers[i].y,
									  (float)g->towers[target].x,(float)g->towers[target].y,
									  -1,0,1,0,0.15f);
				/* Tower destroyed at 3000 damage -> turn betrayed */
				if (g->towers[target].bury_timer>=3000.0f) {
					g->towers[target].is_betrayed=1;
					g->towers[target].bury_timer=0.0f;
					audio_play(&g->audio,"explosion");
				}
			}
		}
	}
	
	base_update(&g->base,dt,g->enemies,g->enemy_count,
				g->float_texts,&g->float_text_count,
				g->projectiles,&g->projectile_count);
	
	/* Enemy update */
	for (i=0;i<g->enemy_count;) {
		int reached = enemy_update(&g->enemies[i],dt,g->game_map.waypoints,g->game_map.waypoint_count);
		if (reached || g->enemies[i].reached_base || g->enemies[i].hp<=0 || !g->enemies[i].active) {
			if (reached || g->enemies[i].reached_base) {
				/* Ancient A2: trap shield */
				if (!base_consume_trap_shield(&g->base))
					g->base.hp--;
			} else if (!g->enemies[i].active &&
					   g->enemies[i].enemy_type==ENEMY_SEAWAVE &&
					   g->enemies[i].hp>0) {
				/* Sea wave reaches destination: deals 1 damage to base */
				g->base.hp--;
			}
			g->enemies[i]=g->enemies[g->enemy_count-1]; g->enemy_count--;
		} else i++;
	}
	
	/* 脓包怪爆炸 */
	handle_slime_explosions(g);
	
	/* 海浪/核辐射对塔的效果 */
	update_enemy_tower_effects(g);
	
	/* Bonfire reveals invisible enemies (Stone W2) */
	for (i=0;i<g->tower_count;i++) {
		if (!g->towers[i].is_bonfire) continue;
		for (j=0;j<g->enemy_count;j++) {
			if (!g->enemies[j].active) continue;
			dx=g->enemies[j].x-(float)g->towers[i].x;
			dy=g->enemies[j].y-(float)g->towers[i].y;
			if (dx*dx+dy*dy<=80.0f*80.0f)
				enemy_reveal(&g->enemies[j]);
		}
	}
	
	/* 抛射物结算 */
	for (i=0;i<g->projectile_count;) {
		g->projectiles[i].life-=dt;
		if (g->projectiles[i].life<=0.0f) {
			if (!g->projectiles[i].is_hit) {
				for (j=0;j<MAX_TRAPS;j++) {
					if (!g->traps[j].active) {
						g->traps[j].x=g->projectiles[i].target_x;
						g->traps[j].y=g->projectiles[i].target_y;
						g->traps[j].life=5.0f; g->traps[j].active=1; break;
					}
				}
			} else if (g->projectiles[i].damage>0) {
				for (j=0;j<g->enemy_count;j++) {
					if (g->enemies[j].id!=g->projectiles[i].target_id) continue;
					if (!g->enemies[j].active||g->enemies[j].hp<=0) break;
					enemy_take_damage(&g->enemies[j],g->projectiles[i].damage);
					audio_play(&g->audio,"explosion");
					floattext_spawn(g->float_texts,&g->float_text_count,
									g->enemies[j].x,g->enemies[j].y,
									g->projectiles[i].damage);
					break;
				}
			}
			g->projectiles[i]=g->projectiles[g->projectile_count-1]; g->projectile_count--;
		} else i++;
	}
	
	/* 绊脚石陷阱 */
	for (i=0;i<MAX_TRAPS;i++) {
		if (!g->traps[i].active) continue;
		g->traps[i].life-=dt;
		if (g->traps[i].life<=0.0f){g->traps[i].active=0;continue;}
		for (j=0;j<g->enemy_count;j++) {
			if (!g->enemies[j].active||g->enemies[j].stun_timer>0.0f) continue;
			dx=g->enemies[j].x-g->traps[i].x; dy=g->enemies[j].y-g->traps[i].y;
			if (dx*dx+dy*dy<15.0f*15.0f) {
				enemy_apply_stun(&g->enemies[j],1.0f);
				floattext_spawn(g->float_texts,&g->float_text_count,
								g->enemies[j].x,g->enemies[j].y,-2);
				g->traps[i].active=0; break;
			}
		}
	}
	
	floattext_update_all(g->float_texts,&g->float_text_count,dt);
}
