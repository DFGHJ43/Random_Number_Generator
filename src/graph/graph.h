/*
 * graph.h — 分布图形绘制接口
 *
 * draw_graph: 清图+分发  |  draw_graph_*: 各分布具体绘制
 */

#ifndef GRAPH_MODULE_H
#define GRAPH_MODULE_H

#include "../tui.h"

/* Terminal cursor positioning (defined in tui.c) */
extern void term_goto(int row, int col);

/*
 * Redraw the distribution graph based on current state.
 * Clears the graph area and dispatches to the appropriate
 * distribution-specific rendering function.
 */
void draw_graph(const TuiState *state);

/* Individual distribution graph renderers */
void draw_graph_uniform(const TuiState *state);
void draw_graph_normal(const TuiState *state);
void draw_graph_bernoulli(const TuiState *state);

#endif /* GRAPH_MODULE_H */