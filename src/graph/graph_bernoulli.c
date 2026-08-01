/*
 * graph_bernoulli.c — Bernoulli distribution PMF graph renderer
 *
 * draw_graph_bernoulli: dual-bar PMF (P(0)=1-p, P(1)=p)
 */

#include "graph_bernoulli.h"

#include <math.h>
#include <stdio.h>

void draw_graph_bernoulli(const TuiState *state) {
    int gx = state->gpx;
    int gy = state->gy;
    int gw = state->gpw;
    int gh = state->gh;
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

    char title[48];
    snprintf(title, sizeof(title), "Bernoulli PMF  p=%.2f", p);
    term_goto(gy - 1, gx);
    printf("%-*.*s", state->mw - 2, state->mw - 2, title);

    /* Scale the original 80-col geometry (bar0 +4, bar_w 6, bar1 +20)
       proportionally to the plot width */
    int bar_w  = 6 * gw / 28;
    if (bar_w < 1) bar_w = 1;
    int bar0_x = gx + 4 * gw / 28;
    int bar1_x = gx + 20 * gw / 28;

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

    term_goto(gy + gh + 1, bar0_x + (bar_w - 1) / 2);
    printf("0");
    term_goto(gy + gh + 1, bar1_x + (bar_w - 1) / 2);
    printf("1");

    term_goto(gy, state->mx);
    printf("1.0");
    term_goto(gy + gh, state->mx);
    printf("0.0");

    term_goto(gy + gh - h0 - 1, bar0_x);
    printf("%.2f", 1.0 - p);
    term_goto(gy + gh - h1 - 1, bar1_x);
    printf("%.2f", p);
}
