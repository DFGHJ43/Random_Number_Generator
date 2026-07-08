#include "random.h"

#include <math.h>
#include <stdlib.h>
#include <time.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void rng_init(unsigned int seed) {
    if (seed == 0) {
        seed = (unsigned int)time(NULL);
    }
    srand(seed);
}

int rng_uniform(int min, int max) {
    if (min > max) {
        int tmp = min;
        min = max;
        max = tmp;
    }
    /* rand() returns [0, RAND_MAX]; scale to [min, max] */
    return min + rand() % (max - min + 1);
}

double rng_normal(double mean, double stddev) {
    /*
     * Box-Muller transform:
     * Given two independent uniform(0,1) random numbers u1, u2,
     * z0 = sqrt(-2 * ln(u1)) * cos(2 * pi * u2)
     * produces a standard normal(0,1) random variable.
     * Then scale: result = mean + stddev * z0.
     */
    double u1, u2, z0;

    /* Avoid log(0): keep generating until u1 > 0 */
    do {
        u1 = (double)rand() / (double)RAND_MAX;
    } while (u1 <= 0.0);

    u2 = (double)rand() / (double)RAND_MAX;

    z0 = sqrt(-2.0 * log(u1)) * cos(2.0 * M_PI * u2);

    return mean + stddev * z0;
}

int rng_bernoulli(double p) {
    /* Clamp p to [0, 1] */
    if (p < 0.0) p = 0.0;
    if (p > 1.0) p = 1.0;

    double u = (double)rand() / (double)RAND_MAX;
    return (u < p) ? 1 : 0;
}
