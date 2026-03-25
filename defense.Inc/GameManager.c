#include "GameManager.h"
#include <stdio.h>
#include <math.h>

/* ================================================================
贴图加载/卸载
================================================================ */
void GameManager_loadAllTextures(GameManager* gm) {
	gm->bgMapTexture1    = LoadTexture("background(1).png");
	gm->bgMapTexture2    = LoadTexture("background(2).png");
	gm->bgMapTexture3    = LoadTexture("background(3).png");
	gm->skeletonTexture  = LoadTexture("Skeleton.png");
	gm->slingTexture     = LoadTexture("sling.png");
	gm->bonespearTexture = LoadTexture("bonespear.png");
	// 13种塔贴图 [type索引 1-13]
	gm->towerTextures[1]  = LoadTexture("sling.png");
	gm->towerTextures[2]  = LoadTexture("bonespear.png");
	gm->towerTextures[3]  = LoadTexture("totem.png");
	gm->towerTextures[4]  = LoadTexture("fire.png");
	gm->towerTextures[5]  = LoadTexture("crossbow.png");
	gm->towerTextures[6]  = LoadTexture("mine.png");
	gm->towerTextures[7]  = LoadTexture("cauldron.png");
	gm->towerTextures[8]  = LoadTexture("firecrow.png");
	gm->towerTextures[9]  = LoadTexture("pulse.png");
	gm->towerTextures[10] = LoadTexture("gravity.png");
	gm->towerTextures[11] = LoadTexture("portal.png");
	gm->towerTextures[12] = LoadTexture("timeclock.png");
	gm->towerTextures[13] = LoadTexture("laser.png");
}
void GameManager_unloadAllTextures(GameManager* gm) {
	UnloadTexture(gm->bgMapTexture1);
	UnloadTexture(gm->bgMapTexture2);
	UnloadTexture(gm->bgMapTexture3);
	UnloadTexture(gm->skeletonTexture);
	UnloadTexture(gm->slingTexture);
	UnloadTexture(gm->bonespearTexture);
	{ int _i; for(_i=1;_i<=13;_i++) UnloadTexture(gm->towerTextures[_i]); }
}

/* ================================================================
初始化
================================================================ */
void GameManager_init(GameManager* gm, int startLevel) {
	int i;
	Map_init(&gm->gameMap, startLevel);
	gm->enemyCount=0;
	gm->towerCount=0;
	gm->floatTextCount=0;
	gm->projectileCount=0;
	gm->selectedTowerIndex=-1;
	gm->baseSelected=0;
	gm->portalPendingIndex=-1;
	gm->burnZoneCount=0;
	for(i=0;i<MAX_TRAPS;i++) gm->traps[i].active=0;
	gm->wood=5000;
	gm->stone=5000;
	gm->metal=0;
	gm->material=0;
	gm->resourceTimer=0;
	gm->currentWave=0;
	gm->isWaveActive=0;
	gm->waveDelayTimer=3.0;
	gm->warningTimer=0;
	gm->currentSelectedTower=TOWER_SLING;
	if(startLevel==1){gm->currentState=PLAYING_LEVEL_1;gm->currentEra=ERA_STONE;}
	else if(startLevel==2){gm->currentState=PLAYING_LEVEL_2;gm->currentEra=ERA_ANCIENT;gm->metal=200;}
	else if(startLevel==3){gm->currentState=PLAYING_LEVEL_3;gm->currentEra=ERA_FUTURE;gm->material=1000;}
	GameManager_loadAllTextures(gm);
	if(startLevel==1)      gm->gameMap.bgImg=gm->bgMapTexture1;
	else if(startLevel==2) gm->gameMap.bgImg=gm->bgMapTexture2;
	else if(startLevel==3) gm->gameMap.bgImg=gm->bgMapTexture3;
	Point bp=gm->gameMap.waypoints[gm->gameMap.waypointCount-1];
	int era=(startLevel==1)?0:(startLevel==2)?1:2;
	Base_init(&gm->base,bp.x,bp.y,era);
	gm->baseHealth=gm->base.hp;
}

/* ================================================================
波次管理
================================================================ */
void GameManager_startNextWave(GameManager* gm) {
	gm->currentWave++;
	gm->enemiesSpawned=0;
	gm->isWaveActive=1;
	if(gm->currentWave==1){gm->enemiesToSpawn=5;gm->currentSpawnInterval=2.0;}
	else if(gm->currentWave==2){gm->enemiesToSpawn=10;gm->currentSpawnInterval=1.5;}
	else if(gm->currentWave==3){gm->enemiesToSpawn=15;gm->currentSpawnInterval=1.0;}
	else {
		gm->isWaveActive=0;
		if(gm->currentState==PLAYING_LEVEL_1){
			gm->currentState=PLAYING_LEVEL_2;gm->currentEra=ERA_ANCIENT;gm->currentWave=0;
			gm->towerCount=0;gm->enemyCount=0;Map_init(&gm->gameMap,2);
			gm->gameMap.bgImg=gm->bgMapTexture2;
			Point bp=gm->gameMap.waypoints[gm->gameMap.waypointCount-1];
			Base_init(&gm->base,bp.x,bp.y,1);gm->baseHealth=gm->base.hp;
			gm->wood+=20000;gm->stone+=10000;gm->metal=20000;
			gm->currentSelectedTower=TOWER_CROSSBOW; // 进入古代文明默认选第一个塔
			gm->selectedTowerIndex=-1;gm->portalPendingIndex=-1;
		} else if(gm->currentState==PLAYING_LEVEL_2){
			gm->currentState=PLAYING_LEVEL_3;gm->currentEra=ERA_FUTURE;gm->currentWave=0;
			gm->towerCount=0;gm->enemyCount=0;Map_init(&gm->gameMap,3);
			gm->gameMap.bgImg=gm->bgMapTexture3;
			Point bp=gm->gameMap.waypoints[gm->gameMap.waypointCount-1];
			Base_init(&gm->base,bp.x,bp.y,2);gm->baseHealth=gm->base.hp;
			gm->material=100000;
			gm->currentSelectedTower=TOWER_PULSE; // 进入科幻未来默认选第一个塔
			gm->selectedTowerIndex=-1;gm->portalPendingIndex=-1;
		} else if(gm->currentState==PLAYING_LEVEL_3){
			gm->currentState=VICTORY;
		}
		return;
	}
	gm->spawnTimer=gm->currentSpawnInterval;
}

