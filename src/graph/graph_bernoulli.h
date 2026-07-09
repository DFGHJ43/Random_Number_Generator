/*
 * graph_bernoulli.h — Bernoulli distribution PMF graph renderer
 */

#ifndef GRAPH_BERNOULLI_H
#define GRAPH_BERNOULLI_H

#include "../tui.h"

extern void term_goto(int row, int col);

void draw_graph_bernoulli(const TuiState *state);

#endif /* GRAPH_BERNOULLI_H */
