/*
 * graph_uniform.h — Uniform distribution PDF graph renderer
 */

#ifndef GRAPH_UNIFORM_H
#define GRAPH_UNIFORM_H

#include "../tui.h"

extern void term_goto(int row, int col);

void draw_graph_uniform(const TuiState *state);

#endif /* GRAPH_UNIFORM_H */