/* ================================================================
升级塔
================================================================ */
void GameManager_upgradeTower(GameManager* gm, int path) {
	if(gm->selectedTowerIndex<0||gm->selectedTowerIndex>=gm->towerCount) return;
	Tower* t=&gm->towers[gm->selectedTowerIndex];
	
	/* ---- 石器时代 ---- */
	if(t->type==TOWER_SLING){
		if(path==0){
			if(t->levelB>0){gm->warningTimer=1.0;return;}
			if(t->levelA==0){if(gm->wood<30){gm->warningTimer=1.0;return;}gm->wood-=30;t->levelA=1;t->pairedIndex=85;}
			else if(t->levelA==1){if(gm->wood<30){gm->warningTimer=1.0;return;}gm->wood-=30;t->levelA=2;t->damage=(int)(t->damage*1.5);t->range=(int)(t->range*1.2);}
		} else {
			if(t->levelA>0){gm->warningTimer=1.0;return;}
			if(t->levelB==0){if(gm->stone<40){gm->warningTimer=1.0;return;}gm->stone-=40;t->levelB=1;t->pairedIndex=100;t->cooldown=t->cooldown*10.0/13.0;}
		}
	} else if(t->type==TOWER_BONESPEAR){
		if(path==0){
			if(t->levelB>0){gm->warningTimer=1.0;return;}
			if(t->levelA==0){if(gm->wood<20){gm->warningTimer=1.0;return;}gm->wood-=20;t->levelA=1;}
			else if(t->levelA==1){if(gm->wood<30){gm->warningTimer=1.0;return;}gm->wood-=30;t->levelA=2;}
		} else {
			if(t->levelA>0){gm->warningTimer=1.0;return;}
			if(t->levelB==0){if(gm->wood<50){gm->warningTimer=1.0;return;}gm->wood-=50;t->levelB=1;}
		}
	} else if(t->type==TOWER_TOTEM){
		if(path==0){
			if(t->levelB>0){gm->warningTimer=1.0;return;}
			if(t->levelA==0){if(gm->wood<30){gm->warningTimer=1.0;return;}gm->wood-=30;t->levelA=1;}
		} else {
			if(t->levelA>0){gm->warningTimer=1.0;return;}
			if(t->levelB==0){if(gm->wood<40){gm->warningTimer=1.0;return;}gm->wood-=40;t->levelB=1;}
			else if(t->levelB==1){if(gm->wood<60){gm->warningTimer=1.0;return;}gm->wood-=60;t->levelB=2;}
		}
	} else if(t->type==TOWER_FIRE){
		if(path==0){
			if(t->levelB>0){gm->warningTimer=1.0;return;}
			if(t->levelA==0){if(gm->wood<100){gm->warningTimer=1.0;return;}gm->wood-=100;t->levelA=1;t->damage=(int)(t->damage*1.4);}
			else if(t->levelA==1){if(gm->wood<150){gm->warningTimer=1.0;return;}gm->wood-=150;t->levelA=2;}
		} else {
			if(t->levelA>0){gm->warningTimer=1.0;return;}
			if(t->levelB==0){if(gm->stone<50){gm->warningTimer=1.0;return;}gm->stone-=50;t->levelB=1;}
			else if(t->levelB==1){if(gm->stone<80){gm->warningTimer=1.0;return;}gm->stone-=80;t->levelB=2;}
		}
		/* ---- 古代文明 ---- */
	} else if(t->type==TOWER_CROSSBOW){
		if(path==0){
			if(t->levelB>0){gm->warningTimer=1.0;return;}
			if(t->levelA==0){if(gm->metal<40){gm->warningTimer=1.0;return;}gm->metal-=40;t->levelA=1;}
			else if(t->levelA==1){if(gm->metal<40){gm->warningTimer=1.0;return;}gm->metal-=40;t->levelA=2;}
		} else {
			if(t->levelA>0){gm->warningTimer=1.0;return;}
			if(t->levelB==0){if(gm->metal<60){gm->warningTimer=1.0;return;}gm->metal-=60;t->levelB=1;}
			else if(t->levelB==1){if(gm->metal<60){gm->warningTimer=1.0;return;}gm->metal-=60;t->levelB=2;}
		}
	} else if(t->type==TOWER_MINE){
		if(path==0){
			if(t->levelB>0){gm->warningTimer=1.0;return;}
			if(t->levelA==0){if(gm->metal<50){gm->warningTimer=1.0;return;}gm->metal-=50;t->levelA=1;}
			else if(t->levelA==1){if(gm->metal<80){gm->warningTimer=1.0;return;}gm->metal-=80;t->levelA=2;}
		} else {
			if(t->levelA>0){gm->warningTimer=1.0;return;}
			if(t->levelB==0){if(gm->metal<60){gm->warningTimer=1.0;return;}gm->metal-=60;t->levelB=1;}
			else if(t->levelB==1){if(gm->metal<80){gm->warningTimer=1.0;return;}gm->metal-=80;t->levelB=2;}
		}
	} else if(t->type==TOWER_CAULDRON){
		if(path==0){
			if(t->levelB>0){gm->warningTimer=1.0;return;}
			if(t->levelA==0){if(gm->metal<80){gm->warningTimer=1.0;return;}gm->metal-=80;t->levelA=1;}
			else if(t->levelA==1){if(gm->metal<100){gm->warningTimer=1.0;return;}gm->metal-=100;t->levelA=2;}
		} else {
			if(t->levelA>0){gm->warningTimer=1.0;return;}
			if(t->levelB==0){
				if(gm->metal<80){gm->warningTimer=1.0;return;}gm->metal-=80;t->levelB=1;
				t->explosiveCharges=3;t->explosiveDmgMult=1.0;
			} else if(t->levelB==1){if(gm->metal<100){gm->warningTimer=1.0;return;}gm->metal-=100;t->levelB=2;}
		}
	} else if(t->type==TOWER_FIRECROW){
		if(path==0){
			if(t->levelB>0){gm->warningTimer=1.0;return;}
			if(t->levelA==0){if(gm->metal<100){gm->warningTimer=1.0;return;}gm->metal-=100;t->levelA=1;}
		} else {
			if(t->levelA>0){gm->warningTimer=1.0;return;}
			if(t->levelB==0){if(gm->metal<80){gm->warningTimer=1.0;return;}gm->metal-=80;t->levelB=1;}
		}
		/* ---- 科幻未来 ---- */
	} else if(t->type==TOWER_PULSE){
		if(path==0){
			if(t->levelB>0){gm->warningTimer=1.0;return;}
			if(t->levelA==0){if(gm->material<180){gm->warningTimer=1.0;return;}gm->material-=180;t->levelA=1;}
			else if(t->levelA==1){if(gm->material<180){gm->warningTimer=1.0;return;}gm->material-=180;t->levelA=2;}
		} else {
			if(t->levelA>0){gm->warningTimer=1.0;return;}
			if(t->levelB==0){if(gm->material<80){gm->warningTimer=1.0;return;}gm->material-=80;t->levelB=1;}
			else if(t->levelB==1){if(gm->material<80){gm->warningTimer=1.0;return;}gm->material-=80;t->levelB=2;}
		}
	} else if(t->type==TOWER_GRAVITY){
		if(path==0){
			if(t->levelB>0){gm->warningTimer=1.0;return;}
			if(t->levelA==0){if(gm->material<150){gm->warningTimer=1.0;return;}gm->material-=150;t->levelA=1;}
			else if(t->levelA==1){if(gm->material<150){gm->warningTimer=1.0;return;}gm->material-=150;t->levelA=2;}
		} else {
			if(t->levelA>0){gm->warningTimer=1.0;return;}
			if(t->levelB==0){
				if(gm->material<150){gm->warningTimer=1.0;return;}
				gm->material-=150;t->levelB=1;
				t->currentCooldown=0;t->timer2=4.0; // 立即激活第一次反重力
			} else if(t->levelB==1){
				if(gm->material<200){gm->warningTimer=1.0;return;}
				gm->material-=200;t->levelB=2;
			}
		}
	} else if(t->type==TOWER_PORTAL){
		if(path==0){
			if(t->levelB>0){gm->warningTimer=1.0;return;}
			if(t->levelA==0){if(gm->material<200){gm->warningTimer=1.0;return;}gm->material-=200;t->levelA=1;if(t->pairedIndex>=0)gm->towers[t->pairedIndex].levelA=1;}
		} else {
			if(t->levelA>0){gm->warningTimer=1.0;return;}
			if(t->levelB==0){if(gm->material<200){gm->warningTimer=1.0;return;}gm->material-=200;t->levelB=1;if(t->pairedIndex>=0)gm->towers[t->pairedIndex].levelB=1;}
			else if(t->levelB==1){if(gm->material<200){gm->warningTimer=1.0;return;}gm->material-=200;t->levelB=2;if(t->pairedIndex>=0)gm->towers[t->pairedIndex].levelB=2;}
		}
	} else if(t->type==TOWER_TIMECLOCK){
		if(path==0){
			if(t->levelB>0){gm->warningTimer=1.0;return;}
			if(t->levelA==0){if(gm->material<100){gm->warningTimer=1.0;return;}gm->material-=100;t->levelA=1;}
			else if(t->levelA==1){if(gm->material<100){gm->warningTimer=1.0;return;}gm->material-=100;t->levelA=2;}
		} else {
			if(t->levelA>0){gm->warningTimer=1.0;return;}
			if(t->levelB==0){if(gm->material<100){gm->warningTimer=1.0;return;}gm->material-=100;t->levelB=1;}
		}
	} else if(t->type==TOWER_LASER){
		if(path==0){
			if(t->levelB>0){gm->warningTimer=1.0;return;}
			if(t->levelA==0){if(gm->material<150){gm->warningTimer=1.0;return;}gm->material-=150;t->levelA=1;}
			else if(t->levelA==1){if(gm->material<150){gm->warningTimer=1.0;return;}gm->material-=150;t->levelA=2;}
		} else {
			if(t->levelA>0){gm->warningTimer=1.0;return;}
			if(t->levelB==0){if(gm->material<70){gm->warningTimer=1.0;return;}gm->material-=70;t->levelB=1;}
			else if(t->levelB==1){if(gm->material<70){gm->warningTimer=1.0;return;}gm->material-=70;t->levelB=2;}
		}
	}
}

