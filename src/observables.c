#include "observables.h"
#include <math.h>

Moments observables_measure(const Lattice *l) {
    long double avgH, avgSquaredM, avgCubicM, avgQuadricM;
    long double FirstK, SecondK, ThirdK, FourthK, Skewness, Kurtosis;
    // Normalizing Raw and Central Moment's Input.
    avgH = 0, avgSquaredM = 0, avgCubicM = 0, avgQuadricM = 0;
    for (int i = 0; i < l->lx; i++) { // <- 1st Raw Moment.
        for (int j = 0; j < l->ly; j++) {
            avgH += (long double)l->height[i * l->capacity + j] / (long double)(l->lx * l->ly);
        }
    }
    for (int i = 0; i < l->lx; i++) { // <- Central Moment's.
        for (int j = 0; j < l->ly; j++) {
            avgSquaredM += Square((long double)(l->height[i * l->capacity + j] - avgH)) /
                           (long double)(l->lx * l->ly);
            avgCubicM += Cubic((long double)(l->height[i * l->capacity + j] - avgH)) /
                         (long double)(l->lx * l->ly);
            avgQuadricM += Quadric((long double)(l->height[i * l->capacity + j] - avgH)) /
                           (long double)(l->lx * l->ly);
        }
    }
    // Obtaining Parameter's.
    FirstK = avgH;
    SecondK = avgSquaredM;
    ThirdK = avgCubicM;
    FourthK = avgQuadricM - 3 * Square(avgSquaredM);
    Skewness = ThirdK / Cubic(sqrt(SecondK));
    Kurtosis = FourthK / Square(SecondK);

    return (Moments){avgH,    avgSquaredM, avgCubicM, avgQuadricM, FirstK,
                     SecondK, ThirdK,      FourthK,   Skewness,    Kurtosis};
}

// Arithmetic Function's.
int Absolute(int a) {
    int Mask = a >> (sizeof(int) * 8 - 1);
    return (a + Mask) ^ Mask;
}
double Square(double x) { return (x * x); }
double Cubic(double y) { return (y * y * y); }
double Quadric(double z) { return (z * z * z * z); }
