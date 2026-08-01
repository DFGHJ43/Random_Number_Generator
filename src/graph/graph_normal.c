/*
 * graph_normal.c — Normal distribution PDF graph renderer
 *
 * draw_graph_normal: bell curve via Box-Muller PDF (28 sample columns)
 */

#include "graph_normal.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void draw_graph_normal(const TuiState *state) {
    int gx = state->gpx;
    int gy = state->gy;
    int gw = state->gpw;
    int gh = state->gh;
    double mu = state->mean;
    double sigma = state->stddev;

    if (sigma <= 0.0) return;

    for (int i = 0; i <= gh; i++) {
        term_goto(gy + gh - i, gx - 1);
        putchar('|');
    }

    term_goto(gy + gh, gx);
    for (int i = 0; i < gw; i++) putchar('-');
    term_goto(gy + gh, gx + gw);
    putchar('>');

    char title[48];
    snprintf(title, sizeof(title), "Normal PDF  mu=%.1f sigma=%.1f", mu, sigma);
    term_goto(gy - 1, gx);
    printf("%-*.*s", state->mw - 2, state->mw - 2, title);

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

    term_goto(gy + gh + 1, gx);
    printf("%.0f", x_min);
    char buf[16];
    snprintf(buf, sizeof(buf), "%.0f", x_max);
    term_goto(gy + gh + 1, gx + gw - (int)strlen(buf));
    printf("%s", buf);

    int mu_col = (int)((mu - x_min) / (x_max - x_min) * (double)(gw - 1));
    if (mu_col >= 0 && mu_col < gw) {
        term_goto(gy + gh + 1, gx + mu_col);
        printf("mu");
    }

    term_goto(gy, state->mx);
    printf("%.3f", pdf_max);
}
