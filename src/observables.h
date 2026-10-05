#ifndef CRS_OBSERVABLES_H
#define CRS_OBSERVABLES_H
#include "lattice.h"

typedef struct {
    long double height, m2, m3, m4;
    long double k1, k2, k3, k4, skewness, kurtosis;
} Moments;

Moments observables_measure(const Lattice *lattice);
/* Double arguments/results intentionally retain historical rounding. */
int Absolute(int value);
double Square(double value);
double Cubic(double value);
double Quadric(double value);
#endif
