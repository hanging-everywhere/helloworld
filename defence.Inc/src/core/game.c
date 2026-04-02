#include "game.h"
#include <string.h>
#include <stdio.h>

/* ============================================================
* Internal: load comic page group
* ============================================================ */
static void load_comic_group(Game* g, const char* prefix, int count, int next_state) {
	int i; char path[128];
	for (i=0;i<g->comic_page_count;i++) UnloadTexture(g->comic_pages[i]);
	g->comic_page_count=0; g->comic_current_page=0; g->comic_next_state=next_state;
	for (i=1;i<=count;i++) {
		snprintf(path,sizeof(path),"assets/images/%s(%d).png",prefix,i);
		Texture2D tex=LoadTexture(path);
		if (tex.id!=0 && g->comic_page_count<MAX_COMIC_PAGES)
			g->comic_pages[g->comic_page_count++]=tex;
	}
}

/* ============================================================
* game_load_all_textures
* NOTE: enemy_texture filenames must match files under assets/images/
* ============================================================ */
void game_load_all_textures(Game* g) {
	g->bg_map_textures[0] = LoadTexture("assets/images/background(1).png");
	g->bg_map_textures[1] = LoadTexture("assets/images/background(2).png");
	g->bg_map_textures[2] = LoadTexture("assets/images/background(3).png");
	g->skeleton_texture   = LoadTexture("assets/images/Skeleton.png");
	
	g->base_textures[0] = LoadTexture("assets/images/base(1).png");
	g->base_textures[1] = LoadTexture("assets/images/base(2).png");
	g->base_textures[2] = LoadTexture("assets/images/base(3).png");
	
	g->tower_textures[1]  = LoadTexture("assets/images/sling.png");
	g->tower_textures[2]  = LoadTexture("assets/images/bonespear.png");
	g->tower_textures[3]  = LoadTexture("assets/images/totem.png");
	g->tower_textures[4]  = LoadTexture("assets/images/fire.png");
	g->tower_textures[5]  = LoadTexture("assets/images/crossbow.png");
	g->tower_textures[6]  = LoadTexture("assets/images/mine.png");
	g->tower_textures[7]  = LoadTexture("assets/images/cauldron.png");
	g->tower_textures[8]  = LoadTexture("assets/images/firecrow.png");
	g->tower_textures[9]  = LoadTexture("assets/images/pulse.png");
	g->tower_textures[10] = LoadTexture("assets/images/gravity.png");
	g->tower_textures[11] = LoadTexture("assets/images/portal.png");
	g->tower_textures[12] = LoadTexture("assets/images/timeclock.png");
	g->tower_textures[13] = LoadTexture("assets/images/laser.png");
	
	/* Enemy textures (indexed by EnemyType enum)*/
	g->enemy_textures[ENEMY_SKELETON]       = g->skeleton_texture;
	g->enemy_textures[ENEMY_WOLF]           = LoadTexture("assets/images/wolf.png");
	g->enemy_textures[ENEMY_BOAR]           = LoadTexture("assets/images/boar.png");
	g->enemy_textures[ENEMY_SABERTOOTH]     = LoadTexture("assets/images/sabertooth.png");
	g->enemy_textures[ENEMY_MAMMOTH]        = LoadTexture("assets/images/mammoth.png");
	g->enemy_textures[ENEMY_PEASANT]        = LoadTexture("assets/images/peasant.png");
	g->enemy_textures[ENEMY_CULT_BOSS]      = (Texture2D){0};           /* runtime: uses betrayed tower texture */
	g->enemy_textures[ENEMY_RAT]            = LoadTexture("assets/images/rat.png");
	g->enemy_textures[ENEMY_SLIME]          = LoadTexture("assets/images/slime.png");
	g->enemy_textures[ENEMY_NOBLE_SKELETON] = LoadTexture("assets/images/nobleskeleton.png");
	g->enemy_textures[ENEMY_WRAITH]         = LoadTexture("assets/images/wraith.png");
	g->enemy_textures[ENEMY_SANDSTORM]      = LoadTexture("assets/images/sandstorm.png");
	g->enemy_textures[ENEMY_SEAWAVE]        = LoadTexture("assets/images/wave.png");
	g->enemy_textures[ENEMY_RADIATION]      = LoadTexture("assets/images/radiation.png");
	
	InitAudioDevice();
	audio_init(&g->audio, g->master_volume, g->sfx_enabled);
}

