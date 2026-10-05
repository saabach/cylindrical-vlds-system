#ifndef CRS_TRACE_H
#define CRS_TRACE_H
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

/* Test-only fingerprint; no RNG calls or changes to simulation arithmetic. */
static inline void trace_measure(int sample, int measurement, double time, int lx, int ly,
                                 int stride, const int *height, const int *top, const int *bottom,
                                 const int *left, const int *right, uint64_t events,
                                 uint64_t depositions, uint64_t duplications, uint64_t diffusion) {
    uint64_t h = UINT64_C(14695981039346656037);
#define HASH_VALUE(v)                                                                              \
    do {                                                                                           \
        h ^= (uint32_t)(v);                                                                        \
        h *= UINT64_C(1099511628211);                                                              \
    } while (0)
    for (int i = 0; i < lx; i++)
        for (int j = 0; j < ly; j++)
            HASH_VALUE(height[i * stride + j]);
    for (int i = 0; i < lx; i++) {
        HASH_VALUE(top[i]);
        HASH_VALUE(bottom[i]);
    }
    for (int j = 0; j < ly; j++) {
        HASH_VALUE(left[j]);
        HASH_VALUE(right[j]);
    }
#undef HASH_VALUE
    FILE *f = fopen("equivalence.trace", "a");
    if (!f)
        exit(91);
    int ok =
        fprintf(f, "%d %d %a %d %" PRIu64 " %" PRIu64 " %" PRIu64 " %" PRIu64 " %016" PRIx64 "\n",
                sample, measurement, time, ly, events, depositions, duplications, diffusion,
                h) >= 0;
    if (fclose(f) != 0 || !ok)
        exit(92);
}
#endif
