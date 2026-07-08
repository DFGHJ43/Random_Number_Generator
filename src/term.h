/*
 * term.h — Terminal control & keyboard input interface
 *
 * term_* : ANSI terminal ops  |  kb_* / read_key / KEY_* : keyboard input
 */

#ifndef TERMINAL_H
#define TERMINAL_H

/* ── Terminal control ─────────────────────────────────── */
void term_clear(void);
void term_goto(int row, int col);
void term_hide_cursor(void);
void term_show_cursor(void);
void term_flush(void);
void term_sleep_ms(int ms);
#ifdef _WIN32
void term_enable_vt(void);
#endif

/* ── Keyboard input ───────────────────────────────────── */
#define KEY_UP      256
#define KEY_DOWN    257
#define KEY_LEFT    258
#define KEY_RIGHT   259
#define KEY_ESC     27

int  kb_hit(void);
int  kb_get(void);
int  read_key(void);

#endif /* TERMINAL_H */
