#ifndef NARRATIVE_H
#define NARRATIVE_H

#include "raylib.h"

#define NARR_MAX_LINES   8
#define NARR_LINE_LEN  128
#define NARR_QUEUE_SIZE  6

typedef enum {
	NARR_FLOAT,       /* 屏幕底部浮动短提示，不暂停游戏 */
	NARR_FULLSCREEN   /* 全屏逐行叙事，玩家按空格继续，游戏暂停 */
} NarrMode;

typedef struct {
	char     lines[NARR_MAX_LINES][NARR_LINE_LEN];
	int      line_count;
	NarrMode mode;
	float    display_time;   /* FLOAT 模式：持续秒数 */
	Color    text_color;
} NarrEvent;

typedef struct {
	NarrEvent queue[NARR_QUEUE_SIZE];
	int       head, tail, count;
	
	int       active;
	NarrEvent current;
	float     float_timer;   /* FLOAT 模式剩余时间 */
	int       current_line;  /* FULLSCREEN 模式当前行 */
	int       is_blocking;   /* 1=游戏逻辑暂停中 */
} NarrSystem;

void narr_init           (NarrSystem* n);
void narr_push_float     (NarrSystem* n, const char* text,
						  float duration, Color color);
/* 末尾传 NULL 结束：narr_push_fullscreen(n, WHITE, "line1", "line2", NULL) */
void narr_push_fullscreen(NarrSystem* n, Color color, ...);
int  narr_update         (NarrSystem* n, float dt);   /* 返回1=阻塞中 */
void narr_advance        (NarrSystem* n);             /* Space/Enter 推进 */
void narr_draw           (const NarrSystem* n);
int  narr_is_blocking    (const NarrSystem* n);

#endif