/* ================================================================
输入处理
================================================================ */
void GameManager_processInput(GameManager* gm) {
	int mx,my,col,row,i;
	Point center;
	if(gm->currentState!=PLAYING_LEVEL_1&&gm->currentState!=PLAYING_LEVEL_2&&gm->currentState!=PLAYING_LEVEL_3) return;
	
	if(IsKeyPressed(KEY_ESCAPE)){
		gm->selectedTowerIndex=-1;
		gm->baseSelected=0;
		/* 取消传送门：移除未配对的A门，还原格子 */
		if(gm->portalPendingIndex>=0){
			Tower* pa=&gm->towers[gm->portalPendingIndex];
			int col2=pa->x/CELL_SIZE, row2=pa->y/CELL_SIZE;
			if(row2>=0&&row2<ROWS&&col2>=0&&col2<COLS) gm->gameMap.grid[row2][col2]=TYPE_BUILDABLE;
			/* 从数组中移除：用最后一个塔覆盖 */
			gm->towers[gm->portalPendingIndex]=gm->towers[gm->towerCount-1];
			gm->towerCount--;
			gm->portalPendingIndex=-1;
		}
	}
	
	/* 快捷键选塔 */
	/* 快捷键：时代专属，1-4为当前时代防御塔，第三时代1-5 */
	if(gm->currentEra==ERA_STONE){
		if(IsKeyPressed(KEY_ONE))   gm->currentSelectedTower=TOWER_SLING;
		if(IsKeyPressed(KEY_TWO))   gm->currentSelectedTower=TOWER_BONESPEAR;
		if(IsKeyPressed(KEY_THREE)) gm->currentSelectedTower=TOWER_TOTEM;
		if(IsKeyPressed(KEY_FOUR))  gm->currentSelectedTower=TOWER_FIRE;
	} else if(gm->currentEra==ERA_ANCIENT){
		if(IsKeyPressed(KEY_ONE))   gm->currentSelectedTower=TOWER_CROSSBOW;
		if(IsKeyPressed(KEY_TWO))   gm->currentSelectedTower=TOWER_MINE;
		if(IsKeyPressed(KEY_THREE)) gm->currentSelectedTower=TOWER_CAULDRON;
		if(IsKeyPressed(KEY_FOUR))  gm->currentSelectedTower=TOWER_FIRECROW;
	} else if(gm->currentEra==ERA_FUTURE){
		if(IsKeyPressed(KEY_ONE))   gm->currentSelectedTower=TOWER_PULSE;
		if(IsKeyPressed(KEY_TWO))   gm->currentSelectedTower=TOWER_GRAVITY;
		if(IsKeyPressed(KEY_THREE)) gm->currentSelectedTower=TOWER_PORTAL;
		if(IsKeyPressed(KEY_FOUR))  gm->currentSelectedTower=TOWER_TIMECLOCK;
		if(IsKeyPressed(KEY_FIVE))  gm->currentSelectedTower=TOWER_LASER;
	}
	
	if(!IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) return;
	mx=GetMouseX();
	my=GetMouseY();
	
	/* ---- 升级面板按钮（塔）---- */
	if(gm->selectedTowerIndex>=0){
		Tower* sel=&gm->towers[gm->selectedTowerIndex];
		int pw=170,ph=115,px=sel->x+24,py=sel->y-60;
		if(px+pw>1024)px=sel->x-pw-8;
		if(py<65)py=65;
		if(py+ph>768)py=768-ph-4;
		Rectangle bA={px+5,py+44,pw-10,22},bB={px+5,py+72,pw-10,22};
		if(CheckCollisionPointRec((Vector2){mx,my},bA)){GameManager_upgradeTower(gm,0);return;}
		if(CheckCollisionPointRec((Vector2){mx,my},bB)){GameManager_upgradeTower(gm,1);return;}
		/* 重力塔A2引爆按钮 */
		if(sel->type==TOWER_GRAVITY&&sel->levelA>=2){
			Rectangle bEx={px+5,py+96,pw-10,16};
			if(CheckCollisionPointRec((Vector2){mx,my},bEx)){sel->gravityExplodeReady=1;return;}
		}
	}
	
	/* ---- 升级面板按钮（基地）---- */
	if(gm->baseSelected){
		int px=gm->base.x+50,py=gm->base.y-70,pw=175,ph=115;
		if(px+pw>1024)px=gm->base.x-pw-20;
		if(py<65)py=65;
		if(py+ph>768)py=768-ph-4;
		Rectangle bA={px+5,py+44,pw-10,22},bB={px+5,py+72,pw-10,22};
		if(CheckCollisionPointRec((Vector2){mx,my},bA)){int r=Base_upgrade(&gm->base,0,&gm->wood,&gm->stone,&gm->metal,&gm->material);if(r==1)gm->warningTimer=1.0;return;}
		if(CheckCollisionPointRec((Vector2){mx,my},bB)){int r=Base_upgrade(&gm->base,1,&gm->wood,&gm->stone,&gm->metal,&gm->material);if(r==1)gm->warningTimer=1.0;return;}
	}
	
	col=mx/CELL_SIZE;
	row=my/CELL_SIZE;
	
	/* ---- 点击基地 ---- */
	{ double ddx=mx-gm->base.x,ddy=my-gm->base.y;
		if(ddx*ddx+ddy*ddy<50*50){gm->baseSelected=!gm->baseSelected;gm->selectedTowerIndex=-1;return;} }
	
	/* ---- 点击已有塔（选中/升级面板）---- */
	for(i=0;i<gm->towerCount;i++){
		double ddx=gm->towers[i].x-(col*CELL_SIZE+CELL_SIZE/2.0);
		double ddy=gm->towers[i].y-(row*CELL_SIZE+CELL_SIZE/2.0);
		if(ddx*ddx+ddy*ddy<20*20){
			gm->selectedTowerIndex=(gm->selectedTowerIndex==i)?-1:i;
			gm->baseSelected=0;
			return;
		}
	}
	
	/* ---- 建造 ---- */
	gm->selectedTowerIndex=-1;
	gm->baseSelected=0;
	
	/* 传送门：第二步放置 */
	if(gm->currentSelectedTower==TOWER_PORTAL&&gm->portalPendingIndex>=0){
		if(row>=0&&row<ROWS&&col>=0&&col<COLS&&gm->gameMap.grid[row][col]==TYPE_PATH){
			center=Map_getCenter(row,col);
			Tower* a=&gm->towers[gm->portalPendingIndex];
			double ddx=center.x-a->x,ddy=center.y-a->y;
			double dist=sqrt(ddx*ddx+ddy*ddy);
			if(dist<3*CELL_SIZE||dist>15*CELL_SIZE){
				/* 距离不合法 */
				gm->warningTimer=1.0;
				gm->warningX=mx;
				gm->warningY=my;
			} else if(gm->material>=500&&gm->towerCount<MAX_TOWERS){
				gm->material-=500;
				Tower_init(&gm->towers[gm->towerCount],center.x,center.y,gm->currentEra,TOWER_PORTAL);
				gm->towers[gm->towerCount].portalRole=1;
				gm->towers[gm->towerCount].state=1;
				gm->towers[gm->towerCount].pairedIndex=gm->portalPendingIndex;
				a->pairedIndex=gm->towerCount;
				a->state=1;
				/* 传送门放在路径上，不改变格子类型 */
				gm->towerCount++;
				gm->portalPendingIndex=-1;
			} else { gm->warningTimer=1.0; gm->warningX=mx; gm->warningY=my; }
		}
		return;
	}
	
	if(row<0||row>=ROWS||col<0||col>=COLS) return;
	
	int requiredType=(gm->currentSelectedTower==TOWER_BONESPEAR||gm->currentSelectedTower==TOWER_MINE||gm->currentSelectedTower==TOWER_PORTAL)?TYPE_PATH:TYPE_BUILDABLE;
	if(gm->gameMap.grid[row][col]!=requiredType) return;
	
	/* 费用表 */
	int cw=0,cs=0,cm=0,cmat=0;
	switch(gm->currentSelectedTower){
		case TOWER_SLING:     cw=20;
		break;
		case TOWER_BONESPEAR: cw=20;cs=30;
		break;
		case TOWER_TOTEM:     cw=40;
		break;
		case TOWER_FIRE:                            break;
		case TOWER_CROSSBOW:  cw=50;
		break;
		case TOWER_MINE:      cm=100;
		break;
		case TOWER_CAULDRON:  cm=150;
		break;
		case TOWER_FIRECROW:  cs=200;cw=100;cm=120;
		break;
		case TOWER_PULSE:     cmat=420;
		break;
		case TOWER_GRAVITY:   cmat=375;
		break;
		case TOWER_PORTAL:    /* 费用在第二步扣 */   break;
		case TOWER_TIMECLOCK: cmat=600;
		break;
		case TOWER_LASER:     cmat=500;
		break;
	}
	if(gm->wood<cw||gm->stone<cs||gm->metal<cm||gm->material<cmat){
		gm->warningTimer=1.0;gm->warningX=mx;gm->warningY=my;return;
	}
	
	/* 时代限制：只能建当前时代的塔 */
	{
		int validForEra=0;
		if(gm->currentEra==ERA_STONE   &&gm->currentSelectedTower>=TOWER_SLING   &&gm->currentSelectedTower<=TOWER_FIRE)     validForEra=1;
		if(gm->currentEra==ERA_ANCIENT &&gm->currentSelectedTower>=TOWER_CROSSBOW&&gm->currentSelectedTower<=TOWER_FIRECROW) validForEra=1;
		if(gm->currentEra==ERA_FUTURE  &&gm->currentSelectedTower>=TOWER_PULSE   &&gm->currentSelectedTower<=TOWER_LASER)    validForEra=1;
		if(!validForEra){gm->warningTimer=1.0;gm->warningX=mx;gm->warningY=my;return;}
	}
	/* 传送门第一步 */
	if(gm->currentSelectedTower==TOWER_PORTAL){
		if(gm->towerCount>=MAX_TOWERS) return;
		center=Map_getCenter(row,col);
		int occ=0;
		for(i=0;i<gm->towerCount;i++){
			double d2=(gm->towers[i].x-center.x)*(gm->towers[i].x-center.x)+(gm->towers[i].y-center.y)*(gm->towers[i].y-center.y);
			if(d2<100){occ=1;break;}
		}
		if(occ) return;
		Tower_init(&gm->towers[gm->towerCount],center.x,center.y,gm->currentEra,TOWER_PORTAL);
		gm->towers[gm->towerCount].portalRole=0;
		gm->towers[gm->towerCount].state=0;
		/* 传送门放在路径上，不改变格子类型 */
		gm->portalPendingIndex=gm->towerCount;
		gm->towerCount++;
		return;
	}
	
	center=Map_getCenter(row,col);
	int occ=0;int replaceIdx=-1;
	for(i=0;i<gm->towerCount;i++){
		double d2=(gm->towers[i].x-center.x)*(gm->towers[i].x-center.x)+(gm->towers[i].y-center.y)*(gm->towers[i].y-center.y);
		if(d2<100){
			if(gm->towers[i].type==TOWER_BONESPEAR&&gm->towers[i].ammo<=0) replaceIdx=i;
			else occ=1;
		}
	}
	if(occ) return;
	if(gm->towerCount>=MAX_TOWERS) return;
	
	/* 古代A1：免费建造机会 */
	{
		int isFree = (gm->currentEra==ERA_ANCIENT && Base_consumeFreeBuild(&gm->base));
		if(!isFree){ gm->wood-=cw; gm->stone-=cs; gm->metal-=cm; gm->material-=cmat; }
	}
	if(gm->currentSelectedTower!=TOWER_BONESPEAR&&gm->currentSelectedTower!=TOWER_MINE)
		gm->gameMap.grid[row][col]=TYPE_OBSTACLE;
	if(replaceIdx>=0) Tower_init(&gm->towers[replaceIdx],center.x,center.y,gm->currentEra,gm->currentSelectedTower);
	else{Tower_init(&gm->towers[gm->towerCount],center.x,center.y,gm->currentEra,gm->currentSelectedTower);gm->towerCount++;}
}

