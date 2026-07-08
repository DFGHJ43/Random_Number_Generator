#include "random.h"
#include "stats.h"
#include "output.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

/* Default values */
#define DEFAULT_COUNT   1
#define DEFAULT_MIN     0
#define DEFAULT_MAX     100
#define DEFAULT_MEAN    0.0
#define DEFAULT_STDDEV  1.0

static void print_usage(const char *prog) {
    printf("Usage: %s [options]\n", prog);
    printf("\nOptions:\n");
    printf("  -n <count>   Number of random numbers to generate (default: %d)\n", DEFAULT_COUNT);
    printf("  -l <min>     Lower bound for uniform distribution (default: %d)\n", DEFAULT_MIN);
    printf("  -u <max>     Upper bound for uniform distribution (default: %d)\n", DEFAULT_MAX);
    printf("  -d <dist>    Distribution: 'uniform' or 'normal' (default: uniform)\n");
    printf("  -m <mean>    Mean for normal distribution (default: %.1f)\n", DEFAULT_MEAN);
    printf("  -v <stddev>  Standard deviation for normal distribution (default: %.1f)\n", DEFAULT_STDDEV);
    printf("  -s <seed>    Random seed for reproducible results\n");
    printf("  -o <file>    Write output to file instead of console\n");
    printf("  --stats      Print statistical summary (min, max, mean, stddev)\n");
    printf("  -h           Show this help message\n");
    printf("\nExamples:\n");
    printf("  %s -n 5 -l 1 -u 100\n", prog);
    printf("  %s -n 1000 -d normal -m 50 -v 10 --stats\n", prog);
    printf("  %s -n 100 -o output.txt -s 42\n", prog);
}

int main(int argc, char *argv[]) {
    int    count  = DEFAULT_COUNT;
    int    min    = DEFAULT_MIN;
    int    max    = DEFAULT_MAX;
    double mean   = DEFAULT_MEAN;
    double stddev = DEFAULT_STDDEV;
    int    seed   = 0;          /* 0 means use time(NULL) */
    int    use_seed = 0;        /* flag: did user specify a seed? */
    char  *dist   = "uniform";
    char  *outfile = NULL;
    int    show_stats = 0;

    /* Long options */
    static struct option long_opts[] = {
        {"stats", no_argument, 0, 'S'},
        {"help",  no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "n:l:u:d:m:v:s:o:h", long_opts, NULL)) != -1) {
        switch (opt) {
        case 'n':
            count = atoi(optarg);
            if (count <= 0) {
                fprintf(stderr, "Error: count must be positive\n");
                return 1;
            }
            break;
        case 'l':
            min = atoi(optarg);
            break;
        case 'u':
            max = atoi(optarg);
            break;
        case 'd':
            dist = optarg;
            break;
        case 'm':
            mean = atof(optarg);
            break;
        case 'v':
            stddev = atof(optarg);
            if (stddev <= 0) {
                fprintf(stderr, "Error: standard deviation must be positive\n");
                return 1;
            }
            break;
        case 's':
            seed = atoi(optarg);
            use_seed = 1;
            break;
        case 'o':
            outfile = optarg;
            break;
        case 'S':
            show_stats = 1;
            break;
        case 'h':
            print_usage(argv[0]);
            return 0;
        default:
            print_usage(argv[0]);
            return 1;
        }
    }

    /* Validate uniform range */
    if (min >= max && strcmp(dist, "uniform") == 0) {
        fprintf(stderr, "Error: lower bound must be less than upper bound\n");
        return 1;
    }

    /* Initialize RNG */
    rng_init(use_seed ? (unsigned int)seed : 0);

    /* Allocate array */
    double *numbers = (double *)malloc((size_t)count * sizeof(double));
    if (numbers == NULL) {
        fprintf(stderr, "Error: memory allocation failed\n");
        return 1;
    }

    /* Generate numbers */
    if (strcmp(dist, "normal") == 0) {
        for (int i = 0; i < count; i++) {
            numbers[i] = rng_normal(mean, stddev);
        }
    } else {
        /* Default: uniform distribution */
        for (int i = 0; i < count; i++) {
            numbers[i] = (double)rng_uniform(min, max);
        }
    }

    /* Output */
    if (outfile != NULL) {
        if (output_file(numbers, count, outfile) != 0) {
            free(numbers);
            return 1;
        }
    } else {
        output_console(numbers, count);
    }

    /* Statistics */
    if (show_stats) {
        StatsResult stats = stats_compute(numbers, count);
        output_stats(&stats);
    }

    free(numbers);
    return 0;
}