/* ============================================================
* game_unload_all_textures
* ============================================================ */
void game_unload_all_textures(Game* g) {
	int i;
	audio_unload(&g->audio);
	CloseAudioDevice();
	for (i=0;i<3;i++) UnloadTexture(g->bg_map_textures[i]);
	for (i=0;i<3;i++) UnloadTexture(g->base_textures[i]);
	UnloadTexture(g->skeleton_texture);
	for (i=1;i<=13;i++) UnloadTexture(g->tower_textures[i]);
	/* Skip reused SKELETON (0) and empty CULT_BOSS */
	for (i=1;i<ENEMY_TYPE_COUNT;i++) {
		if (i==ENEMY_CULT_BOSS) continue;
		if (g->enemy_textures[i].id != 0)
			UnloadTexture(g->enemy_textures[i]);
	}
	for (i=0;i<g->comic_page_count;i++) UnloadTexture(g->comic_pages[i]);
}

/* ============================================================
* Internal: wave event registration
* ============================================================ */
static void wave_event_add(Game* g, WaveEventType type,
						   float trigger_time, float duration) {
	if (g->wave_event_count >= MAX_WAVE_EVENTS) return;
	WaveEvent* ev    = &g->wave_events[g->wave_event_count++];
	ev->type         = type;
	ev->trigger_time = trigger_time;
	ev->triggered    = 0;
	ev->duration     = duration;
	ev->timer        = duration;
	ev->param1       = 0;
}

static void setup_wave_events(Game* g) {
	g->wave_event_count = 0;
	g->wave_elapsed     = 0.0f;
	
	if (g->current_state == GAME_STATE_PLAYING_L1) {
		if (g->current_wave == 2) {
			wave_event_add(g, WAVE_EVENT_LIGHTNING,    0.5f,  0.0f);
			wave_event_add(g, WAVE_EVENT_FEAR_DEBUFF,  0.0f,  999.0f);
		} else if (g->current_wave == 3) {
			wave_event_add(g, WAVE_EVENT_FREEZE_TOWERS, 1.0f, 10.0f);
		}
	} else if (g->current_state == GAME_STATE_PLAYING_L2) {
		if      (g->current_wave == 1) {
			wave_event_add(g, WAVE_EVENT_PEASANT_CONVERT, 30.0f, 0.0f);
		} else if (g->current_wave == 2) {
			/* Religion/cult handled by game_update logic, not timed events */
		} else if (g->current_wave == 3) {
			wave_event_add(g, WAVE_EVENT_PLAGUE,  0.0f, 999.0f);
		} else if (g->current_wave == 4) {
			wave_event_add(g, WAVE_EVENT_REVOLUTION, 0.0f, 999.0f);
		}
	} else if (g->current_state == GAME_STATE_PLAYING_L3) {
		/* Sandstorm bury is handled by future phase spawn system, not wave events */
		if (g->current_wave == 2) {
			/* AI betrayal handled by ai_betray_phase system in game_update */
		} else if (g->current_wave == 3) {
			wave_event_add(g, WAVE_EVENT_PLAYER_CHOICE, 0.0f, 0.0f);
		}
	}
}

/* ============================================================
* Enemy type / count / interval — determined by level+wave
* ============================================================ */
EnemyType game_get_spawn_type(int level, int wave, int spawn_index) {
	if (level == 1) {
		if (wave == 1) return ENEMY_WOLF;
		if (wave == 2) {
			if (spawn_index == 0) return ENEMY_WOLF;   /* alpha wolf: hp x2 set by game_update */
			if (spawn_index < 3)  return ENEMY_BOAR;
			if (spawn_index < 10) return ENEMY_WOLF;
			return ENEMY_SABERTOOTH;
		}
		if (wave == 3) {
			if (spawn_index < 8) return ENEMY_MAMMOTH;
			return ENEMY_SABERTOOTH;
		}
	}
	if (level == 2) {
		if (wave == 1) return ENEMY_PEASANT;
		if (wave == 2) return ENEMY_PEASANT;
		if (wave == 3) return (spawn_index % 2 == 0) ? ENEMY_RAT : ENEMY_SLIME;
		if (wave == 4) return (spawn_index % 2 == 0) ? ENEMY_NOBLE_SKELETON : ENEMY_WRAITH;
	}
	if (level == 3) {
		/* Each wave: 5 sandstorms + 15 sea waves + 15 radiation = 35 */
		if (wave == 1 || wave == 2 || wave == 3) {
			if (spawn_index < 5)  return ENEMY_SANDSTORM;
			if (spawn_index < 20) return ENEMY_SEAWAVE;
			return ENEMY_RADIATION;
		}
	}
	return ENEMY_SKELETON;
}

