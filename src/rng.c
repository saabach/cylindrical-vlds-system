/* Algorithms and unsigned-integer operation order retained from legacy. */
#include "rng.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#define MAX_INT32 (double)4294967296.0
#define MAX_INT24 99999999
#define MIN_INT24 10000000
#define MBIG 1000000000
#define MSEED 161803398
#define MZ 0
#define FAC (1.0 / MBIG)
_Static_assert(UINT_MAX == 4294967295U, "Historical RNG requires 32-bit unsigned int");
static unsigned int RAN3_SEED;
static double Ran3(int idum);
static unsigned int RSeed(void);
// Assigned Flag's for Generator.
static unsigned int RProb1, RProb2, RProb3, RProb4, RProb5, RProb6,
    RProb7; // <- Random Probability Flag.
static unsigned int RLine1, RLine2, RLine3, RLine4, RLine5, RLine6, RLine7; // <- Random Line Flag.
static unsigned int RCol1, RCol2, RCol3, RCol4, RCol5, RCol6, RCol7; // <- Random Column Flag.
static unsigned int RNbr1, RNbr2, RNbr3, RNbr4, RNbr5, RNbr6, RNbr7; // <- Random Neighbor Flag.
// Randomic Number's Generator:
// Random Probability Generator.
static double RProb(unsigned int *RProb1, unsigned int *RProb2, unsigned int *RProb4,
                    unsigned int *RProb5, unsigned int *RProb3, unsigned int *RProb6,
                    unsigned int *RProb7);
// Randomic X-Axis Position Generator.
static double RLine(unsigned int *RLine1, unsigned int *RLine2, unsigned int *RLine4,
                    unsigned int *RLine5, unsigned int *RLine3, unsigned int *RLine6,
                    unsigned int *RLine7, int XAxis);
// Randomic Y-Axis Position Generator.
static double RCol(unsigned int *RCol1, unsigned int *RCol2, unsigned int *RCol4,
                   unsigned int *RCol5, unsigned int *RCol3, unsigned int *RCol6,
                   unsigned int *RCol7, int YAxis);
// Random Neighbor Generator.
static double RNbr(unsigned int *RNbr1, unsigned int *RNbr2, unsigned int *RNbr4,
                   unsigned int *RNbr5, unsigned int *RNbr3, unsigned int *RNbr6,
                   unsigned int *RNbr7, int NbrQuantity);

static double Ran3(int idum) { // <- Seed Generator (Ran3 Method).
    static int inext, inextp;
    static long ma[56];
    static int iff = 0;
    long mj, mk;
    int i, ii, k;

    if (idum < 0 || iff == 0) {
        iff = 1;
        mj = MSEED - (idum < 0 ? -idum : idum);
        mj %= MBIG;
        ma[55] = mj;
        mk = 1;
        for (i = 1; i <= 54; i++) {
            ii = (21 * i) % 55;
            ma[ii] = mk;
            mk = mj - mk;
            if (mk < MZ) {
                mk += MBIG;
            }
            mj = ma[ii];
        }
        for (k = 1; k <= 4; k++) {
            for (i = 1; i <= 55; i++) {
                ma[i] -= ma[1 + (i + 30) % 55];
                if (ma[i] < MZ) {
                    ma[i] += MBIG;
                }
            }
        }
        inext = 0;
        inextp = 31;
        idum = 1;
    }
    if (++inext == 56) {
        inext = 1;
    }
    if (++inextp == 56) {
        inextp = 1;
    }
    mj = ma[inext] - ma[inextp];
    if (mj < MZ) {
        mj += MBIG;
    }
    ma[inext] = mj;
    return (mj * FAC);
}
static unsigned int RSeed(void) { // <- Seed Setter.
    unsigned int RSeedParameter = 0;
    while ((RSeedParameter <= 1000000) || (RSeedParameter > 9999999)) {
        RSeedParameter = 10000000 * Ran3(RAN3_SEED);
    }
    if ((RSeedParameter % 2) == 0) {
        RSeedParameter--;
    }
    return (RSeedParameter);
}

