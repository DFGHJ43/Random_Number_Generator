/*
 * graph_poisson.c — Poisson distribution PMF graph renderer
 *
 * draw_graph_poisson: discrete bar chart of P(k) = (λ^k * e^(-λ)) / k!
 */

#include "graph_poisson.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

void draw_graph_poisson(const TuiState *state) {
    int gx = GRAPH_PLOT_X;
    int gy = GRAPH_Y;
    int gw = GRAPH_PLOT_W;
    int gh = GRAPH_H;
    double lambda = state->lambda;

    if (lambda <= 0.0) return;

    for (int i = 0; i <= gh; i++) {
        term_goto(gy + gh - i, gx - 1);
        putchar('|');
    }

    term_goto(gy + gh, gx);
    for (int i = 0; i < gw; i++) putchar('-');
    term_goto(gy + gh, gx + gw);
    putchar('>');

    term_goto(gy - 1, gx);
    printf("Poisson PMF  lambda=%.1f", lambda);

    int k_max = (int)(lambda + 4.0 * sqrt(lambda));
    if (k_max < (int)lambda + 1) k_max = (int)lambda + 1;

    double pmf[64];
    int count = 0;

    pmf[0] = exp(-lambda);
    count = 1;

    for (int k = 1; k <= k_max && count < 64; k++) {
        pmf[count] = pmf[count - 1] * lambda / (double)k;
        count++;
    }

    double pmf_max = pmf[0];
    for (int i = 1; i < count; i++) {
        if (pmf[i] > pmf_max) pmf_max = pmf[i];
    }
    if (pmf_max <= 0.0) pmf_max = 1.0;

    int bar_w;
    int skip = 1;
    if (count <= gw) {
        bar_w = gw / count;
        if (bar_w < 1) bar_w = 1;
    } else {
        bar_w = 1;
        skip = (count + gw - 1) / gw;
    }

    int col = 0;
    for (int i = 0; i < count && col < gw; i += skip) {
        int h = (int)((pmf[i] / pmf_max) * (double)(gh - 1) + 0.5);
        if (h < 1 && pmf[i] > 0.0) h = 1;
        if (h > gh - 1) h = gh - 1;

        int w = bar_w;
        if (col + w > gw) w = gw - col;

        for (int row = 0; row < h; row++) {
            for (int c = 0; c < w; c++) {
                term_goto(gy + gh - 1 - row, gx + col + c);
                putchar('#');
            }
        }
        col += w;
    }

    term_goto(gy + gh + 1, gx);
    printf("0");
    char buf[16];
    snprintf(buf, sizeof(buf), "%d", k_max);
    term_goto(gy + gh + 1, gx + gw - (int)strlen(buf));
    printf("%s", buf);

    int lambda_col = (int)((lambda / (double)k_max) * (double)(gw - 1));
    if (lambda_col >= 0 && lambda_col < gw) {
        term_goto(gy + gh + 1, gx + lambda_col);
        printf("L");
    }

    term_goto(gy, MID_X);
    printf("%.3f", pmf_max);
}
