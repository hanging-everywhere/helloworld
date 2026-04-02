#include "game.h"
#include "scale.h"
#include "../ui/menu.h"
#include "../ui/settings.h"
#include <math.h>

/* ============================================================
* draw_slime_explosion_effect — 脓包怪爆炸冲击波特效
* ============================================================ */
static void draw_slime_explosion_effect(float cx, float cy, float timer, float max_time) {
	float alpha   = timer / max_time;          /* 1.0 -> 0.0 淡出 */
	float progress = 1.0f - alpha;             /* 0.0 -> 1.0 扩张 */
	float max_r   = 80.0f;                     /* 最大半径 = 眩晕范围 */
	
	/* 三圈扩张冲击波，相位错开 */
	int k;
	for (k = 0; k < 3; k++) {
		float phase   = (float)k / 3.0f;
		float r_prog  = fmodf(progress + phase, 1.0f);
		float r       = max_r * r_prog;
		float a       = alpha * (1.0f - r_prog); /* 越大越透明 */
		Color ring    = Fade((Color){100,220,80,255}, a);
		DrawCircleLines((int)cx, (int)cy, r, ring);
	}
	
	/* 中心毒素闪光 */
	if (alpha > 0.5f) {
		float flash = (alpha - 0.5f) / 0.5f;
		DrawCircle((int)cx, (int)cy, 14.0f * flash, Fade((Color){80,255,80,255}, flash * 0.5f));
	}
	
	/* 眩晕范围边界圈（固定80px，渐隐）*/
	DrawCircleLines((int)cx, (int)cy, max_r, Fade((Color){150,255,100,200}, alpha * 0.4f));
}

/* ============================================================
* draw_lightning_effect — 从屏幕顶部打到目标塔的锯齿闪电
* ============================================================ */
static void draw_lightning_effect(float tx, float ty, float timer, float max_time) {
	float alpha = timer / max_time;           /* 1.0 -> 0.0，随时间淡出 */
	int   steps = 8;                          /* 折线段数 */
	float sx = tx;                            /* 闪电起点横坐标（在塔正上方） */
	float sy = 0.0f;                          /* 从屏幕顶部出发 */
	float ex = tx, ey = ty;                   /* 终点：被摧毁的塔 */
	float seg_h = (ey - sy) / steps;
	float jitter = 28.0f;                     /* 锯齿偏移幅度 */
	int   k;
	
	/* 全屏白色闪光（落雷瞬间） */
	if (alpha > 0.7f) {
		float flash_alpha = (alpha - 0.7f) / 0.3f;
		DrawRectangle(0, 0, 1024, 768, Fade(WHITE, flash_alpha * 0.35f));
	}
	
	/* 主闪电：两条偏移叠加，模拟电弧粗细 */
	for (k = 0; k < 2; k++) {
		float ox = sx;
		float oy = sy;
		float nx, ny;
		float offset_scale = (k == 0) ? 1.0f : 0.5f;
		Color bolt_col = (k == 0)
		? Fade((Color){255,255,180,255}, alpha)   /* 主弧：暖白 */
		: Fade((Color){200,220,255,255}, alpha);  /* 副弧：冷蓝 */
		float thick = (k == 0) ? 3.0f : 1.5f;
		
		int s;
		for (s = 0; s < steps; s++) {
			float progress = (float)(s + 1) / steps;
			nx = ex + GetRandomValue((int)(-jitter*offset_scale), (int)(jitter*offset_scale));
			ny = sy + seg_h * (s + 1);
			if (s == steps - 1) { nx = ex; ny = ey; } /* 最后一段精确落在塔上 */
			DrawLineEx((Vector2){ox, oy}, (Vector2){nx, ny}, thick, bolt_col);
			(void)progress;
			ox = nx; oy = ny;
		}
	}
	
	/* 落点光晕 */
	DrawCircle((int)tx, (int)ty, 18.0f * alpha, Fade((Color){255,255,100,255}, alpha * 0.6f));
	DrawCircle((int)tx, (int)ty, 8.0f,          Fade(WHITE, alpha));
}

