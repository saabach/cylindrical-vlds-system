#ifndef CRS_LATTICE_H
#define CRS_LATTICE_H
#include "config.h"

typedef struct {
    int lx, ly, capacity;
    int *height, *top, *bottom, *right, *left;
} Lattice;

int lattice_create(Lattice *lattice, const Config *config);
void lattice_reset(Lattice *lattice, int initial_ly);
int lattice_expand(Lattice *lattice, unsigned int column);
void lattice_destroy(Lattice *lattice);
#endif