// Randomic Number's Generator:
// Random Probability Generator.
static double RProb(unsigned int *RProb1, unsigned int *RProb2, unsigned int *RProb4,
                    unsigned int *RProb5, unsigned int *RProb3, unsigned int *RProb6,
                    unsigned int *RProb7) {
    unsigned int RStrProb;
    // 'RProb' Operation's.
    *RProb2 *= 1101513973;
    *RProb1 *= 1101513973;
    *RProb1 += (*RProb2 >> 31) * 65538 * (*RProb1);
    *RProb6 *= 1101513973;
    *RProb6 += (*RProb2 >> 31) * 16806 * (*RProb6);
    *RProb7 *= 1101513973;
    *RProb7 *= 16807;
    *RProb3 *= 1101513973;
    *RProb3 += ((~(*RProb2)) >> 31) * 16806 * (*RProb3);
    *RProb4 *= 1101513973;
    *RProb4 += ((~(*RProb2)) >> 31) * 1101513972 * (*RProb4);
    *RProb5 *= 1101513973;
    // Randomic String Former.
    RStrProb = ((*RProb1 >> 26) << 26) + ((*RProb6 >> 26) << 20) + ((*RProb4 >> 27) << 15) +
               ((*RProb3 >> 27) << 10) + ((*RProb5 >> 27) << 5) + ((*RProb7 >> 27));

    // 'RandomProb' Computing.
    return (((double)RStrProb) / MAX_INT32);
}
// Randomic X-Axis Position Generator.
static double RLine(unsigned int *RLine1, unsigned int *RLine2, unsigned int *RLine4,
                    unsigned int *RLine5, unsigned int *RLine3, unsigned int *RLine6,
                    unsigned int *RLine7, int XAxis) {
    unsigned int RStrLine;
    // 'RLine' Operation's.
    *RLine2 *= 1101513973;
    *RLine1 *= 1101513973;
    *RLine1 += (*RLine2 >> 31) * 65538 * (*RLine1);
    *RLine6 *= 1101513973;
    *RLine6 += (*RLine2 >> 31) * 16806 * (*RLine6);
    *RLine7 *= 1101513973;
    *RLine7 *= 16807;
    *RLine3 *= 1101513973;
    *RLine3 += ((~(*RLine2)) >> 31) * 16806 * (*RLine3);
    *RLine4 *= 1101513973;
    *RLine4 += ((~(*RLine2)) >> 31) * 1101513972 * (*RLine4);
    *RLine5 *= 1101513973;
    // Randomic String Former.
    RStrLine = ((*RLine1 >> 26) << 26) + ((*RLine6 >> 26) << 20) + ((*RLine4 >> 27) << 15) +
               ((*RLine3 >> 27) << 10) + ((*RLine5 >> 27) << 5) + ((*RLine7 >> 27));
    // 'RandomLine' Computing.
    return (int)((((double)RStrLine) / MAX_INT32) * XAxis);
}
// Randomic Y-Axis Position Generator.
static double RCol(unsigned int *RCol1, unsigned int *RCol2, unsigned int *RCol4,
                   unsigned int *RCol5, unsigned int *RCol3, unsigned int *RCol6,
                   unsigned int *RCol7, int YAxis) {
    unsigned int RStrCol;
    // 'RCol' Operation's.
    *RCol2 *= 1101513973;
    *RCol1 *= 1101513973;
    *RCol1 += (*RCol2 >> 31) * 65538 * (*RCol1);
    *RCol6 *= 1101513973;
    *RCol6 += (*RCol2 >> 31) * 16806 * (*RCol6);
    *RCol7 *= 1101513973;
    *RCol7 *= 16807;
    *RCol3 *= 1101513973;
    *RCol3 += ((~(*RCol2)) >> 31) * 16806 * (*RCol3);
    *RCol4 *= 1101513973;
    *RCol4 += ((~(*RCol2)) >> 31) * 1101513972 * (*RCol4);
    *RCol5 *= 1101513973;
    // Randomic String Former.
    RStrCol = ((*RCol1 >> 26) << 26) + ((*RCol6 >> 26) << 20) + ((*RCol4 >> 27) << 15) +
              ((*RCol3 >> 27) << 10) + ((*RCol5 >> 27) << 5) + ((*RCol7 >> 27));
    // 'RandomColumn' Computing.
    return (int)((((double)RStrCol) / MAX_INT32) * YAxis);
}
// Random Neighbor Generator.
static double RNbr(unsigned int *RNbr1, unsigned int *RNbr2, unsigned int *RNbr4,
                   unsigned int *RNbr5, unsigned int *RNbr3, unsigned int *RNbr6,
                   unsigned int *RNbr7, int NbrQuantity) {
    unsigned int RStrNbr;
    // 'RNbr' Operation's.
    *RNbr2 *= 1101513973;
    *RNbr1 *= 1101513973;
    *RNbr1 += (*RNbr2 >> 31) * 65538 * (*RNbr1);
    *RNbr6 *= 1101513973;
    *RNbr6 += (*RNbr2 >> 31) * 16806 * (*RNbr6);
    *RNbr7 *= 1101513973;
    *RNbr7 *= 16807;
    *RNbr3 *= 1101513973;
    *RNbr3 += ((~(*RNbr2)) >> 31) * 16806 * (*RNbr3);
    *RNbr4 *= 1101513973;
    *RNbr4 += ((~(*RNbr2)) >> 31) * 1101513972 * (*RNbr4);
    *RNbr5 *= 1101513973;
    // Randomic String Former.
    RStrNbr = ((*RNbr1 >> 26) << 26) + ((*RNbr6 >> 26) << 20) + ((*RNbr4 >> 27) << 15) +
              ((*RNbr3 >> 27) << 10) + ((*RNbr5 >> 27) << 5) + ((*RNbr7 >> 27));
    // 'RandomNeighbor' Computing.
    return (int)((((double)RStrNbr) / MAX_INT32) * NbrQuantity);
}