static void get_upgrade_label(Tower* t, int path,
							  const char** lbl, int* aff,
							  int wood, int stone, int metal, int mat) {
	*lbl=""; *aff=0;
#define CHK(cost,res,str) {*lbl=str;*aff=(res>=cost);}
	/* (identical to original game_render.c)*/
	if (t->type==TOWER_SLING) {
		if (path==0){if(t->level_a==0)CHK(30,wood,"A1:Hit85% -30W") else if(t->level_a==1)CHK(30,wood,"A2:BigRock -30W") else *lbl="A:MAX";}
			else        {if(t->level_b==0&&t->level_a==0)CHK(40,stone,"B1:Hit100% -40S") else *lbl="B:MAX/MutexA";}
	} else if (t->type==TOWER_BONESPEAR) {
		if (path==0){if(t->level_a==0)CHK(20,wood,"A1:Slow -20W") else if(t->level_a==1)CHK(30,wood,"A2:Bleed -30W") else *lbl="A:MAX";}
			else        {if(t->level_b==0)CHK(50,wood,"B1:Triple -50W") else *lbl="B:MAX";}
	} else if (t->type==TOWER_TOTEM) {
		if (path==0){if(t->level_a==0)CHK(30,wood,"A1:Buff -30W") else *lbl="A:MAX";}
		else        {if(t->level_b==0)CHK(40,wood,"B1:Stun5s -40W") else if(t->level_b==1)CHK(60,wood,"B2:AoeDmg -60W") else *lbl="B:MAX";}
	} else if (t->type==TOWER_FIRE) {
		if (path==0){if(t->level_a==0)CHK(100,wood,"A1:+40% -100W") else if(t->level_a==1)CHK(150,wood,"A2:Eternal -150W") else *lbl="A:MAX";}
			else        {if(t->level_b==0)CHK(50,stone,"B1:Spray -50S") else if(t->level_b==1)CHK(80,stone,"B2:Wall -80S") else *lbl="B:MAX";}
	} else if (t->type==TOWER_CROSSBOW) {
		if (path==0){if(t->level_a==0)CHK(40,metal,"A1:Pierce -40M") else if(t->level_a==1)CHK(40,metal,"A2:DoT -40M") else *lbl="A:MAX";}
			else        {if(t->level_b==0)CHK(60,metal,"B1:3Shot -60M") else if(t->level_b==1)CHK(60,metal,"B2:x5 -60M") else *lbl="B:MAX";}
	} else if (t->type==TOWER_MINE) {
		if (path==0){if(t->level_a==0)CHK(50,metal,"A1:+70%R -50M") else if(t->level_a==1)CHK(80,metal,"A2:2Boom -80M") else *lbl="A:MAX";}
			else        {if(t->level_b==0)CHK(60,metal,"B1:AutoFill -60M") else if(t->level_b==1)CHK(80,metal,"B2:Chain -80M") else *lbl="B:MAX";}
	} else if (t->type==TOWER_CAULDRON) {
		if (path==0){if(t->level_a==0)CHK(80,metal,"A1:Poison -80M") else if(t->level_a==1)CHK(100,metal,"A2:ToxFog -100M") else *lbl="A:MAX";}
			else        {if(t->level_b==0)CHK(80,metal,"B1:Explosive -80M") else if(t->level_b==1)CHK(100,metal,"B2:Nitro -100M") else *lbl="B:MAX";}
	} else if (t->type==TOWER_FIRECROW) {
		if (path==0){if(t->level_a==0)CHK(100,metal,"A1:FastFire -100M") else *lbl="A:MAX";}
		else        {if(t->level_b==0)CHK(80,metal,"B1:CrowBomb -80M") else *lbl="B:MAX";}
	} else if (t->type==TOWER_PULSE) {
		if (path==0){if(t->level_a==0)CHK(180,mat,"A1:NoDecay -180") else if(t->level_a==1)CHK(180,mat,"A2:Stun -180") else *lbl="A:MAX";}
			else        {if(t->level_b==0)CHK(80,mat,"B1:Charge -80") else if(t->level_b==1)CHK(80,mat,"B2:Slice -80") else *lbl="B:MAX";}
	} else if (t->type==TOWER_GRAVITY) {
		if (path==0){if(t->level_a==0)CHK(150,mat,"A1:+30%R -150") else if(t->level_a==1)CHK(150,mat,"A2:Explode -150") else *lbl="A:MAX";}
			else        {if(t->level_b==0)CHK(150,mat,"B1:AntiGrav -150") else if(t->level_b==1)CHK(200,mat,"B2:BlackHole -200") else *lbl="B:MAX";}
	} else if (t->type==TOWER_PORTAL) {
		if (path==0){if(t->level_a==0)CHK(200,mat,"A1:Cut240 -200") else *lbl="A:MAX";}
		else        {if(t->level_b==0)CHK(200,mat,"B1:2xTele -200") else if(t->level_b==1)CHK(200,mat,"B2:Collapse -200") else *lbl="B:MAX";}
	} else if (t->type==TOWER_TIMECLOCK) {
		if (path==0){if(t->level_a==0)CHK(100,mat,"A1:Freeze -100") else if(t->level_a==1)CHK(100,mat,"A2:Rewind -100") else *lbl="A:MAX";}
			else        {if(t->level_b==0)CHK(100,mat,"B1:Causality -100") else *lbl="B:MAX";}
	} else if (t->type==TOWER_LASER) {
		if (path==0){if(t->level_a==0)CHK(150,mat,"A1:Store -150") else if(t->level_a==1)CHK(150,mat,"A2:Pulse800 -150") else *lbl="A:MAX";}
			else        {if(t->level_b==0)CHK(70,mat,"B1:FemtoLaser -70") else if(t->level_b==1)CHK(70,mat,"B2:Chain -70") else *lbl="B:MAX";}
	}
#undef CHK
	if (path==0&&t->level_b>0){*aff=0;if(*lbl&&(*lbl)[0]!='A')*lbl="[Locked]";}
	if (path==1&&t->level_a>0){*aff=0;if(*lbl&&(*lbl)[0]!='B')*lbl="[Locked]";}
}

