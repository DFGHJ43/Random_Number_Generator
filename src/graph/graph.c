#include "graph.h"

#include <math.h>
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

    /* Y axis */
    for (int i = 0; i <= gh; i++) {
        term_goto(gy + gh - i, gx - 1);
        putchar('|');
    }
    /* X axis */
    term_goto(gy + gh, gx);
    for (int i = 0; i < gw; i++) putchar('-');
    term_goto(gy + gh, gx + gw);
    putchar('>');

    /* title */
    term_goto(gy - 1, gx);
    printf("Uniform PDF  [%d, %d]", min, max);

    /* Draw the uniform bar (constant height across the whole range) */
    int bar_h = gh - 1;
    if (bar_h < 1) bar_h = 1;

    for (int col = 0; col < gw; col++) {
        for (int row = 0; row < bar_h; row++) {
            term_goto(gy + gh - 1 - row, gx + col);
            putchar('#');
        }
    }

    /* X labels */
    term_goto(gy + gh + 1, gx);
    printf("%d", min);
    char buf[16];
    snprintf(buf, sizeof(buf), "%d", max);
    term_goto(gy + gh + 1, gx + gw - (int)strlen(buf));
    printf("%s", buf);

    /* Y label */
    double pdf_val = 1.0 / (max - min);
    term_goto(gy, MID_X);
    printf("%4s", "");
    term_goto(gy, MID_X);
    printf("%.2f", pdf_val);
}

void draw_graph_normal(const TuiState *state) {
    int gx = GRAPH_PLOT_X;
    int gy = GRAPH_Y;
    int gw = GRAPH_PLOT_W;
    int gh = GRAPH_H;
    double mu = state->mean;
    double sigma = state->stddev;

    if (sigma <= 0.0) return;

    /* Y axis */
    for (int i = 0; i <= gh; i++) {
        term_goto(gy + gh - i, gx - 1);
        putchar('|');
    }
    /* X axis */
    term_goto(gy + gh, gx);
    for (int i = 0; i < gw; i++) putchar('-');
    term_goto(gy + gh, gx + gw);
    putchar('>');

    /* title */
    term_goto(gy - 1, gx);
    printf("Normal PDF  mu=%.1f sigma=%.1f", mu, sigma);

    /* X range: [mu - 3*sigma, mu + 3*sigma] */
    double x_min = mu - 3.0 * sigma;
    double x_max = mu + 3.0 * sigma;

    double pdf_max = 1.0 / (sigma * sqrt(2.0 * M_PI));

    for (int col = 0; col < gw; col++) {
        double x = x_min + (x_max - x_min) * ((double)col / (double)(gw - 1));
        double z = (x - mu) / sigma;
        double pdf = exp(-0.5 * z * z) / (sigma * sqrt(2.0 * M_PI));

        int h = (int)((pdf / pdf_max) * (double)(gh - 1) + 0.5);
        if (h < 0) h = 0;
        if (h > gh - 1) h = gh - 1;

        for (int row = 0; row <= h; row++) {
            term_goto(gy + gh - 1 - row, gx + col);
            putchar('#');
        }
    }

    /* X labels */
    term_goto(gy + gh + 1, gx);
    printf("%.0f", x_min);
    char buf[16];
    snprintf(buf, sizeof(buf), "%.0f", x_max);
    term_goto(gy + gh + 1, gx + gw - (int)strlen(buf));
    printf("%s", buf);

    /* Mean marker */
    int mu_col = (int)((mu - x_min) / (x_max - x_min) * (double)(gw - 1));
    if (mu_col >= 0 && mu_col < gw) {
        term_goto(gy + gh + 1, gx + mu_col);
        printf("mu");
    }

    /* Y label (peak) */
    term_goto(gy, MID_X);
    printf("%.3f", pdf_max);
}

void draw_graph_bernoulli(const TuiState *state) {
    int gx = GRAPH_PLOT_X;
    int gy = GRAPH_Y;
    int gw = GRAPH_PLOT_W;
    int gh = GRAPH_H;
    double p = state->prob;

    if (p < 0.0) p = 0.0;
    if (p > 1.0) p = 1.0;

    /* Y axis */
    for (int i = 0; i <= gh; i++) {
        term_goto(gy + gh - i, gx - 1);
        putchar('|');
    }
    /* X axis */
    term_goto(gy + gh, gx);
    for (int i = 0; i < gw; i++) putchar('-');
    term_goto(gy + gh, gx + gw);
    putchar('>');

    /* title */
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

    /* X labels */
    term_goto(gy + gh + 1, bar0_x + 2);
    printf("0");
    term_goto(gy + gh + 1, bar1_x + 2);
    printf("1");

    /* Y labels */
    term_goto(gy, MID_X);
    printf("1.0");
    term_goto(gy + gh, MID_X);
    printf("0.0");

    /* Probability annotations */
    term_goto(gy + gh - h0 - 1, bar0_x);
    printf("%.2f", 1.0 - p);
    term_goto(gy + gh - h1 - 1, bar1_x);
    printf("%.2f", p);
}

void draw_graph(const TuiState *state) {
    /* Clear graph area */
    for (int row = GRAPH_Y - 1; row < GRAPH_Y + GRAPH_H + 3; row++) {
        term_goto(row, MID_X);
        for (int col = 0; col < MID_W; col++) putchar(' ');
    }

    if (state->dist == DIST_NORMAL) {
        draw_graph_normal(state);
    } else if (state->dist == DIST_BERNOULLI) {
        draw_graph_bernoulli(state);
    } else {
        draw_graph_uniform(state);
    }
}