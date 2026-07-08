#include "tui.h"
#include "graph/graph.h"
#include "random.h"
#include "output.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

/* ══════════════════════════════════════════════════════════
 * Terminal control (ANSI escape codes)
 * ══════════════════════════════════════════════════════════ */

static void term_clear(void) {
    printf("\033[2J\033[H");
}

void term_goto(int row, int col) {
    printf("\033[%d;%dH", row + 1, col + 1);
}

static void term_hide_cursor(void) {
    printf("\033[?25l");
}

static void term_show_cursor(void) {
    printf("\033[?25h");
}

static void term_flush(void) {
    fflush(stdout);
}

#ifdef _WIN32
static void term_enable_vt(void) {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}
#endif

void tui_init(TuiState *state) {
#ifdef _WIN32
    term_enable_vt();
#endif
    term_clear();
    term_hide_cursor();

    /* set defaults */
    state->dist         = DIST_UNIFORM;
    state->count        = 1;
    state->min_val      = 0;
    state->max_val      = 100;
    state->mean         = 0.0;
    state->stddev       = 1.0;
    state->prob         = 0.5;
    state->result_count = 0;
    state->result_scroll= 0;
    state->focus_field  = FIELD_COUNT_NUM;
    state->running      = 1;
    state->graph_dirty  = 1;  /* first render */
    state->status[0]    = '\0';

    /* init field buffers */
    snprintf(state->field_buf[FIELD_COUNT_NUM], FIELD_BUF_LEN, "%d", state->count);
    snprintf(state->field_buf[FIELD_MIN],     FIELD_BUF_LEN, "%d", state->min_val);
    snprintf(state->field_buf[FIELD_MAX],     FIELD_BUF_LEN, "%d", state->max_val);
    snprintf(state->field_buf[FIELD_MEAN],    FIELD_BUF_LEN, "%.1f", state->mean);
    snprintf(state->field_buf[FIELD_STDDEV],  FIELD_BUF_LEN, "%.1f", state->stddev);
    snprintf(state->field_buf[FIELD_PROB],    FIELD_BUF_LEN, "%.1f", state->prob);

    for (int i = 0; i < FIELD_COUNT; i++) {
        state->field_cursor[i] = (int)strlen(state->field_buf[i]);
    }

    /* init RNG */
    rng_init((unsigned int)time(NULL));

    /* clear results */
    memset(state->results, 0, sizeof(state->results));
}

void tui_restore(void) {
    term_clear();
    term_show_cursor();
    term_flush();
}

/* ══════════════════════════════════════════════════════════
 * Keyboard input
 * ══════════════════════════════════════════════════════════ */

#ifdef _WIN32
static int kb_hit(void)  { return _kbhit(); }
static int kb_get(void)  { return _getch(); }
#else
static int kb_hit(void) {
    struct termios oldt, newt;
    int ch;
    int oldf;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= (unsigned int)~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
    if (ch != EOF) { ungetc(ch, stdin); return 1; }
    return 0;
}
static int kb_get(void) {
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= (unsigned int)~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    int ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}
#endif

/*
 * Read a key. Returns:
 *   0-255  = ASCII character
 *   256+   = special key code
 */
#define KEY_UP      256
#define KEY_DOWN    257
#define KEY_LEFT    258
#define KEY_RIGHT   259
#define KEY_ESC     27

static int read_key(void) {
    int c = kb_get();
    if (c == 0 || c == 224) {  /* extended key prefix (Windows) */
        int c2 = kb_get();
        switch (c2) {
        case 72: return KEY_UP;
        case 80: return KEY_DOWN;
        case 75: return KEY_LEFT;
        case 77: return KEY_RIGHT;
        default: return c2;
        }
    }
    if (c == 27) {  /* ESC or ANSI escape sequence */
        if (kb_hit()) {
            int c2 = kb_get();
            if (c2 == '[') {
                int c3 = kb_get();
                switch (c3) {
                case 'A': return KEY_UP;
                case 'B': return KEY_DOWN;
                case 'C': return KEY_RIGHT;
                case 'D': return KEY_LEFT;
                default:  return c3;
                }
            }
        }
        return KEY_ESC;
    }
    if (c == '\r') c = '\n';  /* normalize Enter */
    return c;
}

/* ══════════════════════════════════════════════════════════
 * Screen rendering
 * ══════════════════════════════════════════════════════════ */

