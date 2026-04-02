#include "narrative.h"
#include <string.h>
#include <stdarg.h>

void narr_init(NarrSystem* n) {
	memset(n, 0, sizeof(NarrSystem));
}

static void narr_pop_next(NarrSystem* n) {
	if (n->count == 0) { n->active = 0; n->is_blocking = 0; return; }
	n->current      = n->queue[n->head];
	n->head         = (n->head + 1) % NARR_QUEUE_SIZE;
	n->count--;
	n->active       = 1;
	n->float_timer  = n->current.display_time;
	n->current_line = 0;
	n->is_blocking  = (n->current.mode == NARR_FULLSCREEN) ? 1 : 0;
}

void narr_push_float(NarrSystem* n, const char* text, float duration, Color color) {
	if (n->count >= NARR_QUEUE_SIZE) return;
	NarrEvent* ev = &n->queue[(n->head + n->count) % NARR_QUEUE_SIZE];
	memset(ev, 0, sizeof(NarrEvent));
	ev->mode         = NARR_FLOAT;
	ev->display_time = duration;
	ev->text_color   = color;
	ev->line_count   = 1;
	strncpy(ev->lines[0], text, NARR_LINE_LEN - 1);
	n->count++;
	if (!n->active) narr_pop_next(n);
}

void narr_push_fullscreen(NarrSystem* n, Color color, ...) {
	if (n->count >= NARR_QUEUE_SIZE) return;
	NarrEvent* ev = &n->queue[(n->head + n->count) % NARR_QUEUE_SIZE];
	memset(ev, 0, sizeof(NarrEvent));
	ev->mode       = NARR_FULLSCREEN;
	ev->text_color = color;
	
	va_list args;
	va_start(args, color);
	const char* line;
	while ((line = va_arg(args, const char*)) != NULL &&
		   ev->line_count < NARR_MAX_LINES) {
		strncpy(ev->lines[ev->line_count], line, NARR_LINE_LEN - 1);
		ev->line_count++;
	}
	va_end(args);
	
	n->count++;
	if (!n->active) narr_pop_next(n);
}

int narr_update(NarrSystem* n, float dt) {
	if (!n->active) return 0;
	if (n->current.mode == NARR_FLOAT) {
		n->float_timer -= dt;
		if (n->float_timer <= 0.0f) narr_pop_next(n);
	}
	return n->is_blocking;
}

void narr_advance(NarrSystem* n) {
	if (!n->active || n->current.mode != NARR_FULLSCREEN) return;
	n->current_line++;
	if (n->current_line >= n->current.line_count)
		narr_pop_next(n);
}

void narr_draw(const NarrSystem* n) {
	int i;
	if (!n->active) return;
	
	if (n->current.mode == NARR_FLOAT) {
		float alpha = (n->current.display_time > 0.0f)
		? (n->float_timer / n->current.display_time) : 1.0f;
		Color c = Fade(n->current.text_color, alpha);
		const char* txt = n->current.lines[0];
		int fw = MeasureText(txt, 22);
		DrawText(txt, 512 - fw/2 + 1, 705, 22, Fade(BLACK, alpha));
		DrawText(txt, 512 - fw/2,     703, 22, c);
	} else {
		DrawRectangle(0, 0, 1024, 768, (Color){0,0,0,170});
		int y = 280;
		for (i = 0; i <= n->current_line && i < n->current.line_count; i++) {
			int fw = MeasureText(n->current.lines[i], 26);
			DrawText(n->current.lines[i], 512-fw/2+1, y+1, 26, BLACK);
			DrawText(n->current.lines[i], 512-fw/2,   y,   26, n->current.text_color);
			y += 46;
		}
		const char* hint = (n->current_line < n->current.line_count - 1)
		? "[Space] Continue" : "[Space] Confirm";
		DrawText(hint, 512-MeasureText(hint,18)/2, 680, 18,
				 (Color){160,160,160,200});
	}
}

int narr_is_blocking(const NarrSystem* n) { return n->is_blocking; }