static int get_wave_enemy_count(int level, int wave) {
	if (level == 1) { if(wave==1)return 14; if(wave==2)return 11; return 10; }
	if (level == 2) { if(wave==1)return 40; if(wave==2)return 60; if(wave==3)return 38; return 20; }
	if (level == 3) { if(wave==1)return 35; if(wave==2)return 35; return 35; }
	return 10;
}

static float get_wave_spawn_interval(int level, int wave) {
	if (level == 1) { if(wave==1)return 2.0f; if(wave==2)return 2.0f; return 3.0f; }
	if (level == 2) { if(wave==1)return 0.5f; if(wave==2)return 0.5f; if(wave==3)return 1.0f; return 1.5f; }
	if (level == 3) { if(wave==1)return 2.0f; if(wave==2)return 1.5f; return 3.0f; }
	return 1.5f;
}

/* ============================================================
* game_start_next_wave
* ============================================================ */
void game_start_next_wave(Game* g) {
	Point bp;
	int cur_level;
	
	g->current_wave++;
	g->enemies_spawned = 0;
	g->is_wave_active  = 1;
	/* Reset future era phase fields each wave */
	g->future_phase         = 0;
	g->future_phase_timer   = 0.0f;
	g->future_phase_spawned = 0;
	g->ai_betray_phase      = 0;
	g->ai_laser_cd          = 0.0f;
	
	cur_level = (g->current_state==GAME_STATE_PLAYING_L1)?1:
	(g->current_state==GAME_STATE_PLAYING_L2)?2:3;
	
	const WaveDef* wd = dataload_find_wave(g->wave_defs, g->wave_def_count,
										   cur_level, g->current_wave);
	if (wd) {
		g->enemies_to_spawn = wd->enemy_count;
		g->spawn_interval   = wd->spawn_interval;
	} else {
		g->enemies_to_spawn = get_wave_enemy_count(cur_level, g->current_wave);
		g->spawn_interval   = get_wave_spawn_interval(cur_level, g->current_wave);
	}
	
	setup_wave_events(g);
	g->spawn_timer = g->spawn_interval;
	
	/* Future era W1-W3: phase system takes over, suppress normal spawn loop */
	if (g->current_state == GAME_STATE_PLAYING_L3 &&
		g->current_wave >= 1 && g->current_wave <= 3) {
		g->enemies_to_spawn = 0;
		if (g->current_wave == 1) {
			/* W1: show opening narrative first, then start spawning */
			g->future_phase       = -1;
			g->future_phase_timer = 0.0f;
			narr_push_fullscreen(&g->narr, (Color){100,180,255,255},
								 "Everything you have done to nature,",
								 "nature will return in full.",
								 "Nature is not the enemy.",
								 "It is the merciless backdrop.",
								 NULL);
		} else if (g->current_wave == 2) {
			/* W2: AI betrayal phase system */
			g->future_phase     = 0; /* not used in W2 */
			g->ai_betray_phase  = 1; /* waiting for narr1 to finish, then build AI towers */
			g->ai_laser_cd      = 0.0f;
			/* Bonus: material +30% */
			g->material += g->material * 30 / 100;
			/* Push first narrative */
			narr_push_fullscreen(&g->narr, (Color){180,220,255,255},
								 "The ASI era arrives. Society advances at breakneck speed.",
								 "Resources increased by 30%.",
								 "People begin handing their towers over to AI management.",
								 NULL);
		} else {
			g->future_phase       = 1;
			g->future_phase_timer = 2.0f;
		}
	}
	
	if (g->current_wave > g->total_waves_this_level) {
		g->is_wave_active = 0;
		
		if (g->current_state == GAME_STATE_PLAYING_L1) {
			if (g->max_level_cleared < 1) g->max_level_cleared = 1;
			{ SaveData sd; sd.max_level_cleared=g->max_level_cleared;
				sd.master_volume=g->master_volume; sd.sfx_enabled=g->sfx_enabled;
				save_write(&sd); }
			g->current_state=GAME_STATE_PLAYING_L2; g->current_era=ERA_ANCIENT;
			g->current_wave=0; g->tower_count=0; g->enemy_count=0;
			g->total_waves_this_level=4;
			g->fear_active=0; g->plague_active=0; g->revolution_active=0;
			map_init(&g->game_map, 2);
			g->game_map.bg_img = g->bg_map_textures[1];
			bp = g->game_map.waypoints[g->game_map.waypoint_count-1];
			base_init(&g->base, bp.x, bp.y, 1);
			base_set_texture(&g->base, g->base_textures[1]); g->stone+=10000; g->metal=20000;
			g->current_selected_tower=TOWER_CROSSBOW;
			g->selected_tower_index=-1; g->portal_pending_index=-1;
			g->manual_tower_built=0;
			audio_play_bgm(&g->audio, "ancient");
			
		} else if (g->current_state == GAME_STATE_PLAYING_L2) {
			if (!g->comic_group4_loaded) {
				g->comic_group4_loaded=1;
				load_comic_group(g,"4(",13,3);
				g->current_state=GAME_STATE_COMIC; return;
			} else {
				if (g->max_level_cleared<2) g->max_level_cleared=2;
				{ SaveData sd; sd.max_level_cleared=g->max_level_cleared;
					sd.master_volume=g->master_volume; sd.sfx_enabled=g->sfx_enabled;
					save_write(&sd); }
				g->current_state=GAME_STATE_PLAYING_L3; g->current_era=ERA_FUTURE;
				g->current_wave=0; g->tower_count=0; g->enemy_count=0;
				g->total_waves_this_level=3;
				g->plague_active=0; g->revolution_active=0;
				map_init(&g->game_map, 3);
				g->game_map.bg_img=g->bg_map_textures[2];
				bp=g->game_map.waypoints[g->game_map.waypoint_count-1];
				base_init(&g->base, bp.x, bp.y, 2);
				base_set_texture(&g->base, g->base_textures[2]); g->current_selected_tower=TOWER_PULSE;
				g->selected_tower_index=-1; g->portal_pending_index=-1;
				audio_play_bgm(&g->audio, "future");
			}
		} else if (g->current_state == GAME_STATE_PLAYING_L3) {
			if (!g->comic_group5_loaded) {
				g->comic_group5_loaded=1;
				load_comic_group(g,"5(",17,99);
				g->current_state=GAME_STATE_COMIC; return;
			} else {
				if (g->max_level_cleared<3) g->max_level_cleared=3;
				{ SaveData sd; sd.max_level_cleared=g->max_level_cleared;
					sd.master_volume=g->master_volume; sd.sfx_enabled=g->sfx_enabled;
					save_write(&sd); }
				g->current_state=GAME_STATE_VICTORY;
				audio_stop_bgm(&g->audio);
				audio_play(&g->audio, "victory");
			}
		}
		return;
	}
	
	audio_play(&g->audio, "wave_start");
	
	/* Ancient W1: freeze spawning until intro narr finishes */
	if (g->current_state == GAME_STATE_PLAYING_L2 && g->current_wave == 1) {
		g->peasant_intro_done = 0;
		g->enemies_to_spawn   = 0;   /* held; released by game_update after narr clears */
		narr_push_fullscreen(&g->narr, (Color){255,220,120,255},
							 "80 local units march against you.",
							 "You cannot withstand an army this vast.",
							 "But rule need not rest on force alone.",
							 "Your virtue has drawn farmers weary of war.",
							 "Half of them have surrendered.",
							 "You gain 4 towers and a 70% resource bonus.",
							 "From now on, four in five towers will be blessed by the people.",
							 "Their damage increases by 10%%, and their fire rate by 20%.",
							 "But one in five will remain in the hands of the nobility.",
							 NULL);
	}
	
	/* Ancient W2: freeze spawning until intro narr finishes */
	if (g->current_state == GAME_STATE_PLAYING_L2 && g->current_wave == 2) {
		g->religion_intro_done = 0;
		g->cult_spawn_pending  = 0;
		g->cult_bosses_to_spawn = 0;
		g->enemies_to_spawn    = 0;
		narr_push_fullscreen(&g->narr, (Color){200,180,255,255},
							 "Religion has come to this land.",
							 "The people have found faith -- and grown stronger.",
							 "But a crisis seems to be brewing.",
							 NULL);
	}
}

