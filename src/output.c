#define _POSIX_C_SOURCE 200809L
#include "output.h"
#include "observables.h"
#include <math.h>
#include <inttypes.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

static FILE *open_new(const Config *c, const char *suffix) {
    char path[4096];
    int n = snprintf(path, sizeof(path), "%s/delta%d-%s", c->output_dir, c->delta, suffix);
    if (n < 0 || (size_t)n >= sizeof(path))
        return NULL;
    /* Refuse overwriting an earlier run. Outputs belong to separate run dirs. */
    return fopen(path, "wx");
}
int output_open(Output *o, const Config *c) {
    *o = (Output){0};
    if (mkdir(c->output_dir, 0777) != 0 && errno != EEXIST)
        return 0;
    o->ly = open_new(c, "LySize.dat");
    o->method_a = open_new(c, "MethodA.dat");
    o->b_kappa = open_new(c, "Bkappa.dat");
    o->b_m = open_new(c, "Bm.dat");
    o->run = open_new(c, "run.txt");
    if (!o->ly || !o->method_a || !o->b_kappa || !o->b_m || !o->run)
        return 0;
    return fprintf(o->run,
                   "delta=%d\nmax_time=%d\nsamples=%d\nlx=%d\ninitial_ly=%d\nmax_columns=%d\nomega="
                   "%.17g\nseed=%u\n"
                   "# sample time Ly events depositions duplications diffusion_steps\n",
                   c->delta, c->max_time, c->samples, c->lx, c->initial_ly, c->max_columns,
                   c->omega, c->seed) >= 0;
}
int output_sample(Output *o, int sample, double time, int ly, const EventCounts *n) {
    return fprintf(o->run, "%d %.34g %d %" PRIu64 " %" PRIu64 " %" PRIu64 " %" PRIu64 "\n", sample,
                   time, ly, n->events, n->depositions, n->duplications, n->diffusion_steps) >= 0;
}
int output_write(Output *o, Sampling *s) {
    // Data Management: Archive's Creating.
    // Ly's Size Information's Transcript.
    for (int i = 0; i < s->length; i++) {
        fprintf(o->ly, "%.34g\t%d\n", s->time[i], s->columns[i]);
    }
    for (int i = 0; i < s->length; i++) {
        fprintf(o->method_a, "%.34g\t%.34g\t%.34g\t%.34g\t%.34g\n", s->time[i], s->height[i],
                s->skewness[i], s->kurtosis[i], s->cumulants[1 * s->length + i]);
    }
    for (int i = 0; i < s->length; i++) { // <- Method B Calculating.
        s->skewness[i] =
            s->cumulants[2 * s->length + i] / Cubic(sqrt(s->cumulants[1 * s->length + i]));
        s->kurtosis[i] = s->cumulants[3 * s->length + i] / Square(s->cumulants[1 * s->length + i]);
    }
    for (int i = 0; i < s->length; i++) {
        fprintf(o->b_kappa, "%.34g\t%.34g\t%.34g\t%.34g\t%.34g\n", s->time[i], s->height[i],
                s->skewness[i], s->kurtosis[i], s->cumulants[1 * s->length + i]);
    }
    for (int i = 0; i < s->length; i++) { // <- Method C Calculating.
        s->cumulants[0 * s->length + i] = s->height[i];
        s->cumulants[1 * s->length + i] = s->central[1 * s->length + i];
        s->cumulants[2 * s->length + i] = s->central[2 * s->length + i];
        s->cumulants[3 * s->length + i] =
            s->central[3 * s->length + i] - 3 * Square(s->central[1 * s->length + i]);
    }
    for (int i = 0; i < s->length; i++) {
        s->skewness[i] =
            s->cumulants[2 * s->length + i] / Cubic(sqrt(s->cumulants[1 * s->length + i]));
        s->kurtosis[i] = s->cumulants[3 * s->length + i] / Square(s->cumulants[1 * s->length + i]);
    }
    for (int i = 0; i < s->length; i++) {
        fprintf(o->b_m, "%.34g\t%.34g\t%.34g\t%.34g\t%.34g\n", s->time[i], s->height[i],
                s->skewness[i], s->kurtosis[i], s->cumulants[1 * s->length + i]);
    }

    return !ferror(o->ly) && !ferror(o->method_a) && !ferror(o->b_kappa) && !ferror(o->b_m) &&
           fprintf(o->run, "status=complete\n") >= 0;
}
int output_close(Output *o) {
    int ok = 1;
    FILE *files[] = {o->ly, o->method_a, o->b_kappa, o->b_m, o->run};
    for (int i = 0; i < 5; i++)
        if (files[i] && fclose(files[i]) != 0)
            ok = 0;
    *o = (Output){0};
    return ok;
}