/* ================================================================
逻辑更新
================================================================ */
void GameManager_updateLogic(GameManager* gm, double dt) {
	int i,j;
	double dx,dy;
	if(gm->currentState==GAME_OVER||gm->currentState==VICTORY||gm->currentState==MENU||gm->currentState==LEVEL_TRANSITION) return;
	if(gm->base.hp<=0){gm->currentState=GAME_OVER;return;}
	gm->baseHealth=gm->base.hp;
	if(gm->warningTimer>0) gm->warningTimer-=dt;
	
	/* 资源增长 */
	double rm=(gm->currentEra==ERA_STONE&&gm->base.levelA>=1)?1.5:1.0;
	gm->resourceTimer+=dt;
	if(gm->currentEra==ERA_STONE){if(gm->resourceTimer>=5.0){gm->wood+=(int)(3*rm);gm->stone+=(int)(3*rm);gm->resourceTimer-=5.0;}}
	else if(gm->currentEra==ERA_ANCIENT){if(gm->resourceTimer>=1.0){gm->wood+=2;gm->stone+=2;gm->metal+=2;gm->resourceTimer-=1.0;}}
	else if(gm->currentEra==ERA_FUTURE){if(gm->resourceTimer>=1.0){gm->material+=7;gm->resourceTimer-=1.0;}}
	
	/* 刷怪 */
	if(gm->isWaveActive){
		if(gm->enemiesSpawned<gm->enemiesToSpawn){
			gm->spawnTimer-=dt;
			if(gm->spawnTimer<=0&&gm->enemyCount<MAX_ENEMIES){
				Enemy_init(&gm->enemies[gm->enemyCount],gm->gameMap.waypoints[0],gm->enemiesSpawned);
				gm->enemyCount++;gm->enemiesSpawned++;gm->spawnTimer=gm->currentSpawnInterval;
			}
		} else if(gm->enemyCount==0){gm->isWaveActive=0;gm->waveDelayTimer=5.0;}
	} else {gm->waveDelayTimer-=dt;if(gm->waveDelayTimer<=0)GameManager_startNextWave(gm);}
	
	/* 塔更新 */
	for(i=0;i<gm->towerCount;i++){
		if(gm->currentEra==ERA_STONE&&gm->base.levelA>=2&&gm->towers[i].buffSpeedRatio<1.3){
			gm->towers[i].buffSpeedRatio=1.3;gm->towers[i].buffTimer=0.5;
		}
		Tower_update(&gm->towers[i],dt,gm->enemies,gm->enemyCount,
					 gm->towers,gm->towerCount,gm->gameMap.waypoints,
					 gm->projectiles,&gm->projectileCount,
					 gm->floatTexts,&gm->floatTextCount,
					 &gm->wood,&gm->stone,&gm->metal,&gm->material,
					 gm->burnZones,&gm->burnZoneCount);
	}
	
	Base_update(&gm->base,dt,gm->enemies,gm->enemyCount,gm->floatTexts,&gm->floatTextCount,gm->projectiles,&gm->projectileCount);
	/* BurnZone 燃烧区域更新 */
	{
		int bi, ei;
		double bdx, bdy;
		for (bi = 0; bi < gm->burnZoneCount; ) {
			gm->burnZones[bi].life -= dt;
			if (gm->burnZones[bi].life <= 0 || !gm->burnZones[bi].active) {
				gm->burnZones[bi] = gm->burnZones[gm->burnZoneCount-1];
				gm->burnZoneCount--;
				continue;
			}
			gm->burnZones[bi].tickTimer -= dt;
			if (gm->burnZones[bi].tickTimer <= 0) {
				gm->burnZones[bi].tickTimer = 1.0;
				for (ei = 0; ei < gm->enemyCount; ei++) {
					if (!gm->enemies[ei].active || gm->enemies[ei].hp <= 0) continue;
					bdx = gm->enemies[ei].x - gm->burnZones[bi].x;
					bdy = gm->enemies[ei].y - gm->burnZones[bi].y;
					if (bdx*bdx + bdy*bdy <= 32.0*32.0) {
						gm->enemies[ei].hp -= gm->burnZones[bi].damage;
						gm->enemies[ei].hitFlashTimer = 0.1;
						if (gm->enemies[ei].causeMarkActive)
							gm->enemies[ei].causeDamageAccum += gm->burnZones[bi].damage;
					}
				}
			}
			bi++;
		}
	}
	
	
	/* 敌人更新 */
	for(i=0;i<gm->enemyCount;){
		Enemy_update(&gm->enemies[i],dt,gm->gameMap.waypoints,gm->gameMap.waypointCount);
		if(gm->enemies[i].reachedBase||gm->enemies[i].hp<=0){
			if(gm->enemies[i].reachedBase){
				/* 古代A2：陷阱盾拦截 */
				if(Base_consumeTrapShield(&gm->base)){
					/* 本次入侵被拦截，不扣血 */
				} else if(gm->base.invincible){
					/* 未来A1：无敌中，不扣血 */
				} else {
					gm->base.hp--;
					gm->baseHealth=gm->base.hp;
					/* 未来A1：最后一滴血触发无敌 */
					Base_tryInvincible(&gm->base);
				}
			}
			gm->enemies[i]=gm->enemies[gm->enemyCount-1];gm->enemyCount--;
		} else i++;
	}
	
	/* 子弹结算 */
	for(i=0;i<gm->projectileCount;){
		gm->projectiles[i].life-=dt;
		if(gm->projectiles[i].life<=0){
			if(!gm->projectiles[i].isHit){
				for(j=0;j<MAX_TRAPS;j++){
					if(!gm->traps[j].active){gm->traps[j].x=gm->projectiles[i].targetX;gm->traps[j].y=gm->projectiles[i].targetY;gm->traps[j].life=5.0;gm->traps[j].active=1;break;}
				}
			} else if(gm->projectiles[i].damage>0){
				for(j=0;j<gm->enemyCount;j++){
					if(gm->enemies[j].id==gm->projectiles[i].targetId&&gm->enemies[j].active&&gm->enemies[j].hp>0){
						gm->enemies[j].hp-=gm->projectiles[i].damage;
						gm->enemies[j].hitFlashTimer=0.1;
						if(gm->enemies[j].causeMarkActive) gm->enemies[j].causeDamageAccum+=gm->projectiles[i].damage;
						if(gm->floatTextCount<MAX_FLOAT_TEXTS){
							gm->floatTexts[gm->floatTextCount].x=gm->enemies[j].x;
							gm->floatTexts[gm->floatTextCount].y=gm->enemies[j].y-20;
							gm->floatTexts[gm->floatTextCount].damage=gm->projectiles[i].damage;
							gm->floatTexts[gm->floatTextCount].life=0.8;gm->floatTextCount++;
						}
						break;
					}
				}
			}
			gm->projectiles[i]=gm->projectiles[gm->projectileCount-1];gm->projectileCount--;
		} else i++;
	}
	
	/* 绊脚石陷阱 */
	for(i=0;i<MAX_TRAPS;i++){
		if(!gm->traps[i].active) continue;
		gm->traps[i].life-=dt;
		if(gm->traps[i].life<=0){gm->traps[i].active=0;continue;}
		for(j=0;j<gm->enemyCount;j++){
			if(!gm->enemies[j].active||gm->enemies[j].stunTimer>0) continue;
			dx=gm->enemies[j].x-gm->traps[i].x;dy=gm->enemies[j].y-gm->traps[i].y;
			if(dx*dx+dy*dy<15*15){
				gm->enemies[j].stunTimer=1.0;gm->traps[i].active=0;
				if(gm->floatTextCount<MAX_FLOAT_TEXTS){gm->floatTexts[gm->floatTextCount].x=gm->enemies[j].x;gm->floatTexts[gm->floatTextCount].y=gm->enemies[j].y-20;gm->floatTexts[gm->floatTextCount].damage=-2;gm->floatTexts[gm->floatTextCount].life=1.0;gm->floatTextCount++;}
				break;
			}
		}
	}
	
	/* 飘字 */
	for(i=0;i<gm->floatTextCount;){
		gm->floatTexts[i].life-=dt;gm->floatTexts[i].y-=30.0*dt;
		if(gm->floatTexts[i].life<=0){gm->floatTexts[i]=gm->floatTexts[gm->floatTextCount-1];gm->floatTextCount--;}
		else i++;
	}
}

