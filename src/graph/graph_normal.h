/*
 * graph_normal.h — Normal distribution PDF graph renderer
 */

#ifndef GRAPH_NORMAL_H
#define GRAPH_NORMAL_H

#include "../tui.h"

extern void term_goto(int row, int col);

void draw_graph_normal(const TuiState *state);

#endif /* GRAPH_NORMAL_H */
