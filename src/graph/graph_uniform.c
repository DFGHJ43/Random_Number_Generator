/*
 * graph_uniform.c — Uniform distribution PDF graph renderer
 *
 * draw_graph_uniform: flat bar (constant PDF = 1/(max-min))
 */

#include "graph_uniform.h"

#include <stdio.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void draw_graph_uniform(const TuiState *state) {
    int gx = GRAPH_PLOT_X;
    int gy = GRAPH_Y;
    int gw = GRAPH_PLOT_W;
    int gh = GRAPH_H;
    int min = state->min_val;
    int max = state->max_val;

    if (min >= max) return;

    for (int i = 0; i <= gh; i++) {
        term_goto(gy + gh - i, gx - 1);
        putchar('|');
    }

    term_goto(gy + gh, gx);
    for (int i = 0; i < gw; i++) putchar('-');
    term_goto(gy + gh, gx + gw);
    putchar('>');

    term_goto(gy - 1, gx);
    printf("Uniform PDF  [%d, %d]", min, max);

    int bar_h = gh - 1;
    if (bar_h < 1) bar_h = 1;

    for (int col = 0; col < gw; col++) {
        for (int row = 0; row < bar_h; row++) {
            term_goto(gy + gh - 1 - row, gx + col);
            putchar('#');
        }
    }

    term_goto(gy + gh + 1, gx);
    printf("%d", min);
    char buf[16];
    snprintf(buf, sizeof(buf), "%d", max);
    term_goto(gy + gh + 1, gx + gw - (int)strlen(buf));
    printf("%s", buf);

    double pdf_val = 1.0 / (max - min);
    term_goto(gy, MID_X);
    printf("%4s", "");
    term_goto(gy, MID_X);
    printf("%.2f", pdf_val);
}