static void draw_box(void) {
    /* Top border with T-junctions at dividers */
    term_goto(0, 0);    putchar('+');
    for (int c = 1; c <= 78; c++) {
        if (c == DIV1 || c == DIV2) putchar('+');
        else                        putchar('-');
    }
    term_goto(0, 79);   putchar('+');

    /* Left/right borders + dividers */
    for (int r = 1; r <= TERM_H - 2; r++) {
        term_goto(r, 0);    putchar('|');
        term_goto(r, DIV1); putchar('|');
        term_goto(r, DIV2); putchar('|');
        term_goto(r, 79);   putchar('|');
    }

    /* Bottom border */
    term_goto(TERM_H - 1, 0);  putchar('+');
    for (int c = 1; c <= 78; c++) {
        if (c == DIV1 || c == DIV2) putchar('+');
        else                        putchar('-');
    }
    term_goto(TERM_H - 1, 79); putchar('+');
}

/* ══════════════════════════════════════════════════════════
 * Graph drawing
 * ══════════════════════════════════════════════════════════ */

/*
 * Draw the uniform distribution PDF: a flat rectangle from min to max.
 * The PDF value is 1/(max-min), drawn as a constant-height bar.
 */
/* ══════════════════════════════════════════════════════════
 * Left panel: controls + results
 * ══════════════════════════════════════════════════════════ */

static int field_is_visible(int fid, DistType dist) {
    switch (dist) {
    case DIST_UNIFORM:
        return fid == FIELD_COUNT_NUM || fid == FIELD_MIN || fid == FIELD_MAX;
    case DIST_NORMAL:
        return fid == FIELD_COUNT_NUM || fid == FIELD_MEAN || fid == FIELD_STDDEV;
    case DIST_BERNOULLI:
        return fid == FIELD_COUNT_NUM || fid == FIELD_PROB;
    }
    return 0;
}

static void draw_controls(TuiState *state) {
    int x = LEFT_X;
    int y = 3;

    /* Distribution selection (left-padded to LEFT_W) */
    term_goto(y, x);
    printf("%-*s", LEFT_W, "Distribution:");

    term_goto(y + 1, x + 2);
    if (state->dist == DIST_UNIFORM)
        printf("[*] Uniform      ");
    else
        printf("[ ] Uniform (U)  ");

    term_goto(y + 2, x + 2);
    if (state->dist == DIST_NORMAL)
        printf("[*] Normal       ");
    else
        printf("[ ] Normal (N)   ");

    term_goto(y + 3, x + 2);
    if (state->dist == DIST_BERNOULLI)
        printf("[*] Bernoulli    ");
    else
        printf("[ ] Bernoulli (B)");

    /* Parameter fields — only show relevant ones for current distribution */
    static const char *labels[FIELD_COUNT] = {
        "Count:  ", "Min:    ", "Max:    ", "Mean:   ", "StdDev: ", "Prob:   "
    };

    int ry = y + 6;
    for (int i = 0; i < FIELD_COUNT; i++) {
        if (!field_is_visible(i, state->dist)) {
            /* Clear stale field text */
            term_goto(ry, x);
            printf("%-*s", LEFT_W, "");
            ry++;
            continue;
        }
        term_goto(ry, x);
        printf("%s", labels[i]);

        /* draw field: "[value]" at offset 10 */
        term_goto(ry, x + 10);
        printf("[");
        if (i == state->focus_field)
            printf("\033[7m");
        printf("%-10s", state->field_buf[i]);
        if (i == state->focus_field)
            printf("\033[0m");
        printf("]");

        /* pad rest of row — must be same row, not ry+1 */
        ry++;
    }

    /* Clear any remaining stale lines */
    for (; ry < y + 12; ry++) {
        term_goto(ry, x);
        printf("%-*s", LEFT_W, "");
    }

    /* Buttons */
    int by = y + 13;
    term_goto(by, x);
    printf("[ Generate ]  G");
    term_goto(by + 1, x);
    printf("[ Export   ]  E");
    term_goto(by + 2, x);
    printf("[ Quit     ]  Q");

    /* Clear any old cruft below buttons */
    for (int row = by + 3; row < TERM_H - 1; row++) {
        term_goto(row, x);
        printf("%-*s", LEFT_W, "");
    }
}

