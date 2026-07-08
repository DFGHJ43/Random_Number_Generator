#ifndef RANDOM_H
#define RANDOM_H

/*
 * Initialize the random number generator with a seed.
 * Call once before generating numbers. If seed is 0, uses time(NULL).
 */
void rng_init(unsigned int seed);

/*
 * Generate a uniformly distributed random integer in [min, max].
 */
int rng_uniform(int min, int max);

/*
 * Generate a normally distributed random number
 * with the given mean and standard deviation.
 * Uses the Box-Muller transform.
 */
double rng_normal(double mean, double stddev);

#endif /* RANDOM_H */
