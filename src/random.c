/*
 * random.c — Random number generation (based on C stdlib rand())
 *
 * rng_init(seed): init seed, uses time(NULL) when seed=0
 * rng_uniform(min,max): uniform integer in [min, max]
 * rng_normal(mean,stddev): normal via Box-Muller transform
 * rng_bernoulli(p): Bernoulli, returns 1 with probability p, else 0
 */

#include "random.h"

#include <limits.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

/* rand() is intentionally used for this learning project */

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void rng_init(unsigned int seed) {
    if (seed == 0) {
        seed = (unsigned int)time(NULL);
    }
    srand(seed);
}

/* RAND_MAX >= 32767 is guaranteed by the standard (true for both MSVC and
   mingw), so five 15-bit chunks cover the full 64-bit space. */
static unsigned long long rng_raw64(void) {
    unsigned long long v = 0;
    for (int i = 0; i < 5; i++)
        v = (v << 15) | (unsigned long long)((unsigned)rand() & 0x7FFFu);  /* NOLINT */
    return v;
}

int rng_uniform(int min, int max) {
    if (min > max) {
        int tmp = min;
        min = max;
        max = tmp;
    }
    /*
     * Range computed in 64-bit: (max - min + 1) reaches 2^32 for the full
     * int span, which overflows int (min=INT_MIN, max=INT_MAX wraps it to 0
     * and rand() % 0 crashes). A single rand() call covers at most
     * RAND_MAX+1 values, so calls are combined into a 64-bit pool, then
     * rejection-sampled to remove the modulo bias.
     */
    unsigned long long range = (unsigned long long)max - (unsigned long long)min + 1;
    unsigned long long limit = ULLONG_MAX - ULLONG_MAX % range;
    unsigned long long r;
    do {
        r = rng_raw64();
    } while (r >= limit);
    return (int)((long long)min + (long long)(r % range));
}

double rng_normal(double mean, double stddev) {
    /*
     * Box-Muller transform:
     * Given two independent uniform(0,1) random numbers u1, u2,
     * z0 = sqrt(-2 * ln(u1)) * cos(2 * pi * u2)
     * produces a standard normal(0,1) random variable.
     * Then scale: result = mean + stddev * z0.
     */
    /* Avoid log(0): keep generating until u1 > 0 */
    double u1;
    do {
        u1 = (double)rand() / (double)RAND_MAX;  /* NOLINT */
    } while (u1 <= 0.0);

    double u2 = (double)rand() / (double)RAND_MAX;  /* NOLINT */
    double z0 = sqrt(-2.0 * log(u1)) * cos(2.0 * M_PI * u2);

    return mean + stddev * z0;
}

int rng_bernoulli(double p) {
    /* Clamp p to [0, 1] */
    if (p < 0.0) p = 0.0;
    if (p > 1.0) p = 1.0;

    double u = (double)rand() / (double)RAND_MAX;  /* NOLINT */
    return (u < p) ? 1 : 0;
}

int rng_poisson(double lambda) {
    /* Clamp lambda to positive */
    if (lambda <= 0.0) lambda = 0.001;

    if (lambda > 10.0) {
        /*
         * PTRS (Hörmann 1993): transformed-rejection sampling from a
         * truncated logistic distribution - exact for any lambda.
         * Knuth's loop below is O(lambda) and exp(-lambda) underflows to
         * 0.0 past lambda ~ 745, silently capping results at ~745.
         */
        const double c     = 0.767 - 3.36 / lambda;
        const double beta  = M_PI / sqrt(3.0 * lambda);
        const double alpha = beta * lambda;
        const double k_log = log(c) - lambda - log(beta);

        for (;;) {
            /* u in (0,1): keeps log((1-u)/u) finite */
            double u = ((double)rand() + 1.0) / ((double)RAND_MAX + 2.0);  /* NOLINT */
            double x = (alpha - log((1.0 - u) / u)) / beta;
            if (x > 9.0e18) return INT_MAX;  /* absurd lambda via direct call */
            long long n = (long long)floor(x + 0.5);
            if (n < 0) continue;

            double v = ((double)rand() + 1.0) / ((double)RAND_MAX + 2.0);  /* NOLINT */
            double y = alpha - beta * x;
            double lhs = y + log(v) - 2.0 * log(1.0 + exp(y));
            double rhs = k_log + (double)n * log(lambda) - lgamma((double)n + 1.0);
            if (lhs <= rhs) {
                /* Guard direct callers passing an absurd lambda */
                if (n > INT_MAX) return INT_MAX;
                return (int)n;
            }
        }
    }

    /*
     * Knuth's algorithm for Poisson distribution:
     * L = e^(-lambda), k = 0, p = 1
     * do { k++; p *= U } while (p > L)
     * return k - 1
     */
    double L = exp(-lambda);
    int k = 0;
    double p = 1.0;

    do {
        k++;
        double u = (double)rand() / (double)RAND_MAX;  /* NOLINT */
        p *= u;
    } while (p > L);

    return k - 1;
}
