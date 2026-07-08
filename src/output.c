/*
 * output.c — 输出功能
 *
 * output_console: 打印数字到控制台(CLI 模式使用)
 * output_file: 写入数字到文件(CSV 导出)
 * output_stats: 打印统计摘要(CLI 模式使用)
 */

#include "output.h"

#include <stdio.h>

void output_console(const double *numbers, int count) {
    for (int i = 0; i < count; i++) {
        printf("%g\n", numbers[i]);
    }
}

int output_file(const double *numbers, int count, const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        fprintf(stderr, "Error: cannot open file '%s' for writing\n", filename);
        return -1;
    }
    for (int i = 0; i < count; i++) {
        fprintf(fp, "%g\n", numbers[i]);
    }
    fclose(fp);
    /* TUI handles status display; stdout would corrupt the terminal */
    return 0;
}

void output_stats(const StatsResult *stats) {
    printf("\n--- Statistics ---\n");
    printf("Count:          %d\n", stats->count);
    printf("Minimum:        %g\n", stats->min);
    printf("Maximum:        %g\n", stats->max);
    printf("Mean:           %g\n", stats->mean);
    printf("Std Deviation:  %g\n", stats->stddev);
}