/* ================================================================
升级面板辅助
================================================================ */
static void GetUpgradeLabel(Tower* t,int path,const char** lbl,int* aff,int wood,int stone,int metal,int material){
	*lbl="";*aff=0;
#define CHK(cost,res,str) {*lbl=str;*aff=(res>=cost);}
	if(t->type==TOWER_SLING){
		if(path==0){if(t->levelA==0)CHK(30,wood,"A1:Hit85% -30W") else if(t->levelA==1)CHK(30,wood,"A2:BigRock -30W") else *lbl="A:MAX";}
			else{if(t->levelB==0&&t->levelA==0)CHK(40,stone,"B1:Hit100% -40S") else *lbl="B:MAX/MutexA";}
	} else if(t->type==TOWER_BONESPEAR){
		if(path==0){if(t->levelA==0)CHK(20,wood,"A1:Slow -20W") else if(t->levelA==1)CHK(30,wood,"A2:Bleed -30W") else *lbl="A:MAX";}
			else{if(t->levelB==0)CHK(50,wood,"B1:Triple -50W") else *lbl="B:MAX";}
	} else if(t->type==TOWER_TOTEM){
		if(path==0){if(t->levelA==0)CHK(30,wood,"A1:Buff -30W") else *lbl="A:MAX";}
		else{if(t->levelB==0)CHK(40,wood,"B1:Stun5s -40W") else if(t->levelB==1)CHK(60,wood,"B2:AoeDmg -60W") else *lbl="B:MAX";}
	} else if(t->type==TOWER_FIRE){
		if(path==0){if(t->levelA==0)CHK(100,wood,"A1:+40% -100W") else if(t->levelA==1)CHK(150,wood,"A2:Eternal -150W") else *lbl="A:MAX";}
			else{if(t->levelB==0)CHK(50,stone,"B1:Spray -50S") else if(t->levelB==1)CHK(80,stone,"B2:Wall -80S") else *lbl="B:MAX";}
	} else if(t->type==TOWER_CROSSBOW){
		if(path==0){if(t->levelA==0)CHK(40,metal,"A1:Pierce -40M") else if(t->levelA==1)CHK(40,metal,"A2:DoT -40M") else *lbl="A:MAX";}
			else{if(t->levelB==0)CHK(60,metal,"B1:3Shot -60M") else if(t->levelB==1)CHK(60,metal,"B2:x5 -60M") else *lbl="B:MAX";}
	} else if(t->type==TOWER_MINE){
		if(path==0){if(t->levelA==0)CHK(50,metal,"A1:+70%R -50M") else if(t->levelA==1)CHK(80,metal,"A2:2Boom -80M") else *lbl="A:MAX";}
			else{if(t->levelB==0)CHK(60,metal,"B1:AutoFill -60M") else if(t->levelB==1)CHK(80,metal,"B2:Chain -80M") else *lbl="B:MAX";}
	} else if(t->type==TOWER_CAULDRON){
		if(path==0){if(t->levelA==0)CHK(80,metal,"A1:Poison -80M") else if(t->levelA==1)CHK(100,metal,"A2:ToxFog -100M") else *lbl="A:MAX";}
			else{if(t->levelB==0)CHK(80,metal,"B1:Explosive -80M") else if(t->levelB==1)CHK(100,metal,"B2:Nitro -100M") else *lbl="B:MAX";}
	} else if(t->type==TOWER_FIRECROW){
		if(path==0){if(t->levelA==0)CHK(100,metal,"A1:FastFire -100M") else *lbl="A:MAX";}
		else{if(t->levelB==0)CHK(80,metal,"B1:CrowBomb -80M") else *lbl="B:MAX";}
	} else if(t->type==TOWER_PULSE){
		if(path==0){if(t->levelA==0)CHK(180,material,"A1:NoDecay -180") else if(t->levelA==1)CHK(180,material,"A2:Stun -180") else *lbl="A:MAX";}
			else{if(t->levelB==0)CHK(80,material,"B1:Charge -80") else if(t->levelB==1)CHK(80,material,"B2:Slice -80") else *lbl="B:MAX";}
	} else if(t->type==TOWER_GRAVITY){
		if(path==0){if(t->levelA==0)CHK(150,material,"A1:+30%R -150") else if(t->levelA==1)CHK(150,material,"A2:Explode -150") else *lbl="A:MAX";}
			else{if(t->levelB==0)CHK(150,material,"B1:AntiGrav -150") else if(t->levelB==1)CHK(200,material,"B2:BlackHole -200") else *lbl="B:MAX";}
	} else if(t->type==TOWER_PORTAL){
		if(path==0){if(t->levelA==0)CHK(200,material,"A1:Cut240 -200") else *lbl="A:MAX";}
		else{if(t->levelB==0)CHK(200,material,"B1:2xTele -200") else if(t->levelB==1)CHK(200,material,"B2:Collapse -200") else *lbl="B:MAX";}
	} else if(t->type==TOWER_TIMECLOCK){
		if(path==0){if(t->levelA==0)CHK(100,material,"A1:Freeze -100") else if(t->levelA==1)CHK(100,material,"A2:Rewind -100") else *lbl="A:MAX";}
			else{if(t->levelB==0)CHK(100,material,"B1:Causality -100") else *lbl="B:MAX";}
	} else if(t->type==TOWER_LASER){
		if(path==0){if(t->levelA==0)CHK(150,material,"A1:Store -150") else if(t->levelA==1)CHK(150,material,"A2:Pulse800 -150") else *lbl="A:MAX";}
			else{if(t->levelB==0)CHK(70,material,"B1:FemtoLaser -70") else if(t->levelB==1)CHK(70,material,"B2:Chain -70") else *lbl="B:MAX";}
	}
#undef CHK
	// 互斥后置：已走对面路径则本路径强制置灰
	if(path==0 && t->levelB>0) { *aff=0; if(strcmp(*lbl,"A:MAX")!=0) *lbl="[Locked]"; }
	if(path==1 && t->levelA>0) { *aff=0; if(strcmp(*lbl,"B:MAX")!=0) *lbl="[Locked]"; }
}

