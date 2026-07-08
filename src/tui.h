/*
 * tui.h — 布局常量、字段枚举、分布类型、应用状态 TuiState 定义
 *
 * 栅格列: LEFT_X/W=1/22  DIV1=23  MID_X/W=24/32  DIV2=56  RIGHT_X/W=57/22
 * TuiState: dist(分布类型) count/min_val/max_val/mean/stddev/prob(参数)
 *           results[] result_count/scroll(结果与滚动)
 *           focus_field field_buf[][] field_cursor[](输入编辑)
 *           status[](状态消息) graph_dirty(脏标记)
 */

#ifndef TUI_H
#define TUI_H

/* ── Terminal grid ────────────────────────────────────── */
#define TERM_W  80
#define TERM_H  24

/* ── Column grid (strict, 0-indexed) ──────────────────── */
/*  col 0=border, 1-22=controls, 23=div, 24-55=graph,
    56=div, 57-78=results, 79=border                         */
#define LEFT_X    1
#define LEFT_W    22
#define DIV1      23        /* LEFT_X + LEFT_W */
#define MID_X     24        /* DIV1 + 1 */
#define MID_W     32
#define DIV2      56        /* MID_X + MID_W */
#define RIGHT_X   57        /* DIV2 + 1 */
#define RIGHT_W   22

/* ── Graph inside middle panel ────────────────────────── */
#define GRAPH_Y    4
#define GRAPH_H    10
#define GRAPH_PLOT_X  (MID_X + 2)   /* plot area start */
#define GRAPH_PLOT_W  (MID_W - 4)   /* 28 data columns */

/* ── Status bar ───────────────────────────────────────── */
#define STATUS_Y  (TERM_H - 2)

/* ── Input fields ─────────────────────────────────────── */
#define FIELD_COUNT    6
#define FIELD_BUF_LEN  32

typedef enum {
    FIELD_COUNT_NUM = 0,
    FIELD_MIN,
    FIELD_MAX,
    FIELD_MEAN,
    FIELD_STDDEV,
    FIELD_PROB
} FieldId;

/* ── Distribution type ────────────────────────────────── */
typedef enum {
    DIST_UNIFORM    = 0,
    DIST_NORMAL     = 1,
    DIST_BERNOULLI  = 2
} DistType;

/* ── Main application state ───────────────────────────── */
#define MAX_RESULTS 1000

typedef struct {
    /* parameters */
    DistType dist;
    int      count;
    int      min_val;
    int      max_val;
    double   mean;
    double   stddev;
    double   prob;

    /* results */
    double   results[MAX_RESULTS];
    int      result_count;
    int      result_scroll;

    /* input editing */
    int      focus_field;
    char     field_buf[FIELD_COUNT][FIELD_BUF_LEN];
    int      field_cursor[FIELD_COUNT];

    /* status message */
    char     status[128];

    /* running flag */
    int      running;

    /* dirty flags for selective redraw */
    int      graph_dirty;
} TuiState;

/* ── TUI API ──────────────────────────────────────────── */
void tui_init(TuiState *state);
void tui_restore(void);
void tui_run(void);

#endif /* TUI_H */
