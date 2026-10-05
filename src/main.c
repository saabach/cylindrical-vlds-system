#define _POSIX_C_SOURCE 200809L
#include "config.h"
#include "lattice.h"
#include "crsos.h"
#include "rng.h"
#include "observables.h"
#include "sampling.h"
#include "output.h"
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

static void usage(void) {
    puts("Cylindrical conservative RSOS growth\n"
         "Usage: cylindrical-crsos [options]\n"
         "  --delta N        RSOS height restriction (default 1)\n"
         "  --max-time N     Integer final time (default 2000)\n"
         "  --samples N      Realizations per process (default 10)\n"
         "  --lx N           Fixed x length (default 32768)\n"
         "  --initial-ly N   Initial growing length (default 4)\n"
         "  --omega X        Expansion event rate (default 1)\n"
         "  --max-columns N  Allocated column capacity (default 2800)\n"
         "  --seed N         Positive odd seed <=99999999; otherwise /dev/random\n"
         "  --output-dir DIR Existing parent, new/existing run directory (default output)\n"
         "  --help           Show this help\n"
         "Default dimensions/time are the historical production settings.\n"
         "Use reduced dimensions/time for a quick test; see README.md.");
}

static int positive_int(const char *text, int *value) {
    char *end;
    errno = 0;
    long v = strtol(text, &end, 10);
    if (errno || !*text || *end || v < 1 || v > INT_MAX)
        return 0;
    *value = (int)v;
    return 1;
}

static int parse(int argc, char **argv, Config *c) {
    for (int i = 1; i < argc; i++) {
        const char *key = argv[i];
        if (!strcmp(key, "--help")) {
            usage();
            return 1;
        }
        if (i + 1 == argc) {
            fprintf(stderr, "Missing value for %s\n", key);
            return -1;
        }
        const char *value = argv[++i];
        int *dest = NULL;
        if (!strcmp(key, "--delta"))
            dest = &c->delta;
        else if (!strcmp(key, "--max-time"))
            dest = &c->max_time;
        else if (!strcmp(key, "--samples"))
            dest = &c->samples;
        else if (!strcmp(key, "--lx"))
            dest = &c->lx;
        else if (!strcmp(key, "--initial-ly"))
            dest = &c->initial_ly;
        else if (!strcmp(key, "--max-columns"))
            dest = &c->max_columns;
        else if (!strcmp(key, "--output-dir")) {
            if (!*value || strlen(value) > 3900)
                return -1;
            c->output_dir = value;
        } else if (!strcmp(key, "--seed")) {
            int seed;
            if (!positive_int(value, &seed) || seed > 99999999 || !(seed % 2)) {
                fputs("Seed must be positive, odd and <=99999999.\n", stderr);
                return -1;
            }
            c->seed = (unsigned int)seed;
            c->seed_given = 1;
        } else if (!strcmp(key, "--omega")) {
            char *end;
            errno = 0;
            c->omega = strtod(value, &end);
            if (errno || !*value || *end || !isfinite(c->omega) || c->omega < 0 ||
                c->omega > INT_MAX) {
                fputs("Omega must be finite and in [0, INT_MAX].\n", stderr);
                return -1;
            }
        } else {
            fprintf(stderr, "Unknown option: %s\n", key);
            return -1;
        }
        if (dest && !positive_int(value, dest)) {
            fprintf(stderr, "Invalid integer for %s\n", key);
            return -1;
        }
    }
    /* Preserve int indexing/arithmetic while rejecting overflow rather than UB. */
    if (c->lx < 2 || c->initial_ly < 2 || c->max_columns < c->initial_ly ||
        c->lx > INT_MAX / c->max_columns || c->samples > INT_MAX / c->max_columns ||
        c->max_time > INT_MAX / 4 || (size_t)c->lx * c->max_columns > SIZE_MAX / sizeof(int) ||
        (size_t)c->max_time > SIZE_MAX / (4 * sizeof(double))) {
        fputs("Invalid geometry, capacity or integer/allocation overflow.\n", stderr);
        return -1;
    }
    return 0;
}

int main(int argc, char **argv) {
    Config c = {.delta = 1,
                .max_time = 2000,
                .samples = 10,
                .lx = 32768,
                .initial_ly = 4,
                .max_columns = 2800,
                .omega = 1,
                .output_dir = "output"};
    int result = parse(argc, argv, &c);
    if (result)
        return result < 0 ? 2 : 0;
    if (mkdir(c.output_dir, 0777) != 0 && errno != EEXIST) {
        perror("output directory");
        return 1;
    }
    if (!c.seed_given) {
        char path[4096];
        snprintf(path, sizeof(path), "%s/Choiced-Ran3-OddSeed.dat", c.output_dir);
        if (!rng_choose_seed(path, &c.seed)) {
            fputs("Cannot select/log seed.\n", stderr);
            return 1;
        }
    }
    rng_set_seed(c.seed);
    Output output = {0};
    Lattice lattice = {0};
    Sampling sampling = {0};
    int ok = 0;
    if (!output_open(&output, &c)) {
        fputs("Cannot create output files (existing files are not overwritten).\n", stderr);
        goto done;
    }
    if (!lattice_create(&lattice, &c) || !sampling_create(&sampling, c.max_time)) {
        fputs("Allocation failed.\n", stderr);
        goto done;
    }
    printf("Cylindrical C-RSOS: delta=%d, seed=%u, samples=%d\n", c.delta, c.seed, c.samples);
    for (int sample = 1; sample <= c.samples; sample++) {
        rng_initialize_sample();
        lattice_reset(&lattice, c.initial_ly);
        double time = 0;
        int measurement = 1;
        EventCounts counts = {0};
        do {
            time += 1.0 / (lattice.lx * lattice.ly + c.omega);
            double probability = rng_probability();
            /* Historical float cast is scientifically significant: retain it. */
            double deposition_probability =
                (float)(lattice.lx * lattice.ly) / ((lattice.lx * lattice.ly) + c.omega);
            counts.events++;
            if (probability <= deposition_probability) {
                if (!crsos_deposit(&lattice, c.delta, &counts)) {
                    fputs("Height overflow.\n", stderr);
                    goto done;
                }
            } else {
                unsigned int column = rng_column(lattice.ly);
                if (!lattice_expand(&lattice, column)) {
                    fputs("Column capacity exhausted; increase --max-columns.\n", stderr);
                    goto done;
                }
                counts.duplications++;
            }
            if (time >= measurement) {
                Moments moments = observables_measure(&lattice);
                sampling_add(&sampling, measurement - 1, time, lattice.ly, &moments);
#ifdef CRS_TEST_TRACE
                trace_measure(sample, measurement, time, lattice.lx, lattice.ly, lattice.capacity,
                              lattice.height, lattice.top, lattice.bottom, lattice.left,
                              lattice.right, counts.events, counts.depositions, counts.duplications,
                              counts.diffusion_steps);
#endif
                measurement++;
            }
        } while (time < c.max_time);
        if (!output_sample(&output, sample, time, lattice.ly, &counts))
            goto done;
    }
    sampling_normalize(&sampling, c.samples);
    ok = output_write(&output, &sampling);
done:
    lattice_destroy(&lattice);
    sampling_destroy(&sampling);
    if (!output_close(&output))
        ok = 0;
    if (!ok) {
        fputs("Run incomplete; do not use partial output.\n", stderr);
        return 1;
    }
    puts("Simulation finished.");
    return 0;
}