/* ============================================================
* game_init
* ============================================================ */
void game_init(Game* g, int start_level) {
	int i; Point bp; int era;
	memset(g, 0, sizeof(Game));
	
	g->selected_tower_index  = -1;
	g->portal_pending_index  = -1;
	g->wave_delay_timer      = FIRST_WAVE_DELAY_SEC;
	g->state_before_pause    = GAME_STATE_MENU;
	for (i=0;i<MAX_TRAPS;i++) g->traps[i].active=0;
	narr_init(&g->narr);
	
	{ SaveData sd;
		if (save_read(&sd)) {
			g->master_volume    = sd.master_volume;
			g->sfx_enabled      = sd.sfx_enabled;
			g->max_level_cleared= sd.max_level_cleared;
		} else {
			g->master_volume=0.5f; g->sfx_enabled=1; g->max_level_cleared=0;
		}
	}
	
	g->tower_def_count = dataload_towers(g->tower_defs, 14);
	g->wave_def_count  = dataload_waves (g->wave_defs,  12);
	
	game_load_all_textures(g);
	
	if (start_level == 0) {
		g->current_state = GAME_STATE_MENU;
		g->current_era   = ERA_STONE;
		audio_play_bgm(&g->audio, "menu");
		return;
	}
	
	g->wood=5000; g->stone=5000; g->metal=0; g->material=0;
	if (start_level == 1) {
		g->current_state=GAME_STATE_PLAYING_L1; g->current_era=ERA_STONE;
		g->total_waves_this_level=3;
		g->current_selected_tower=TOWER_SLING;
	} else if (start_level == 2) {
		g->current_state=GAME_STATE_PLAYING_L2; g->current_era=ERA_ANCIENT;
		g->total_waves_this_level=4;
		g->metal=200; g->current_selected_tower=TOWER_CROSSBOW;
	} else {
		g->current_state=GAME_STATE_PLAYING_L3; g->current_era=ERA_FUTURE;
		g->total_waves_this_level=3;
		g->material=1000; g->current_selected_tower=TOWER_PULSE;
	}
	
	map_init(&g->game_map, start_level);
	g->game_map.bg_img = g->bg_map_textures[start_level-1];
	bp  = g->game_map.waypoints[g->game_map.waypoint_count-1];
	era = start_level - 1;
	base_init(&g->base, bp.x, bp.y, era);
	base_set_texture(&g->base, g->base_textures[era]);
}