static void draw_upgrade_panel(Tower* t, int px, int py, int pw, int ph,
							   int wood, int stone, int metal, int mat) {
	static const char* names[]={"","Sling","BoneSpear","Totem","Fire","Crossbow","Mine","Cauldron","FireCrow","Pulse","Gravity","Portal","TimeClock","Laser"};
	const char *lA,*lB; int afA,afB; Color cA,cB;
	DrawRectangle(px,py,pw,ph,(Color){20,20,20,220});
	DrawRectangleLines(px,py,pw,ph,GOLD);
	DrawText(names[t->type],px+5,py+5,13,GOLD);
	DrawText(TextFormat("LvA:%d LvB:%d",t->level_a,t->level_b),px+5,py+22,11,LIGHTGRAY);
	get_upgrade_label(t,0,&lA,&afA,wood,stone,metal,mat);
	cA=afA?GREEN:DARKGRAY;
	DrawRectangle(px+5,py+44,pw-10,22,(Color){40,40,40,255});DrawRectangleLines(px+5,py+44,pw-10,22,cA);DrawText(lA,px+8,py+49,10,cA);
	get_upgrade_label(t,1,&lB,&afB,wood,stone,metal,mat);
	cB=afB?SKYBLUE:DARKGRAY;
	DrawRectangle(px+5,py+72,pw-10,22,(Color){40,40,40,255});DrawRectangleLines(px+5,py+72,pw-10,22,cB);DrawText(lB,px+8,py+77,10,cB);
	if (t->type==TOWER_GRAVITY&&t->level_a>=2){DrawRectangle(px+5,py+96,pw-10,16,(Color){60,20,20,255});DrawRectangleLines(px+5,py+96,pw-10,16,RED);DrawText("EXPLODE",px+8,py+100,10,RED);}
	DrawText("[ESC]Close",px+5,py+ph-12,9,(Color){100,100,100,255});
}