static void DrawUpgradePanel(Tower* t,int px,int py,int pw,int ph,int wood,int stone,int metal,int mat){
	DrawRectangle(px,py,pw,ph,(Color){20,20,20,220});
	DrawRectangleLines(px,py,pw,ph,GOLD);
	const char* names[]={"","Sling","BoneSpear","Totem","Fire","Crossbow","Mine","Cauldron","FireCrow","Pulse","Gravity","Portal","TimeClock","Laser"};
	DrawText(names[t->type],px+5,py+5,13,GOLD);
	DrawText(TextFormat("LvA:%d LvB:%d",t->levelA,t->levelB),px+5,py+22,11,LIGHTGRAY);
	const char* lA;int afA; GetUpgradeLabel(t,0,&lA,&afA,wood,stone,metal,mat);
	Color cA=afA?GREEN:DARKGRAY;
	DrawRectangle(px+5,py+44,pw-10,22,(Color){40,40,40,255});DrawRectangleLines(px+5,py+44,pw-10,22,cA);DrawText(lA,px+8,py+49,10,cA);
	const char* lB;int afB; GetUpgradeLabel(t,1,&lB,&afB,wood,stone,metal,mat);
	Color cB=afB?SKYBLUE:DARKGRAY;
	DrawRectangle(px+5,py+72,pw-10,22,(Color){40,40,40,255});DrawRectangleLines(px+5,py+72,pw-10,22,cB);DrawText(lB,px+8,py+77,10,cB);
	if(t->type==TOWER_GRAVITY&&t->levelA>=2){
		DrawRectangle(px+5,py+96,pw-10,16,(Color){60,20,20,255});DrawRectangleLines(px+5,py+96,pw-10,16,RED);DrawText("EXPLODE",px+8,py+100,10,RED);
	}
	DrawText("[ESC]Close",px+5,py+ph-12,9,(Color){100,100,100,255});
}