static void draw_results(const TuiState *state) {
    int x = RIGHT_X;
    int y = 3;
    int max_show = 10;

    /* Clear result area */
    for (int i = 0; i < 12; i++) {
        term_goto(y + i, x);
        printf("%-*s", RIGHT_W, "");
    }

    if (state->result_count == 0) {
        term_goto(y + 1, x);
        printf("(no results)");
        return;
    }

    int start = state->result_scroll;
    if (start > state->result_count - max_show) {
        start = state->result_count - max_show;
    }
    if (start < 0) start = 0;

    for (int i = 0; i < max_show && (start + i) < state->result_count; i++) {
        term_goto(y + i + 1, x);
        printf("%3d: %g", start + i + 1, state->results[start + i]);
    }

    if (state->result_count > max_show) {
        term_goto(y + max_show + 2, x);
        printf("(%d total, arrows)", state->result_count);
    }
}

/* ══════════════════════════════════════════════════════════
 * Full frame
 * ══════════════════════════════════════════════════════════ */

static void draw_frame(TuiState *state) {
    /* Outer box with T-junctions at dividers */
    draw_box();

    /* Title (centered on full width) */
    term_goto(0, LEFT_X + 2);
    printf("Random Number Generator");

    /* Column headers */
    term_goto(2, MID_X + 2);
    printf("Distribution Graph");
    term_goto(2, RIGHT_X + 2);
    printf("Results");

    /* Panels */
    draw_controls(state);
    draw_results(state);

    /* Graph: only when dirty */
    if (state->graph_dirty) {
        draw_graph(state);
        state->graph_dirty = 0;
    }

    /* Status message — last content row, full width */
    term_goto(STATUS_Y, LEFT_X);
    if (state->status[0] != '\0') {
        printf("\033[1m%-78s\033[0m", state->status);
    } else {
        printf("%-78s", "");
    }

    term_flush();
}

/* ══════════════════════════════════════════════════════════
 * Input field editing
 * ══════════════════════════════════════════════════════════ */

static void field_insert(FieldId fid, TuiState *state, char c) {
    char *buf = state->field_buf[fid];
    int *cur  = &state->field_cursor[fid];
    int len   = (int)strlen(buf);

    if (len >= FIELD_BUF_LEN - 1) return;

    /* shift right */
    for (int i = len; i >= *cur; i--) {
        buf[i + 1] = buf[i];
    }
    buf[*cur] = c;
    (*cur)++;
}

static void field_backspace(FieldId fid, TuiState *state) {
    char *buf = state->field_buf[fid];
    int *cur  = &state->field_cursor[fid];
    int len   = (int)strlen(buf);

    if (*cur <= 0) return;

    /* shift left */
    for (int i = *cur - 1; i < len; i++) {
        buf[i] = buf[i + 1];
    }
    (*cur)--;
}

static void parse_fields(TuiState *state) {
    state->count  = (int)strtol(state->field_buf[FIELD_COUNT_NUM], NULL, 10);
    state->min_val= (int)strtol(state->field_buf[FIELD_MIN],     NULL, 10);
    state->max_val= (int)strtol(state->field_buf[FIELD_MAX],     NULL, 10);
    state->mean   = strtod(state->field_buf[FIELD_MEAN],   NULL);
    state->stddev = strtod(state->field_buf[FIELD_STDDEV], NULL);
    state->prob   = strtod(state->field_buf[FIELD_PROB],   NULL);

    /* clamp */
    if (state->count < 1)  state->count = 1;
    if (state->count > MAX_RESULTS) state->count = MAX_RESULTS;
    if (state->prob < 0.0) state->prob = 0.0;
    if (state->prob > 1.0) state->prob = 1.0;
    if (state->stddev <= 0.0) state->stddev = 1.0;
}

/* ══════════════════════════════════════════════════════════
 * Actions
 * ══════════════════════════════════════════════════════════ */

static void do_generate(TuiState *state) {
    parse_fields(state);

    int n = state->count;
    if (n > MAX_RESULTS) n = MAX_RESULTS;

    state->result_count = n;
    state->result_scroll = 0;

    rng_init((unsigned int)time(NULL));

    if (state->dist == DIST_NORMAL) {
        for (int i = 0; i < n; i++) {
            state->results[i] = rng_normal(state->mean, state->stddev);
        }
    } else if (state->dist == DIST_BERNOULLI) {
        for (int i = 0; i < n; i++) {
            state->results[i] = (double)rng_bernoulli(state->prob);
        }
    } else {
        for (int i = 0; i < n; i++) {
            state->results[i] = (double)rng_uniform(state->min_val, state->max_val);
        }
    }

    snprintf(state->status, sizeof(state->status),
             "Gen %d %s numbers.",
             n,
             state->dist == DIST_NORMAL    ? "normal" :
             state->dist == DIST_BERNOULLI ? "bernoulli" :
                                             "uniform");
}

