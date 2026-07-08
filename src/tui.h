#ifndef TUI_H
#define TUI_H

/* ── Terminal dimensions ─────────────────────────────── */
#define TUI_MIN_ROWS  24
#define TUI_MIN_COLS  80

/* ── Layout geometry ─────────────────────────────────── */
#define LEFT_X        2
#define LEFT_W        18
#define MID_DIV       21     /* divider between controls & graph */
#define GRAPH_X       23
#define GRAPH_W       30
#define GRAPH_Y       5
#define GRAPH_H       10
#define RIGHT_DIV     55     /* divider between graph & results */
#define RES_X         57
#define RES_W         22
#define STATUS_Y      (TUI_MIN_ROWS - 2)

/* ── Input fields ────────────────────────────────────── */
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

/* ── Distribution type ───────────────────────────────── */
typedef enum {
    DIST_UNIFORM    = 0,
    DIST_NORMAL     = 1,
    DIST_BERNOULLI  = 2
} DistType;

/* ── Main application state ──────────────────────────── */
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

/* ── TUI API ─────────────────────────────────────────── */
void tui_init(TuiState *state);
void tui_restore(void);
void tui_run(void);

#endif /* TUI_H */
