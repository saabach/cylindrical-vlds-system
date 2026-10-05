#ifndef CRS_OUTPUT_H
#define CRS_OUTPUT_H
#include "config.h"
#include "sampling.h"
#include "crsos.h"
#include <stdio.h>

typedef struct {
    FILE *ly, *method_a, *b_kappa, *b_m, *run;
} Output;

int output_open(Output *output, const Config *config);
int output_write(Output *output, Sampling *sampling);
int output_close(Output *output);
int output_sample(Output *output, int sample, double time, int ly, const EventCounts *counts);
#endif
