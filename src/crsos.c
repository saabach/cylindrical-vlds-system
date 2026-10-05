#include "crsos.h"
#include "rng.h"
#include "observables.h"
#include <limits.h>

int crsos_deposit(Lattice *l, int delta, EventCounts *counts) {
    unsigned int row = rng_line(l->lx);
    unsigned int column = rng_column(l->ly);
    while (1) {
        if (l->height[row * l->capacity + column] == INT_MAX)
            return 0;
        int height = l->height[row * l->capacity + column] + 1;
        int neighbors[4] = {
            l->height[l->top[row] * l->capacity + column],
            l->height[l->bottom[row] * l->capacity + column],
            l->height[row * l->capacity + l->right[column]],
            l->height[row * l->capacity + l->left[column]],
        };
        for (int i = 0; i < 4; i++) {
            if (Absolute(neighbors[i] - height) > delta)
                goto diffusion;
        }
        break;
    diffusion:
        switch (rng_neighbor()) {
        case 0:
            row = l->top[row];
            break;
        case 1:
            row = l->bottom[row];
            break;
        case 2:
            column = l->right[column];
            break;
        case 3:
            column = l->left[column];
            break;
        }
        counts->diffusion_steps++;
    }
    l->height[row * l->capacity + column] += 1;
    counts->depositions++;
    return 1;
}
