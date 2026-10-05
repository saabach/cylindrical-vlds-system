#ifndef CRS_CONFIG_H
#define CRS_CONFIG_H

typedef struct {
    int delta, max_time, samples, lx, initial_ly, max_columns;
    double omega;
    unsigned int seed;
    int seed_given;
    const char *output_dir;
} Config;

#endif
