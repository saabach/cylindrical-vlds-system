#include "lattice.h"
#include <stdlib.h>
#include <string.h>

int lattice_create(Lattice *l, const Config *c) {
    *l = (Lattice){.lx = c->lx, .capacity = c->max_columns};
    l->height = malloc((size_t)l->lx * l->capacity * sizeof(int));
    l->top = malloc((size_t)l->lx * sizeof(int));
    l->bottom = malloc((size_t)l->lx * sizeof(int));
    l->right = malloc((size_t)l->capacity * sizeof(int));
    l->left = malloc((size_t)l->capacity * sizeof(int));
    if (!l->height || !l->top || !l->bottom || !l->right || !l->left) {
        lattice_destroy(l);
        return 0;
    }
    return 1;
}

void lattice_reset(Lattice *l, int initial_ly) {
    l->ly = initial_ly;
    memset(l->height, 0, (size_t)l->lx * l->capacity * sizeof(int));
    memset(l->top, 0, (size_t)l->lx * sizeof(int));
    memset(l->bottom, 0, (size_t)l->lx * sizeof(int));
    memset(l->right, 0, (size_t)l->capacity * sizeof(int));
    memset(l->left, 0, (size_t)l->capacity * sizeof(int));
    for (int i = 0; i < l->lx; i++) {
        l->top[i] = (i == 0) ? l->lx - 1 : i - 1;
        l->bottom[i] = (i == l->lx - 1) ? 0 : i + 1;
    }
    for (int i = 0; i < l->ly; i++) {
        l->right[i] = (i == l->ly - 1) ? 0 : i + 1;
        l->left[i] = (i == 0) ? l->ly - 1 : i - 1;
    }
}

int lattice_expand(Lattice *l, unsigned int column) {
    if (l->ly >= l->capacity)
        return 0;
    for (int i = 0; i < l->lx; i++)
        l->height[i * l->capacity + l->ly] = l->height[i * l->capacity + column];
    /* Append in memory, splice immediately after column in the physical ring. */
    l->left[l->right[column]] = l->ly;
    l->right[l->ly] = l->right[column];
    l->right[column] = l->ly;
    l->left[l->ly] = column;
    l->ly++;
    return 1;
}

void lattice_destroy(Lattice *l) {
    free(l->height);
    free(l->top);
    free(l->bottom);
    free(l->right);
    free(l->left);
    *l = (Lattice){0};
}