/* ============================================================
* game_reset_to_level
* ============================================================ */
void game_reset_to_level(Game* g, int level) {
	int i; Point bp;
	
	g->enemy_count=0;           g->tower_count=0;
	g->float_text_count=0;      g->projectile_count=0;
	g->burn_zone_count=0;
	g->selected_tower_index=-1; g->base_selected=0;
	g->portal_pending_index=-1;
	g->warning_timer=0.0f;      g->warning_x=0; g->warning_y=0;
	g->resource_timer=0.0f;
	g->current_wave=0;          g->is_wave_active=0;
	g->wave_delay_timer=FIRST_WAVE_DELAY_SEC;
	g->wave_elapsed=0.0f;       g->wave_event_count=0;
	g->comic_page_count=0;      g->comic_current_page=0;
	g->comic_next_state=0;
	g->comic_group4_loaded=0;   g->comic_group5_loaded=0;
	g->menu_selected=0;
	g->plague_active=0;         g->plague_resource_drain=0.0f;
	g->fear_active=0;           g->revolution_active=0;
	g->peasant_bonus_active=0;  g->peasant_intro_done=0;
	g->religion_intro_done=0;   g->cult_spawn_pending=0;
	g->cult_bosses_to_spawn=0;
	g->manual_tower_built=0;
	g->pending_ancient_end=0;
	g->future_phase=0; g->future_phase_timer=0.0f; g->future_phase_spawned=0;
	g->ai_betray_phase=0; g->ai_laser_cd=0.0f;
	narr_init(&g->narr);
	for (i=0;i<MAX_TRAPS;i++) g->traps[i].active=0;
	
	g->wood=5000; g->stone=5000; g->metal=0; g->material=0;
	if (level == 1) {
		g->current_state=GAME_STATE_PLAYING_L1; g->current_era=ERA_STONE;
		g->total_waves_this_level=3;
		g->current_selected_tower=TOWER_SLING;
		audio_play_bgm(&g->audio, "stone");
	} else if (level == 2) {
		g->current_state=GAME_STATE_PLAYING_L2; g->current_era=ERA_ANCIENT;
		g->total_waves_this_level=4;
		g->metal=20000; g->current_selected_tower=TOWER_CROSSBOW;
		audio_play_bgm(&g->audio, "ancient");
	} else {
		g->current_state=GAME_STATE_PLAYING_L3; g->current_era=ERA_FUTURE;
		g->total_waves_this_level=3;
		g->material=100000; g->current_selected_tower=TOWER_PULSE;
		audio_play_bgm(&g->audio, "future");
	}
	
	map_init(&g->game_map, level);
	g->game_map.bg_img = g->bg_map_textures[level-1];
	bp = g->game_map.waypoints[g->game_map.waypoint_count-1];
	base_init(&g->base, bp.x, bp.y, level-1);
	base_set_texture(&g->base, g->base_textures[level-1]);
}

