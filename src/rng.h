#ifndef CRS_RNG_H
#define CRS_RNG_H

/* Historical generators; one stream family per process, not reentrant. */
void rng_set_seed(unsigned int seed);
int rng_choose_seed(const char *log_path, unsigned int *seed);
void rng_initialize_sample(void);
double rng_probability(void);
unsigned int rng_line(int length);
unsigned int rng_column(int length);
unsigned int rng_neighbor(void);
#endif
