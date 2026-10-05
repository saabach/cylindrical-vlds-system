#ifndef CRS_DYNAMICS_H
#define CRS_DYNAMICS_H
#include "lattice.h"
#include <stdint.h>

typedef struct {
    uint64_t events, depositions, duplications, diffusion_steps;
} EventCounts;

int crsos_deposit(Lattice *lattice, int delta, EventCounts *counts);
#endif
