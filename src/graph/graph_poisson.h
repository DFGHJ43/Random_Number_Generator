/*
 * graph_poisson.h — Poisson distribution PMF graph renderer
 */

#ifndef GRAPH_POISSON_H
#define GRAPH_POISSON_H

#include "../tui.h"

extern void term_goto(int row, int col);

void draw_graph_poisson(const TuiState *state);

#endif /* GRAPH_POISSON_H */
