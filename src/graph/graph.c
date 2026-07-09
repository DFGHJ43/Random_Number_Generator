/*
 * graph.c — Graph area dispatch
 *
 * draw_graph: clear graph area and dispatch to specific distribution
 */

#include "graph.h"

#include <stdio.h>

void draw_graph(const TuiState *state) {
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