void rng_set_seed(unsigned int seed) { RAN3_SEED = seed; }

/* Same /dev/random odd-number selection; missing log is an empty history.
   Explicit --seed bypasses this non-deterministic selector. */
int rng_choose_seed(const char *path, unsigned int *seed) {
    FILE *random = fopen("/dev/random", "rb");
    FILE *history = fopen(path, "a+");
    if (!random || !history) {
        if (random)
            fclose(random);
        if (history)
            fclose(history);
        return 0;
    }
    for (;;) {
        unsigned int candidate, previous;
        if (fread(&candidate, sizeof(candidate), 1, random) != 1)
            break;
        candidate = (candidate % (MAX_INT24 - MIN_INT24 + 1)) + MIN_INT24;
        if ((candidate % 2) == 0)
            candidate--;
        rewind(history);
        int used = 0, result;
        while ((result = fscanf(history, "%u", &previous)) == 1)
            if (previous == candidate)
                used = 1;
        if (result != EOF || ferror(history))
            break;
        if (used)
            continue;
        if (fseek(history, 0, SEEK_END) || fprintf(history, "%u\n", candidate) < 0)
            break;
        int ok = fclose(history) == 0;
        fclose(random);
        if (!ok)
            return 0;
        *seed = candidate;
        return 1;
    }
    fclose(random);
    fclose(history);
    return 0;
}

void rng_initialize_sample(void) {
    // Seed Assignment's:
    // 'RProb' Flag Seed Assignment.
    RProb1 = RSeed(), RProb2 = RSeed(), RProb3 = RSeed(), RProb4 = RSeed(), RProb5 = RSeed(),
    RProb6 = RSeed(), RProb7 = RSeed();
    for (int i = 1; i < 1000; i++) { // <- 'RProb' Warming Up.
        RProb1 *= 1101513973, RProb2 *= 1101513973, RProb3 *= 1101513973, RProb4 *= 1101513973,
            RProb5 *= 1101513973, RProb6 *= 1101513973, RProb7 *= 1101513973;
    }
    // 'RLine' Flag Seed Assignment.
    RLine1 = RSeed(), RLine2 = RSeed(), RLine3 = RSeed(), RLine4 = RSeed(), RLine5 = RSeed(),
    RLine6 = RSeed(), RLine7 = RSeed();
    for (int i = 1; i < 1000; i++) { // <- 'RLine' Warming Up.
        RLine1 *= 1101513973, RLine2 *= 1101513973, RLine3 *= 1101513973, RLine4 *= 1101513973,
            RLine5 *= 1101513973, RLine6 *= 1101513973, RLine7 *= 1101513973;
    }
    // 'RCol' Flag Seed Assignment.
    RCol1 = RSeed(), RCol2 = RSeed(), RCol3 = RSeed(), RCol4 = RSeed(), RCol5 = RSeed(),
    RCol6 = RSeed(), RCol7 = RSeed();
    for (int i = 1; i < 1000; i++) { // <- 'RCol' Warming Up.
        RCol1 *= 1101513973, RCol2 *= 1101513973, RCol3 *= 1101513973, RCol4 *= 1101513973,
            RCol5 *= 1101513973, RCol6 *= 1101513973, RCol7 *= 1101513973;
    }
    // 'RNbr' Flag Seed Assignment.
    RNbr1 = RSeed(), RNbr2 = RSeed(), RNbr3 = RSeed(), RNbr4 = RSeed(), RNbr5 = RSeed(),
    RNbr6 = RSeed(), RNbr7 = RSeed();
    for (int i = 1; i < 1000; i++) { // <- 'RNbr' Warming Up.
        RNbr1 *= 1101513973, RNbr2 *= 1101513973, RNbr3 *= 1101513973, RNbr4 *= 1101513973,
            RNbr5 *= 1101513973, RNbr6 *= 1101513973, RNbr7 *= 1101513973;
    }
}
double rng_probability(void) {
    return RProb(&RProb1, &RProb2, &RProb3, &RProb4, &RProb5, &RProb6, &RProb7);
}
unsigned int rng_line(int n) {
    return RLine(&RLine1, &RLine2, &RLine3, &RLine4, &RLine5, &RLine6, &RLine7, n);
}
unsigned int rng_column(int n) {
    return RCol(&RCol1, &RCol2, &RCol3, &RCol4, &RCol5, &RCol6, &RCol7, n);
}
unsigned int rng_neighbor(void) {
    return RNbr(&RNbr1, &RNbr2, &RNbr3, &RNbr4, &RNbr5, &RNbr6, &RNbr7, 4);
}
