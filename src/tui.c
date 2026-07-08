#include "tui.h"
#include "term.h"
#include "controls.h"
#include "graph/graph.h"
#include "random.h"
#include "output.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ══════════════════════════════════════════════════════════
 * TUI lifecycle
 * ══════════════════════════════════════════════════════════ */

void tui_init(TuiState *state) {
#ifdef _WIN32
    term_enable_vt();
#endif
    term_clear();
    term_hide_cursor();

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
    state->graph_dirty  = 1;
    state->status[0]    = '\0';

    snprintf(state->field_buf[FIELD_COUNT_NUM], FIELD_BUF_LEN, "%d", state->count);
    snprintf(state->field_buf[FIELD_MIN],     FIELD_BUF_LEN, "%d", state->min_val);
    snprintf(state->field_buf[FIELD_MAX],     FIELD_BUF_LEN, "%d", state->max_val);
    snprintf(state->field_buf[FIELD_MEAN],    FIELD_BUF_LEN, "%.1f", state->mean);
    snprintf(state->field_buf[FIELD_STDDEV],  FIELD_BUF_LEN, "%.1f", state->stddev);
    snprintf(state->field_buf[FIELD_PROB],    FIELD_BUF_LEN, "%.1f", state->prob);

    for (int i = 0; i < FIELD_COUNT; i++)
        state->field_cursor[i] = (int)strlen(state->field_buf[i]);

    rng_init((unsigned int)time(NULL));
    memset(state->results, 0, sizeof(state->results));
}

void tui_restore(void) {
    term_clear();
    term_show_cursor();
    term_flush();
}

/* ══════════════════════════════════════════════════════════
 * Screen rendering
 * ══════════════════════════════════════════════════════════ */

static void draw_box(void) {
    term_goto(0, 0); putchar('+');
    for (int c = 1; c <= 78; c++) {
        if (c == DIV1 || c == DIV2) putchar('+');
        else                        putchar('-');
    }
    term_goto(0, 79); putchar('+');

    for (int r = 1; r <= TERM_H - 2; r++) {
        term_goto(r, 0);    putchar('|');
        term_goto(r, DIV1); putchar('|');
        term_goto(r, DIV2); putchar('|');
        term_goto(r, 79);   putchar('|');
    }

    term_goto(TERM_H - 1, 0); putchar('+');
    for (int c = 1; c <= 78; c++) {
        if (c == DIV1 || c == DIV2) putchar('+');
        else                        putchar('-');
    }
    term_goto(TERM_H - 1, 79); putchar('+');
}

static void draw_results(const TuiState *state) {
    int x = RIGHT_X;
    int y = 3;
    int max_show = 10;

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
    if (start > state->result_count - max_show)
        start = state->result_count - max_show;
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
    draw_box();

    term_goto(0, LEFT_X + 2);
    printf("Random Number Generator");
    term_goto(2, MID_X + 2);
    printf("Distribution Graph");
    term_goto(2, RIGHT_X + 2);
    printf("Results");

    draw_controls(state);
    draw_results(state);

    if (state->graph_dirty) {
        draw_graph(state);
        state->graph_dirty = 0;
    }

    term_goto(STATUS_Y, LEFT_X);
    if (state->status[0] != '\0')
        printf("\033[1m%-78s\033[0m", state->status);
    else
        printf("%-78s", "");

    term_flush();
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
        for (int i = 0; i < n; i++)
            state->results[i] = rng_normal(state->mean, state->stddev);
    } else if (state->dist == DIST_BERNOULLI) {
        for (int i = 0; i < n; i++)
            state->results[i] = (double)rng_bernoulli(state->prob);
    } else {
        for (int i = 0; i < n; i++)
            state->results[i] = (double)rng_uniform(state->min_val, state->max_val);
    }

    snprintf(state->status, sizeof(state->status), "Gen %d %s numbers.", n,
             state->dist == DIST_NORMAL    ? "normal" :
             state->dist == DIST_BERNOULLI ? "bernoulli" : "uniform");
}

static void do_export(TuiState *state) {
    if (state->result_count == 0) {
        snprintf(state->status, sizeof(state->status), "Nothing to export!");
        return;
    }

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char fname[64];
    snprintf(fname, sizeof(fname), "rng_export_%04d%02d%02d_%02d%02d%02d.csv",
             t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
             t->tm_hour, t->tm_min, t->tm_sec);

    if (output_file(state->results, state->result_count, fname) == 0)
        snprintf(state->status, sizeof(state->status), "Exported %d to CSV.", state->result_count);
    else
        snprintf(state->status, sizeof(state->status), "Export failed!");
}

/* ══════════════════════════════════════════════════════════
 * Main event loop
 * ══════════════════════════════════════════════════════════ */

void tui_run(void) {
    TuiState state;
    tui_init(&state);

    while (state.running) {
        draw_frame(&state);

        while (!kb_hit() && state.running)
            term_sleep_ms(10);
        if (!state.running) break;

        int key = read_key();

        switch (key) {
        case 'q': case 'Q': case KEY_ESC:
            state.running = 0; break;

        case 'u': case 'U':
            state.dist = DIST_UNIFORM;
            state.focus_field = FIELD_COUNT_NUM;
            state.graph_dirty = 1;
            snprintf(state.status, sizeof(state.status), "Switched to Uniform."); break;

        case 'n': case 'N':
            state.dist = DIST_NORMAL;
            state.focus_field = FIELD_COUNT_NUM;
            state.graph_dirty = 1;
            snprintf(state.status, sizeof(state.status), "Switched to Normal."); break;

        case 'b': case 'B':
            state.dist = DIST_BERNOULLI;
            state.focus_field = FIELD_COUNT_NUM;
            state.graph_dirty = 1;
            snprintf(state.status, sizeof(state.status), "Switched to Bernoulli."); break;

        case 'g': case 'G':
            do_generate(&state); break;

        case 'e': case 'E':
            do_export(&state); break;

        case '\t':
            do {
                state.focus_field = (state.focus_field + 1) % FIELD_COUNT;
            } while (!field_is_visible(state.focus_field, state.dist));
            break;

        case KEY_UP:
            if (state.result_count > 0 && state.result_scroll > 0)
                state.result_scroll--;
            break;

        case KEY_DOWN:
            if (state.result_count > 0 && state.result_scroll < state.result_count - 10)
                state.result_scroll++;
            break;

        case KEY_LEFT:
            if (state.field_cursor[state.focus_field] > 0)
                state.field_cursor[state.focus_field]--;
            break;

        case KEY_RIGHT: {
            int len = (int)strlen(state.field_buf[state.focus_field]);
            if (state.field_cursor[state.focus_field] < len)
                state.field_cursor[state.focus_field]++;
            break;
        }

        case '\b': case 127:
            field_backspace((FieldId)state.focus_field, &state);
            parse_fields(&state);
            if (field_is_visible(state.focus_field, state.dist))
                state.graph_dirty = 1;
            break;

        default:
            if ((key >= '0' && key <= '9') || key == '-' || key == '.') {
                field_insert((FieldId)state.focus_field, &state, (char)key);
                parse_fields(&state);
                if (field_is_visible(state.focus_field, state.dist))
                    state.graph_dirty = 1;
            }
            break;
        }
    }

    tui_restore();
}

int main(void) {
    tui_run();
    return 0;
}
