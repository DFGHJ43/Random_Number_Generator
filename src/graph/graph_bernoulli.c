/*
 * graph_bernoulli.c — Bernoulli distribution PMF graph renderer
 *
 * draw_graph_bernoulli: dual-bar PMF (P(0)=1-p, P(1)=p)
 */

#include "graph_bernoulli.h"

#include <math.h>
#include <stdio.h>

void draw_graph_bernoulli(const TuiState *state) {
    int gx = GRAPH_PLOT_X;
    int gy = GRAPH_Y;
    int gw = GRAPH_PLOT_W;
    int gh = GRAPH_H;
    double p = state->prob;

    if (p < 0.0) p = 0.0;
    if (p > 1.0) p = 1.0;

    for (int i = 0; i <= gh; i++) {
        term_goto(gy + gh - i, gx - 1);
        putchar('|');
    }

    term_goto(gy + gh, gx);
    for (int i = 0; i < gw; i++) putchar('-');
    term_goto(gy + gh, gx + gw);
    putchar('>');

    term_goto(gy - 1, gx);
    printf("Bernoulli PMF  p=%.2f", p);

    int bar0_x = gx + 4;
    int bar1_x = gx + gw - 8;
    int bar_w  = 6;

    int h0 = (int)((1.0 - p) * (double)(gh - 1) + 0.5);
    int h1 = (int)(p * (double)(gh - 1) + 0.5);
    if (h0 < 1) h0 = 1;
    if (h1 < 1) h1 = 1;

    for (int row = 0; row < h0; row++) {
        for (int col = 0; col < bar_w; col++) {
            term_goto(gy + gh - 1 - row, bar0_x + col);
            putchar('#');
        }
    }
    for (int row = 0; row < h1; row++) {
        for (int col = 0; col < bar_w; col++) {
            term_goto(gy + gh - 1 - row, bar1_x + col);
            putchar('#');
        }
    }

    term_goto(gy + gh + 1, bar0_x + 2);
    printf("0");
    term_goto(gy + gh + 1, bar1_x + 2);
    printf("1");

    term_goto(gy, MID_X);
    printf("1.0");
    term_goto(gy + gh, MID_X);
    printf("0.0");

    term_goto(gy + gh - h0 - 1, bar0_x);
    printf("%.2f", 1.0 - p);
    term_goto(gy + gh - h1 - 1, bar1_x);
    printf("%.2f", p);
}
