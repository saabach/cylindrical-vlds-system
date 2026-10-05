#include "sampling.h"
#include <stdlib.h>

int sampling_create(Sampling *s, int max_time) {
    *s = (Sampling){.length = max_time};
    s->time = calloc((size_t)max_time, sizeof(double));
    s->columns = calloc((size_t)max_time, sizeof(int));
    s->height = calloc((size_t)max_time, sizeof(double));
    s->central = calloc((size_t)4 * max_time, sizeof(double));
    s->cumulants = calloc((size_t)4 * max_time, sizeof(double));
    s->skewness = calloc((size_t)max_time, sizeof(double));
    s->kurtosis = calloc((size_t)max_time, sizeof(double));
    if (!s->time || !s->columns || !s->height || !s->central || !s->cumulants || !s->skewness ||
        !s->kurtosis) {
        sampling_destroy(s);
        return 0;
    }
    return 1;
}
void sampling_add(Sampling *s, int index, double time, int ly, const Moments *m) {
    // Collecting Measures.
    s->time[index] += time;
    s->columns[index] += ly;
    s->height[index] += m->height;
    s->central[1 * s->length + (index)] += m->m2;
    s->central[2 * s->length + (index)] += m->m3;
    s->central[3 * s->length + (index)] += m->m4;
    s->cumulants[0 * s->length + (index)] += m->k1;
    s->cumulants[1 * s->length + (index)] += m->k2;
    s->cumulants[2 * s->length + (index)] += m->k3;
    s->cumulants[3 * s->length + (index)] += m->k4;
    s->skewness[index] += m->skewness;
    s->kurtosis[index] += m->kurtosis;
}
void sampling_normalize(Sampling *s, int samples) {
    // Normalizing the Input's.
    for (int i = 0; i < s->length; i++) {
        s->time[i] /= samples;
        s->columns[i] /= samples;
        s->height[i] /= samples;
        s->skewness[i] /= samples;
        s->kurtosis[i] /= samples;
        for (int j = 0; j < 4; j++) {
            s->central[j * s->length + i] /= samples;
            s->cumulants[j * s->length + i] /= samples;
        }
    }
}
void sampling_destroy(Sampling *s) {
    free(s->time);
    free(s->columns);
    free(s->height);
    free(s->central);
    free(s->cumulants);
    free(s->skewness);
    free(s->kurtosis);
    *s = (Sampling){0};
}