/* ================================================================
渲染
================================================================ */
void GameManager_renderGraphics(GameManager* gm) {
	int i,mx,my,hoverCol,hoverRow,floatY,reqType;
	Point c;
	double prog,cx,cy;
	const char *infoText,*dmgText;
	Color textColor;
	
	BeginDrawing();
	ClearBackground(BLACK);
	Map_draw(&gm->gameMap);
	
	mx=GetMouseX();my=GetMouseY();
	hoverCol=mx/CELL_SIZE;hoverRow=my/CELL_SIZE;
	
	/* 建造预览 */
	if(gm->selectedTowerIndex<0&&!gm->baseSelected&&hoverRow>=0&&hoverRow<ROWS&&hoverCol>=0&&hoverCol<COLS){
		reqType=(gm->currentSelectedTower==TOWER_BONESPEAR||gm->currentSelectedTower==TOWER_MINE||gm->currentSelectedTower==TOWER_PORTAL)?TYPE_PATH:TYPE_BUILDABLE;
		if(gm->gameMap.grid[hoverRow][hoverCol]==reqType){
			c=Map_getCenter(hoverRow,hoverCol);
			Color pc=WHITE;
			switch(gm->currentSelectedTower){
				case TOWER_SLING:    pc=LIGHTGRAY;break;
				case TOWER_BONESPEAR:pc=RED;break;
				case TOWER_TOTEM:    pc=PURPLE;break;
				case TOWER_FIRE:     pc=ORANGE;break;
				case TOWER_CROSSBOW: pc=(Color){160,110,50,255};break;
				case TOWER_MINE:     pc=(Color){100,100,100,255};break;
				case TOWER_CAULDRON: pc=(Color){180,140,50,255};break;
				case TOWER_FIRECROW: pc=(Color){220,180,40,255};break;
				case TOWER_PULSE:    pc=SKYBLUE;break;
				case TOWER_GRAVITY:  pc=(Color){80,80,160,255};break;
				case TOWER_PORTAL:   pc=VIOLET;break;
				case TOWER_TIMECLOCK:pc=(Color){150,150,220,255};break;
				case TOWER_LASER:    pc=RED;break;
			}
			DrawRectangle(c.x-14,c.y-14,28,28,Fade(pc,0.4f));
			DrawRectangleLines(c.x-14,c.y-14,28,28,pc);
		}
	}
	/* 传送门等待配对时显示连线预览 */
	if(gm->portalPendingIndex>=0){
		Tower* pa=&gm->towers[gm->portalPendingIndex];
		DrawLineEx((Vector2){(float)pa->x,(float)pa->y},(Vector2){(float)mx,(float)my},1,Fade(VIOLET,0.5f));
		DrawText("Place 2nd Portal (8-15 cells away)",10,700,14,VIOLET);
	}
	
	/* 选中高亮 */
	if(gm->selectedTowerIndex>=0){
		Tower* s=&gm->towers[gm->selectedTowerIndex];
		DrawRectangleLines(s->x-16,s->y-16,32,32,YELLOW);
	}
	if(gm->baseSelected){
		DrawCircleLines(gm->base.x,gm->base.y,52,YELLOW);
	}
	
	/* 激光光束（绘制在塔前面，敌人下面）*/
	for(i=0;i<gm->towerCount;i++) Tower_drawLaserBeam(&gm->towers[i],gm->enemies,gm->enemyCount);
	
	for(i=0;i<gm->towerCount;i++) Tower_draw(&gm->towers[i],gm->towerTextures);
	for(i=0;i<gm->enemyCount;i++) Enemy_draw(&gm->enemies[i],gm->skeletonTexture);
	Base_draw(&gm->base);
	
	/* 陷阱 */
	for(i=0;i<MAX_TRAPS;i++){if(gm->traps[i].active){DrawCircle(gm->traps[i].x,gm->traps[i].y,4,GRAY);DrawCircleLines(gm->traps[i].x,gm->traps[i].y,4,DARKGRAY);}}
	
	/* 子弹 */
	for(i=0;i<gm->projectileCount;i++){
		prog=1.0-(gm->projectiles[i].life/gm->projectiles[i].maxLife);
		cx=gm->projectiles[i].startX+(gm->projectiles[i].targetX-gm->projectiles[i].startX)*prog;
		cy=gm->projectiles[i].startY+(gm->projectiles[i].targetY-gm->projectiles[i].startY)*prog;
		if(gm->projectiles[i].isCrow){
			cy-=120.0*sin(prog*3.14159);
			/* 飞鸦抛物线更高 */;
			DrawCircle((int)cx,(int)cy,6,(Color){220,180,40,255});
		} else {
			cy-=80.0*sin(prog*3.14159);
			DrawCircle((int)cx,(int)cy,5,(Color){180,180,180,255});
		}
	}
	
	/* 升级面板（塔）*/
	if(gm->selectedTowerIndex>=0&&gm->selectedTowerIndex<gm->towerCount){
		Tower* sel=&gm->towers[gm->selectedTowerIndex];
		int pw=170,ph=120,px=sel->x+24,py=sel->y-60;
		if(px+pw>1024)px=sel->x-pw-8;if(py<65)py=65;if(py+ph>768)py=768-ph-4;
		DrawUpgradePanel(sel,px,py,pw,ph,gm->wood,gm->stone,gm->metal,gm->material);
	}
	/* 升级面板（基地）*/
	if(gm->baseSelected){
		int pw=175,ph=115,px=gm->base.x+50,py=gm->base.y-70;
		if(px+pw>1024)px=gm->base.x-pw-20;if(py<65)py=65;if(py+ph>768)py=768-ph-4;
		DrawRectangle(px,py,pw,ph,(Color){10,10,40,230});DrawRectangleLines(px,py,pw,ph,GOLD);
		const char* bn=(gm->base.era==0)?"Base:FireShrine":(gm->base.era==1)?"Base:MechTower":"Base:Core";
		DrawText(bn,px+5,py+5,13,GOLD);
		DrawText(TextFormat("HP:%d/%d LvA:%d",gm->base.hp,gm->base.maxHp,gm->base.levelA),px+5,py+22,11,LIGHTGRAY);
		const char* lA;int afA;
		Base_getUpgradeLabel(&gm->base,0,&lA,&afA,gm->wood,gm->stone,gm->metal,gm->material);
		Color cA=afA?GREEN:DARKGRAY;
		DrawRectangle(px+5,py+44,pw-10,22,(Color){30,30,30,255});DrawRectangleLines(px+5,py+44,pw-10,22,cA);DrawText(lA,px+8,py+49,10,cA);
		const char* lB;int afB;
		Base_getUpgradeLabel(&gm->base,1,&lB,&afB,gm->wood,gm->stone,gm->metal,gm->material);
		Color cB=afB?SKYBLUE:DARKGRAY;
		DrawRectangle(px+5,py+72,pw-10,22,(Color){30,30,30,255});DrawRectangleLines(px+5,py+72,pw-10,22,cB);DrawText(lB,px+8,py+77,10,cB);
		DrawText("[ESC]Close",px+5,py+100,9,(Color){100,100,100,255});
	}
	
	/* 顶部UI */
	DrawRectangle(0,0,1024,60,(Color){30,30,30,255});
	DrawLine(0,60,1024,60,(Color){100,100,100,255});
	if(gm->currentEra==ERA_STONE) infoText=TextFormat("HP:%d  Wood:%d  Stone:%d  Wave:%d/3",gm->base.hp,gm->wood,gm->stone,gm->currentWave);
	else if(gm->currentEra==ERA_ANCIENT) infoText=TextFormat("HP:%d  Wood:%d  Stone:%d  Metal:%d  Wave:%d/3",gm->base.hp,gm->wood,gm->stone,gm->metal,gm->currentWave);
	else infoText=TextFormat("HP:%d  Material:%d  Wave:%d/3",gm->base.hp,gm->material,gm->currentWave);
	DrawText(infoText,12,10,20,BLACK);DrawText(infoText,10,8,20,GOLD);
	
	/* 选塔提示 */
	/* 选塔提示 */
	/* 时代专属提示 */
	{
		const char* selNames[]={"","Sling","BoneSpear","Totem","Fire","Crossbow","Mine","Cauldron","FireCrow","Pulse","Gravity","Portal","TimeClock","Laser"};
		const char* eraHint="";
		if(gm->currentEra==ERA_STONE)      eraHint="[1]Sling [2]Spear [3]Totem [4]Fire";
		else if(gm->currentEra==ERA_ANCIENT) eraHint="[1]Crossbow [2]Mine [3]Cauldron [4]FireCrow";
		else if(gm->currentEra==ERA_FUTURE)  eraHint="[1]Pulse [2]Gravity [3]Portal [4]TimeClock [5]Laser";
		DrawText(TextFormat("Sel:%s | %s | [ESC]Cancel",selNames[gm->currentSelectedTower],eraHint),10,38,12,RAYWHITE);
	}
	
	
	if(gm->warningTimer>0){
		floatY=gm->warningY-20-(int)((1.0-gm->warningTimer)*30);
		DrawText("Not Enough Resources!",gm->warningX-40+2,floatY+2,18,BLACK);
		DrawText("Not Enough Resources!",gm->warningX-40,floatY,18,RED);
	}
	
	/* 飘字 */
	for(i=0;i<gm->floatTextCount;i++){
		if(gm->floatTexts[i].damage==-1){dmgText="Miss!";textColor=LIGHTGRAY;}
		else if(gm->floatTexts[i].damage==-2){dmgText="Stun!";textColor=YELLOW;}
		else if(gm->floatTexts[i].damage==-3){dmgText="ROAR!";textColor=PURPLE;}
		else if(gm->floatTexts[i].damage==-4){dmgText="VOID";textColor=(Color){100,0,200,255};}
		else{dmgText=TextFormat("-%d",gm->floatTexts[i].damage);textColor=RED;}
		DrawText(dmgText,(int)gm->floatTexts[i].x+1,(int)gm->floatTexts[i].y+1,20,BLACK);
		DrawText(dmgText,(int)gm->floatTexts[i].x,(int)gm->floatTexts[i].y,20,textColor);
	}
	
	if(!gm->isWaveActive&&(gm->currentState==PLAYING_LEVEL_1||gm->currentState==PLAYING_LEVEL_2||gm->currentState==PLAYING_LEVEL_3)&&gm->currentWave<3)
		DrawText(TextFormat("Next Wave in: %.1f s",gm->waveDelayTimer),350,350,40,WHITE);
	if(gm->currentState==GAME_OVER){DrawText("GAME OVER",300,300,80,RED);DrawText("The base was destroyed...",310,400,28,LIGHTGRAY);}
	if(gm->currentState==VICTORY){DrawText("YOU WIN!",350,300,80,GREEN);DrawText("All ages conquered!",350,400,30,LIGHTGRAY);}
	
	EndDrawing();
}
