/*
 * controls.c — Left panel rendering + field editing
 *
 * draw_controls: distribution selector + parameter fields + buttons
 * field_insert/backspace: character insert/delete in field buffers
 * parse_fields: field buffer text -> numeric values (strtol/strtod)
 * field_is_visible: whether a field is shown for current distribution
 */

#include "controls.h"
#include "term.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int field_is_visible(int fid, DistType dist) {
    switch (dist) {
    case DIST_UNIFORM:
        return fid == FIELD_COUNT_NUM || fid == FIELD_MIN || fid == FIELD_MAX;
    case DIST_NORMAL:
        return fid == FIELD_COUNT_NUM || fid == FIELD_MEAN || fid == FIELD_STDDEV;
    case DIST_BERNOULLI:
        return fid == FIELD_COUNT_NUM || fid == FIELD_PROB;
    case DIST_POISSON:
        return fid == FIELD_COUNT_NUM || fid == FIELD_LAMBDA;
    }
    return 0;
}

void field_insert(FieldId fid, TuiState *state, char c) {
    char *buf = state->field_buf[fid];
    int *cur  = &state->field_cursor[fid];
    int len   = (int)strlen(buf);

    if (len >= FIELD_BUF_LEN - 1) return;

    for (int i = len; i >= *cur; i--)
        buf[i + 1] = buf[i];
    buf[*cur] = c;
    (*cur)++;
}

void field_backspace(FieldId fid, TuiState *state) {
    char *buf = state->field_buf[fid];
    int *cur  = &state->field_cursor[fid];
    int len   = (int)strlen(buf);

    if (*cur <= 0) return;

    for (int i = *cur - 1; i < len; i++)
        buf[i] = buf[i + 1];
    (*cur)--;
}

void parse_fields(TuiState *state) {
    state->count  = (int)strtol(state->field_buf[FIELD_COUNT_NUM], NULL, 10);
    state->min_val= (int)strtol(state->field_buf[FIELD_MIN],     NULL, 10);
    state->max_val= (int)strtol(state->field_buf[FIELD_MAX],     NULL, 10);
    state->mean   = strtod(state->field_buf[FIELD_MEAN],   NULL);
    state->stddev = strtod(state->field_buf[FIELD_STDDEV], NULL);
    state->prob   = strtod(state->field_buf[FIELD_PROB],   NULL);
    state->lambda = strtod(state->field_buf[FIELD_LAMBDA], NULL);

    if (state->count < 1)  state->count = 1;
    if (state->count > MAX_RESULTS) state->count = MAX_RESULTS;
    if (state->prob < 0.0) state->prob = 0.0;
    if (state->prob > 1.0) state->prob = 1.0;
    if (state->stddev <= 0.0) state->stddev = 1.0;
    if (state->lambda <= 0.0) state->lambda = 1.0;
}

void draw_controls(TuiState *state) {
    int x = LEFT_X;
    int y = 3;

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

    term_goto(y + 4, x + 2);
    if (state->dist == DIST_POISSON)
        printf("[*] Poisson      ");
    else
        printf("[ ] Poisson (P)  ");

    static const char *labels[FIELD_COUNT] = {
        "Count:  ", "Min:    ", "Max:    ", "Mean:   ", "StdDev: ", "Prob:   ", "Lambda: "
    };

    int ry = y + 7;
    for (int i = 0; i < FIELD_COUNT; i++) {
        if (!field_is_visible(i, state->dist)) {
            term_goto(ry, x);
            printf("%-*s", LEFT_W, "");
            ry++;
            continue;
        }
        term_goto(ry, x);
        printf("%s", labels[i]);

        term_goto(ry, x + 10);
        printf("[");
        if (i == state->focus_field)
            printf("\033[7m");
        printf("%-10s", state->field_buf[i]);
        if (i == state->focus_field)
            printf("\033[0m");
        printf("]");

        ry++;
    }

    for (; ry < y + 12; ry++) {
        term_goto(ry, x);
        printf("%-*s", LEFT_W, "");
    }

    int by = y + 14;
    term_goto(by, x);
    printf("[ Generate ]  G");
    term_goto(by + 1, x);
    printf("[ Export   ]  E");
    term_goto(by + 2, x);
    printf("[ Quit     ]  Q");

    for (int row = by + 3; row < TERM_H - 1; row++) {
        term_goto(row, x);
        printf("%-*s", LEFT_W, "");
    }
}
