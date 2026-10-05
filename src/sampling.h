#ifndef CRS_SAMPLING_H
#define CRS_SAMPLING_H
#include "observables.h"

typedef struct {
    int length;
    double *time, *height, *central, *cumulants, *skewness, *kurtosis;
    int *columns; /* Deliberate legacy integer averaging; see docs/OUTPUTS.md. */
} Sampling;

int sampling_create(Sampling *sampling, int max_time);
void sampling_add(Sampling *sampling, int index, double time, int ly, const Moments *m);
void sampling_normalize(Sampling *sampling, int samples);
void sampling_destroy(Sampling *sampling);
#endif