static void do_export(TuiState *state) {
    if (state->result_count == 0) {
        snprintf(state->status, sizeof(state->status),
                 "Nothing to export!");
        return;
    }

    /* Generate filename with timestamp */
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char fname[64];
    snprintf(fname, sizeof(fname), "rng_export_%04d%02d%02d_%02d%02d%02d.csv",
             t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
             t->tm_hour, t->tm_min, t->tm_sec);

    if (output_file(state->results, state->result_count, fname) == 0) {
        snprintf(state->status, sizeof(state->status),
                 "Exported %d to CSV.", state->result_count);
    } else {
        snprintf(state->status, sizeof(state->status),
                 "Export failed!");
    }
}

/* ══════════════════════════════════════════════════════════
 * Main event loop
 * ══════════════════════════════════════════════════════════ */

void tui_run(void) {
    TuiState state;
    tui_init(&state);

    while (state.running) {
        draw_frame(&state);

        /* wait for key */
        while (!kb_hit() && state.running) {
            /* small sleep to avoid busy-waiting */
#ifdef _WIN32
            Sleep(10);
#else
            usleep(10000);
#endif
        }

        if (!state.running) break;

        int key = read_key();

        switch (key) {
        case 'q':
        case 'Q':
        case KEY_ESC:
            state.running = 0;
            break;

        case 'u':
        case 'U':
            state.dist = DIST_UNIFORM;
            state.focus_field = FIELD_COUNT_NUM;
            state.graph_dirty = 1;
            snprintf(state.status, sizeof(state.status),
                     "Switched to Uniform.");
            break;

        case 'n':
        case 'N':
            state.dist = DIST_NORMAL;
            state.focus_field = FIELD_COUNT_NUM;
            state.graph_dirty = 1;
            snprintf(state.status, sizeof(state.status),
                     "Switched to Normal.");
            break;

        case 'b':
        case 'B':
            state.dist = DIST_BERNOULLI;
            state.focus_field = FIELD_COUNT_NUM;
            state.graph_dirty = 1;
            snprintf(state.status, sizeof(state.status),
                     "Switched to Bernoulli.");
            break;

        case 'g':
        case 'G':
            do_generate(&state);
            break;

        case 'e':
        case 'E':
            do_export(&state);
            break;

        case '\t':  /* Tab: cycle focus (skip invisible fields) */
            do {
                state.focus_field = (state.focus_field + 1) % FIELD_COUNT;
            } while (!field_is_visible(state.focus_field, state.dist));
            break;

        case KEY_UP:
            if (state.result_count > 0 && state.result_scroll > 0) {
                state.result_scroll--;
            }
            break;

        case KEY_DOWN:
            if (state.result_count > 0
                && state.result_scroll < state.result_count - 10) {
                state.result_scroll++;
            }
            break;

        case KEY_LEFT:
            if (state.field_cursor[state.focus_field] > 0) {
                state.field_cursor[state.focus_field]--;
            }
            break;

        case KEY_RIGHT: {
            int len = (int)strlen(state.field_buf[state.focus_field]);
            if (state.field_cursor[state.focus_field] < len) {
                state.field_cursor[state.focus_field]++;
            }
            break;
        }

        case '\b':
        case 127:  /* backspace (127 == 0x7f on some terminals) */
            field_backspace((FieldId)state.focus_field, &state);
            parse_fields(&state);
            if (field_is_visible(state.focus_field, state.dist)) {
                state.graph_dirty = 1;
            }
            break;

        default:
            /* Printable ASCII: digit, minus, dot */
            if ((key >= '0' && key <= '9') || key == '-' || key == '.') {
                field_insert((FieldId)state.focus_field, &state, (char)key);
                parse_fields(&state);
                if (field_is_visible(state.focus_field, state.dist)) {
                    state.graph_dirty = 1;
                }
            }
            break;
        }
    }

    tui_restore();
}

/* ══════════════════════════════════════════════════════════
 * Entry point for rng-tui
 * ══════════════════════════════════════════════════════════ */

int main(void) {
    tui_run();
    return 0;
}