/* ============================================================
* game_render
* ============================================================ */
void game_render(Game* g) {
	int i, mx, my, row, col;
	
	ClearBackground(BLACK);
	
	if (g->current_state==GAME_STATE_MENU)     { menu_render(g);     return; }
	if (g->current_state==GAME_STATE_SETTINGS) { settings_render(g); return; }
	
	/* Comic viewer */
	if (g->current_state==GAME_STATE_COMIC) {
		if (g->comic_page_count>0) {
			Texture2D pg=g->comic_pages[g->comic_current_page];
			float sx=1024.0f/pg.width,sy=768.0f/pg.height,sc=sx<sy?sx:sy;
			int dw=(int)(pg.width*sc),dh=(int)(pg.height*sc);
			int ox=(1024-dw)/2,oy=(768-dh)/2;
			DrawTexturePro(pg,(Rectangle){0,0,(float)pg.width,(float)pg.height},
						   (Rectangle){(float)ox,(float)oy,(float)dw,(float)dh},(Vector2){0,0},0.0f,WHITE);
			DrawRectangle(0,742,1024,26,(Color){0,0,0,180});
			DrawText(TextFormat("%d / %d",g->comic_current_page+1,g->comic_page_count),8,748,16,LIGHTGRAY);
			DrawText("[Space/Click] Next  [<-] Prev  [ESC] Skip",280,748,16,(Color){160,160,160,255});
			if (g->comic_current_page==g->comic_page_count-1)DrawText("[Space] Continue >>",820,748,16,GOLD);
		}
		return;
	}
	
	/* Multiple-ending choice screen */
	if (g->current_state==GAME_STATE_CHOICE) {
		map_draw(&g->game_map);
		DrawRectangle(0,0,1024,768,(Color){0,0,0,160});
		DrawText("They are watching.",512-MeasureText("They are watching.",40)/2,160,40,GOLD);
		struct { const char* key; const char* desc; Color col; } opts[]={
			{"[6]","Reveal your civilisation",(Color){100,200,100,255}},
			{"[7]","Hide and appear weak",    (Color){200,200,100,255}},
			{"[8]","Fight",                   (Color){220,80, 80,255}},
		};
		for (i=0;i<3;i++) {
			int y=280+i*120;
			Color hi=opts[i].col;
			DrawRectangle(362,y,300,80,(Color){20,20,20,220});
			DrawRectangleLines(362,y,300,80,hi);
			DrawText(opts[i].key, 375,y+12,28,hi);
			DrawText(opts[i].desc,375,y+46,22,RAYWHITE);
		}
		return;
	}
	
	map_draw(&g->game_map);
	
	mx=MOUSE_LX(); my=MOUSE_LY();
	row=my/CELL_SIZE; col=mx/CELL_SIZE;
	
	/* Placement preview */
	if (g->selected_tower_index<0&&!g->base_selected&&
		row>=0&&row<MAP_ROWS&&col>=0&&col<MAP_COLS) {
		int need_path=(g->current_selected_tower==TOWER_BONESPEAR||
					   g->current_selected_tower==TOWER_MINE||
					   g->current_selected_tower==TOWER_PORTAL);
		int can=need_path?map_is_path(&g->game_map,row,col)
		:map_can_place_tower(&g->game_map,row,col);
		if (can) {
			static const Color prev_colors[]={WHITE,LIGHTGRAY,RED,PURPLE,ORANGE,{160,110,50,255},{100,100,100,255},{180,140,50,255},{220,180,40,255},SKYBLUE,{80,80,160,255},VIOLET,{150,150,220,255},RED};
			Point c=map_get_center(row,col);
			Color pc=prev_colors[g->current_selected_tower];
			DrawRectangle(c.x-14,c.y-14,28,28,Fade(pc,0.4f));
			DrawRectangleLines(c.x-14,c.y-14,28,28,pc);
		}
	}
	
	/* Portal link preview */
	if (g->portal_pending_index>=0) {
		Tower* pa=&g->towers[g->portal_pending_index];
		DrawLineEx((Vector2){(float)pa->x,(float)pa->y},(Vector2){(float)mx,(float)my},1.0f,Fade(VIOLET,0.5f));
		DrawText("Place 2nd Portal (8-15 cells away)",10,700,14,VIOLET);
	}
	
	/* Selection highlight */
	if (g->selected_tower_index>=0){Tower* s=&g->towers[g->selected_tower_index];DrawRectangleLines(s->x-16,s->y-16,32,32,YELLOW);}
	if (g->base_selected) DrawCircleLines(g->base.x,g->base.y,52.0f,YELLOW);
	
	for (i=0;i<g->tower_count;i++) tower_draw_laser_beam(&g->towers[i],g->enemies,g->enemy_count);
	for (i=0;i<g->tower_count;i++) tower_draw(&g->towers[i],g->tower_textures);
	
	/* Enemy rendering (new signature: two texture arrays) */
	for (i=0;i<g->enemy_count;i++)
		enemy_draw(&g->enemies[i], g->enemy_textures, g->tower_textures);
	
	base_draw(&g->base);
	
	/* Traps */
	for (i=0;i<MAX_TRAPS;i++){if(!g->traps[i].active)continue;DrawCircle((int)g->traps[i].x,(int)g->traps[i].y,4.0f,GRAY);DrawCircleLines((int)g->traps[i].x,(int)g->traps[i].y,4.0f,DARKGRAY);}
	
	projectile_draw_all(g->projectiles,g->projectile_count);
	
	/* Upgrade panel (tower)*/
	if (g->selected_tower_index>=0&&g->selected_tower_index<g->tower_count) {
		Tower* sel=&g->towers[g->selected_tower_index];
		int pw=170,ph=120,px=sel->x+24,py=sel->y-60;
		if (px+pw>1024)px=sel->x-pw-8;if(py<65)py=65;if(py+ph>768)py=768-ph-4;
		draw_upgrade_panel(sel,px,py,pw,ph,g->wood,g->stone,g->metal,g->material);
	}
	
	/* Upgrade panel (base)*/
	if (g->base_selected) {
		int pw=175,ph=115,px=g->base.x+50,py=g->base.y-70;
		if (px+pw>1024)px=g->base.x-pw-20;if(py<65)py=65;if(py+ph>768)py=768-ph-4;
		const char *lA,*lB; int afA,afB;
		const char* bn=(g->base.era==0)?"Base:FireShrine":(g->base.era==1)?"Base:MechTower":"Base:Core";
		DrawRectangle(px,py,pw,ph,(Color){10,10,40,230});DrawRectangleLines(px,py,pw,ph,GOLD);
		DrawText(bn,px+5,py+5,13,GOLD);
		DrawText(TextFormat("HP:%d/%d LvA:%d",g->base.hp,g->base.max_hp,g->base.level_a),px+5,py+22,11,LIGHTGRAY);
		base_get_upgrade_label(&g->base,0,&lA,&afA,g->wood,g->stone,g->metal,g->material);
		Color cA=afA?GREEN:DARKGRAY;DrawRectangle(px+5,py+44,pw-10,22,(Color){30,30,30,255});DrawRectangleLines(px+5,py+44,pw-10,22,cA);DrawText(lA,px+8,py+49,10,cA);
		base_get_upgrade_label(&g->base,1,&lB,&afB,g->wood,g->stone,g->metal,g->material);
		Color cB=afB?SKYBLUE:DARKGRAY;DrawRectangle(px+5,py+72,pw-10,22,(Color){30,30,30,255});DrawRectangleLines(px+5,py+72,pw-10,22,cB);DrawText(lB,px+8,py+77,10,cB);
		DrawText("[ESC]Close",px+5,py+100,9,(Color){100,100,100,255});
	}
	
	/* Top status bar */
	DrawRectangle(0,0,1024,60,(Color){30,30,30,255});
	DrawLine(0,60,1024,60,(Color){100,100,100,255});
	const char* info_text;
	if (g->current_era==ERA_STONE)
		info_text=TextFormat("HP:%d  Wood:%d  Stone:%d  Wave:%d/%d",g->base.hp,g->wood,g->stone,g->current_wave,g->total_waves_this_level);
	else if (g->current_era==ERA_ANCIENT)
		info_text=TextFormat("HP:%d  Wood:%d  Metal:%d  Wave:%d/%d  %s%s",g->base.hp,g->wood,g->metal,g->current_wave,g->total_waves_this_level,g->plague_active?"[PLAGUE] ":"",g->revolution_active?"[REVOLUTION]":"");
	else
		info_text=TextFormat("HP:%d  Material:%d  Wave:%d/%d",g->base.hp,g->material,g->current_wave,g->total_waves_this_level);
	DrawText(info_text,12,10,20,BLACK); DrawText(info_text,10,8,20,GOLD);
	
	/* Plague / Revolution indicators */
	if (g->plague_active)
		DrawText("PLAGUE  Dmg-15%  Resources draining",10,38,13,(Color){180,255,150,255});
	if (g->revolution_active)
		DrawText("REVOLUTION  Atk+50%%  Resources x2",350,38,13,(Color){255,100,100,255});
	
	/* Buried tower indicator */
	{ int buried_count=0,j; for(j=0;j<g->tower_count;j++)if(g->towers[j].is_buried)buried_count++;
		if (buried_count>0) DrawText(TextFormat("[C] Clear %d buried tower(s)",buried_count),600,38,13,(Color){220,180,80,255}); }
	
	/* Insufficient resources warning */
	if (g->warning_timer>0.0f) {
		int fy=g->warning_y-20-(int)((1.0f-g->warning_timer)*30.0f);
		DrawText("Not Enough Resources!",g->warning_x-38,fy+2,18,BLACK);
		DrawText("Not Enough Resources!",g->warning_x-40,fy,18,RED);
	}
	
	floattext_draw_all(g->float_texts,g->float_text_count);
	
	if (!g->is_wave_active&&
		!g->narr.is_blocking&&
		(g->current_state==GAME_STATE_PLAYING_L1||
		 g->current_state==GAME_STATE_PLAYING_L2||
		 g->current_state==GAME_STATE_PLAYING_L3)&&
		 g->current_wave<g->total_waves_this_level)
		DrawText(TextFormat("Next Wave in: %.1f s",g->wave_delay_timer),350,350,40,WHITE);
	
	/* End screen */
	if (g->current_state==GAME_STATE_GAME_OVER) {
		DrawRectangle(0,0,1024,768,(Color){0,0,0,160});
		DrawText("GAME OVER",300,280,80,RED);
		DrawText("The base was destroyed...",310,380,28,LIGHTGRAY);
		DrawText("[R] Restart   [M] Main Menu",330,450,24,GOLD);
	}
	if (g->current_state==GAME_STATE_VICTORY) {
		DrawRectangle(0,0,1024,768,(Color){0,0,0,140});
		DrawText("YOU WIN!",350,280,80,GREEN);
		DrawText("All ages conquered!",350,380,30,LIGHTGRAY);
		DrawText("[R] Restart   [M] Main Menu",330,450,24,GOLD);
	}
	if (g->current_state==GAME_STATE_PAUSED) {
		DrawRectangle(0,0,1024,768,(Color){0,0,0,140});
		DrawText("PAUSED",370,310,80,GOLD);
		DrawText("Press [P] to continue",340,420,28,RAYWHITE);
	}
	
	/* Lightning visual effect (drawn before narrative overlay) */
	if (g->lightning_active)
		draw_lightning_effect(g->lightning_target_x, g->lightning_target_y,
							  g->lightning_timer, 0.6f);
	
	/* Slime explosion visual effect */
	if (g->slime_explosion_active)
		draw_slime_explosion_effect(g->slime_explosion_x, g->slime_explosion_y,
									g->slime_explosion_timer, 0.6f);
	
	/* AI laser enemy attack beams (Future W2 battle) */
	if (g->current_state==GAME_STATE_PLAYING_L3 &&
		g->current_wave==2 && g->ai_betray_phase==3) {
		int ei, tj;
		float dx, dy;
		for (ei=0;ei<g->enemy_count;ei++) {
			if (!g->enemies[ei].active) continue;
			if (g->enemies[ei].enemy_type!=ENEMY_CULT_BOSS) continue;
			/* Find nearest non-AI player tower within laser range */
			int nearest=-1; float best_d2=700.0f*700.0f;
			for (tj=0;tj<g->tower_count;tj++) {
				if (g->towers[tj].is_ai_tower) continue;
				dx=g->enemies[ei].x-(float)g->towers[tj].x;
				dy=g->enemies[ei].y-(float)g->towers[tj].y;
				float d2=dx*dx+dy*dy;
				if (d2<best_d2){best_d2=d2;nearest=tj;}
			}
			if (nearest>=0) {
				float pulse=(float)sin(GetTime()*12.0)*0.3f+0.7f;
				/* Outer glow */
				DrawLineEx((Vector2){g->enemies[ei].x, g->enemies[ei].y},
						   (Vector2){(float)g->towers[nearest].x,(float)g->towers[nearest].y},
						   5.0f, Fade((Color){180,0,255,255}, pulse*0.4f));
				/* Core beam */
				DrawLineEx((Vector2){g->enemies[ei].x, g->enemies[ei].y},
						   (Vector2){(float)g->towers[nearest].x,(float)g->towers[nearest].y},
						   2.0f, Fade((Color){220,80,255,255}, pulse*0.9f));
				/* Impact glow on tower */
				DrawCircle(g->towers[nearest].x, g->towers[nearest].y,
						   6.0f*pulse, Fade((Color){200,60,255,255}, 0.5f));
			}
		}
	}
	
	/* Narrative system (top-most render layer)*/
	narr_draw(&g->narr);
}