/* ============================================================
* game_upgrade_tower
* ============================================================ */
void game_upgrade_tower(Game* g, int path) {
	Tower* t;
	if (g->selected_tower_index < 0 ||
		g->selected_tower_index >= g->tower_count) return;
	t = &g->towers[g->selected_tower_index];
	
	/* Betrayed/frozen/buried towers cannot be upgraded */
	if (t->is_betrayed || t->is_frozen || t->is_buried) {
		g->warning_timer = 1.0f; return;
	}
	
#define NOROOM    { g->warning_timer=1.0f; return; }
#define CHK_A     if (t->level_b>0) NOROOM
#define CHK_B     if (t->level_a>0) NOROOM
#define COST_W(n) if (g->wood    <(n)) NOROOM; g->wood    -=(n); audio_play(&g->audio,"upgrade")
#define COST_S(n) if (g->stone   <(n)) NOROOM; g->stone   -=(n); audio_play(&g->audio,"upgrade")
#define COST_M(n) if (g->metal   <(n)) NOROOM; g->metal   -=(n); audio_play(&g->audio,"upgrade")
#define COST_T(n) if (g->material<(n)) NOROOM; g->material-=(n); audio_play(&g->audio,"upgrade")
	
	switch (t->type) {
	case TOWER_SLING:
		if (path==0) {
			CHK_A;
			if      (t->level_a==0) { COST_W(30); t->level_a=1; t->paired_index=85; }
			else if (t->level_a==1) { COST_W(30); t->level_a=2;
				t->damage=(int)(t->damage*1.5f); t->range=(int)(t->range*1.2f); }
		} else {
			CHK_B;
			if (t->level_b==0) { COST_S(40); t->level_b=1;
				t->paired_index=100; t->cooldown*=10.0f/13.0f; }
		}
		break;
		
	case TOWER_BONESPEAR:
		if (path==0) {
			CHK_A;
			if      (t->level_a==0) { COST_W(20); t->level_a=1; }
			else if (t->level_a==1) { COST_W(30); t->level_a=2; }
		} else {
			CHK_B;
			if (t->level_b==0) { COST_W(50); t->level_b=1; }
		}
		break;
		
	case TOWER_TOTEM:
		if (path==0) {
			CHK_A;
			if (t->level_a==0) { COST_W(30); t->level_a=1; }
		} else {
			CHK_B;
			if      (t->level_b==0) { COST_W(40); t->level_b=1; }
			else if (t->level_b==1) { COST_W(60); t->level_b=2; }
		}
		break;
		
	case TOWER_FIRE:
		if (path==0) {
			CHK_A;
			if      (t->level_a==0) { COST_W(100); t->level_a=1; t->damage=(int)(t->damage*1.4f); }
			else if (t->level_a==1) { COST_W(150); t->level_a=2; }
		} else {
			CHK_B;
			if      (t->level_b==0) { COST_S(50); t->level_b=1; }
			else if (t->level_b==1) { COST_S(80); t->level_b=2; }
		}
		break;
		
	case TOWER_CROSSBOW:
		if (path==0) {
			CHK_A;
			if      (t->level_a==0) { COST_M(40); t->level_a=1; }
			else if (t->level_a==1) { COST_M(40); t->level_a=2; }
		} else {
			CHK_B;
			if      (t->level_b==0) { COST_M(60); t->level_b=1; }
			else if (t->level_b==1) { COST_M(60); t->level_b=2; }
		}
		break;
		
	case TOWER_MINE:
		if (path==0) {
			CHK_A;
			if      (t->level_a==0) { COST_M(50); t->level_a=1; }
			else if (t->level_a==1) { COST_M(80); t->level_a=2; }
		} else {
			CHK_B;
			if      (t->level_b==0) { COST_M(60); t->level_b=1; }
			else if (t->level_b==1) { COST_M(80); t->level_b=2; }
		}
		break;
		
	case TOWER_CAULDRON:
		if (path==0) {
			CHK_A;
			if      (t->level_a==0) { COST_M(80);  t->level_a=1; }
			else if (t->level_a==1) { COST_M(100); t->level_a=2; }
		} else {
			CHK_B;
			if      (t->level_b==0) { COST_M(80);
				t->level_b=1; t->explosive_charges=3; t->explosive_dmg_mult=1.0f; }
			else if (t->level_b==1) { COST_M(100); t->level_b=2; }
		}
		break;
		
	case TOWER_FIRECROW:
		if (path==0) { CHK_A; if (t->level_a==0) { COST_M(100); t->level_a=1; } }
		else         { CHK_B; if (t->level_b==0) { COST_M(80);  t->level_b=1; } }
		break;
		
	case TOWER_PULSE:
		if (path==0) {
			CHK_A;
			if      (t->level_a==0) { COST_T(180); t->level_a=1; }
			else if (t->level_a==1) { COST_T(180); t->level_a=2; }
		} else {
			CHK_B;
			if      (t->level_b==0) { COST_T(80); t->level_b=1; }
			else if (t->level_b==1) { COST_T(80); t->level_b=2; }
		}
		break;
		
	case TOWER_GRAVITY:
		if (path==0) {
			CHK_A;
			if      (t->level_a==0) { COST_T(150); t->level_a=1; }
			else if (t->level_a==1) { COST_T(150); t->level_a=2; }
		} else {
			CHK_B;
			if      (t->level_b==0) { COST_T(150); t->level_b=1;
				t->current_cooldown=0.0f; t->timer2=4.0f; }
			else if (t->level_b==1) { COST_T(200); t->level_b=2; }
		}
		break;
		
	case TOWER_PORTAL:
		if (path==0) {
			CHK_A;
			if (t->level_a==0) { COST_T(200); t->level_a=1;
				if (t->paired_index>=0) g->towers[t->paired_index].level_a=1; }
		} else {
			CHK_B;
			if      (t->level_b==0) { COST_T(200); t->level_b=1;
				if (t->paired_index>=0) g->towers[t->paired_index].level_b=1; }
			else if (t->level_b==1) { COST_T(200); t->level_b=2;
				if (t->paired_index>=0) g->towers[t->paired_index].level_b=2; }
		}
		break;
		
	case TOWER_TIMECLOCK:
		if (path==0) {
			CHK_A;
			if      (t->level_a==0) { COST_T(100); t->level_a=1; }
			else if (t->level_a==1) { COST_T(100); t->level_a=2; }
		} else {
			CHK_B;
			if (t->level_b==0) { COST_T(100); t->level_b=1; }
		}
		break;
		
	case TOWER_LASER:
		if (path==0) {
			CHK_A;
			if      (t->level_a==0) { COST_T(150); t->level_a=1; }
			else if (t->level_a==1) { COST_T(150); t->level_a=2; }
		} else {
			CHK_B;
			if      (t->level_b==0) { COST_T(70); t->level_b=1; }
			else if (t->level_b==1) { COST_T(70); t->level_b=2; }
		}
		break;
		
		default: break;
	}
	
#undef NOROOM
#undef CHK_A
#undef CHK_B
#undef COST_W
#undef COST_S
#undef COST_M
#undef COST_T
}
