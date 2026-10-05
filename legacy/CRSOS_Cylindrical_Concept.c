// Library Importation.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <time.h>

// Global Attributes.
#define MAX_INT32 (double) 4294967296.0 // <- 2^32 Bit's.
#define MAX_INT24 (int) 99999999 // Max Value with 24 Bit's.
#define MIN_INT24 (int) 10000000 // Min Value with 24 Bit's.
#define MBIG 1000000000
#define MSEED 161803398
#define MZ 0
#define FAC (1.0/MBIG)
unsigned int RAN3_SEED; // <- Assigned Flag for Ran3.
#define SAVE_FILE "Choiced-Ran3-OddSeed.dat" // <- Choiced Seed List for Ran3.

// Seed Generator's.
int ChoicedOddNum (unsigned int OddNum);
void SaveOddNum (unsigned int OddNum);
unsigned int GetOddNum ();
__attribute__((constructor))
void Ran3_SeedAssignment (void) {
  RAN3_SEED = GetOddNum();
}
double Ran3 (int idum); // <- Seed Generator (Ran3 Method).
unsigned int RSeed (); // <- Seed Setter.

// Assigned Flag's for Generator.
unsigned int RProb1, RProb2, RProb3, RProb4, RProb5, RProb6, RProb7; // <- Random Probability Flag.
unsigned int RLine1, RLine2, RLine3, RLine4, RLine5, RLine6, RLine7; // <- Random Line Flag.
unsigned int RCol1, RCol2, RCol3, RCol4, RCol5, RCol6, RCol7; // <- Random Column Flag.
unsigned int RNbr1, RNbr2, RNbr3, RNbr4, RNbr5, RNbr6, RNbr7; // <- Random Neighbor Flag.
unsigned int RandomProb, RandomLine, RandomColumn, RandomNeighbor; // <- Randomic Number's.

// Randomic Number's Generator:
// Random Probability Generator. 
double RProb (unsigned int *RProb1, unsigned int *RProb2, unsigned int *RProb4, unsigned int *RProb5, \
unsigned int *RProb3, unsigned int *RProb6, unsigned int *RProb7);
// Randomic X-Axis Position Generator. 
double RLine (unsigned int *RLine1, unsigned int *RLine2, unsigned int *RLine4, unsigned int *RLine5, \
unsigned int *RLine3, unsigned int *RLine6, unsigned int *RLine7, int XAxis);
// Randomic Y-Axis Position Generator.
double RCol (unsigned int *RCol1, unsigned int *RCol2, unsigned int *RCol4, unsigned int *RCol5, \
unsigned int *RCol3, unsigned int *RCol6, unsigned int *RCol7, int YAxis);
// Random Neighbor Generator.
double RNbr (unsigned int *RNbr1, unsigned int *RNbr2, unsigned int *RNbr4, unsigned int *RNbr5, \
unsigned int *RNbr3, unsigned int *RNbr6, unsigned int *RNbr7, int NbrQuantity);

// Arithmetic Function's.
int Absolute (int a);
double Square (double x);
double Cubic (double y);
double Quadric (double z);

// Algorythm Scope.
int main (int argc, char **argv) {
  // Variable Declaration.
  int M, maxSamples, maxColumns, maxTime, initLy, Lx, Ly, sampleCount, timeParameter, heightParameter, \
  hours, minutes;
  double timeAmount, depProbability, Omega, RandomProb;
  long double avgH, avgSquaredM, avgCubicM, avgQuadricM, FirstK, SecondK, ThirdK, FourthK, Skewness, \
  Kurtosis;
  struct timespec start, end;

  // Information's Request.
  printf("Behavior: Expanded Cylindrical Geometry Deposition.\n");
  printf("Model: Conserved Restricted Solid-on-Solid (C-RSOS).\n");
  // Pre-Configuration.
  maxSamples = 10, maxTime = 2000; // Launch Pre-Configuration.
  Omega = 1, M = 1; // RSOS Pre-Configuration.
  Lx = 32768, initLy = 4, maxColumns = 2800; // Substract Pre-Configuration.

  // Contiguous Substract's Memory Dedicate.
  int *Substract = (int *)malloc(Lx * maxColumns * sizeof(int));
  double *CentralMomentsDB = (double *)calloc(4 * maxTime, sizeof(double));
  double *CumulantsDB = (double *)calloc(4 * maxTime, sizeof(double));
  // Neighbor Array's Memory Dedicate.
  int *TopNeighbor = (int *)malloc(Lx * sizeof(int));
  int *BottomNeighbor = (int *)malloc(Lx * sizeof(int));
  int *RightNeighbor = (int *)malloc(maxColumns * sizeof(int));
  int *LeftNeighbor = (int *)malloc(maxColumns * sizeof(int));
  // Measurement Array's Memory Dedicate.
  double *MeasurementTimeDB = (double *)calloc(maxTime, sizeof(double));
  int *MeasurementColumnDB = (int *)calloc(maxTime, sizeof(int));
  double *RawMomentsDB = (double *)calloc(maxTime, sizeof(double));
  double *SkewnessDB = (double *)calloc(maxTime, sizeof(double));
  double *KurtosisDB = (double *)calloc(maxTime, sizeof(double)); 

  clock_gettime(CLOCK_MONOTONIC, &start); // <- Start the Cronometer.

  for (sampleCount = 1; sampleCount <= maxSamples; sampleCount++) {
    // Initial Y-Axis.
    Ly = initLy;
    // Reseting Measurement Parameter's.
    FirstK = 0, SecondK = 0, ThirdK = 0, FourthK = 0;
    Skewness = 0, Kurtosis = 0;
    // Seed Assignment's:
    // 'RProb' Flag Seed Assignment.
    RProb1 = RSeed(), RProb2 = RSeed(), RProb3 = RSeed(), RProb4 = RSeed(),  RProb5 = RSeed(), \
    RProb6 = RSeed(), RProb7 = RSeed();
    for (int i = 1; i < 1000; i++) { // <- 'RProb' Warming Up.
      RProb1 *= 1101513973, RProb2 *= 1101513973, RProb3 *= 1101513973, RProb4 *= 1101513973, \
      RProb5 *= 1101513973, RProb6 *= 1101513973, RProb7 *= 1101513973;
    }
    // 'RLine' Flag Seed Assignment.
    RLine1 = RSeed(), RLine2 = RSeed(), RLine3 = RSeed(), RLine4 = RSeed(), RLine5 = RSeed(), \
    RLine6 = RSeed(), RLine7 = RSeed();
    for (int i = 1; i < 1000; i++) { // <- 'RLine' Warming Up.
      RLine1 *= 1101513973, RLine2 *= 1101513973, RLine3 *= 1101513973, RLine4 *= 1101513973, \
      RLine5 *= 1101513973, RLine6 *= 1101513973, RLine7 *= 1101513973;
    }
    // 'RCol' Flag Seed Assignment.
    RCol1 = RSeed(), RCol2 = RSeed(), RCol3 = RSeed(), RCol4 = RSeed(), RCol5 = RSeed(),
    RCol6 = RSeed(), RCol7 = RSeed();
    for (int i = 1; i < 1000; i++) { // <- 'RCol' Warming Up.
      RCol1 *= 1101513973, RCol2 *= 1101513973, RCol3 *= 1101513973, RCol4 *= 1101513973, \
      RCol5 *= 1101513973, RCol6 *= 1101513973, RCol7 *= 1101513973;
    }
    // 'RNbr' Flag Seed Assignment.
    RNbr1 = RSeed(), RNbr2 = RSeed(), RNbr3 = RSeed(), RNbr4 = RSeed(), RNbr5 = RSeed(),
    RNbr6 = RSeed(), RNbr7 = RSeed();
    for (int i = 1; i < 1000; i++) { // <- 'RNbr' Warming Up.
      RNbr1 *= 1101513973, RNbr2 *= 1101513973, RNbr3 *= 1101513973, RNbr4 *= 1101513973, \
      RNbr5 *= 1101513973, RNbr6 *= 1101513973, RNbr7 *= 1101513973;
    }
    // Array's Reseting.
    memset(Substract, 0, Lx * maxColumns * sizeof(int));
    memset(TopNeighbor, 0, Lx * sizeof(int));
    memset(BottomNeighbor, 0, Lx * sizeof(int));
    memset(RightNeighbor, 0, maxColumns * sizeof(int));
    memset(LeftNeighbor, 0, maxColumns * sizeof(int));
    // Neighbor's Assignment.
    for (int i = 0; i < Lx; i++) { // X-Axis Neighbor's.
      TopNeighbor[i] = (i == 0) ? Lx - 1 : i - 1;
      BottomNeighbor[i] = (i == Lx - 1) ? 0 : i + 1;  
    }
    for (int i = 0; i < Ly; i++) { // Y-Axis Neighbor's.
      RightNeighbor[i] = (i == Ly - 1) ? 0 : i + 1;
      LeftNeighbor[i] = (i == 0) ? Ly - 1 : i - 1;
    }
    // Reseting Time Parameter's.
    timeAmount = 0, timeParameter = 1;
    // Cronology Definement.
    do {
      timeAmount += 1.0 / (Lx*Ly + Omega); // <- Time Stamp.
      RandomProb = RProb(&RProb1, &RProb2, &RProb3, &RProb4, &RProb5, &RProb6, &RProb7);
      depProbability = (float) (Lx*Ly) / ((Lx*Ly) + Omega); // <- Deposition Probability.
      if (RandomProb <= depProbability) { // <- Deposition Process.
        // Randomic Position Assortment.
        RandomLine = RLine(&RLine1, &RLine2, &RLine3, &RLine4, &RLine5, &RLine6, &RLine7, Lx);
        RandomColumn = RCol(&RCol1, &RCol2, &RCol3, &RCol4, &RCol5, &RCol6, &RCol7, Ly);
        while (1) { // <- Conservative RSOS Rule.
          heightParameter = Substract[RandomLine * maxColumns + RandomColumn] + 1; // <- Current Pseudo-Height.
          int NeighborHeight[4] = {
            Substract[TopNeighbor[RandomLine] * maxColumns + RandomColumn],
            Substract[BottomNeighbor[RandomLine] * maxColumns + RandomColumn],
            Substract[RandomLine * maxColumns + RightNeighbor[RandomColumn]],
            Substract[RandomLine * maxColumns + LeftNeighbor[RandomColumn]],
          };
          for (int i = 0; i < 4; i++) { // <- RSOS Rule
            if (Absolute(NeighborHeight[i] - heightParameter) > M) {
              goto Diffusion;
            }
          }
          break; // <- Break Point.
          Diffusion: // <- C-RSOS Flag.
          RandomNeighbor = RNbr(&RNbr1, &RNbr2, &RNbr3, &RNbr4, &RNbr5, &RNbr6, &RNbr7, 4);
          switch (RandomNeighbor) { // <- Particle Diffusion.
            case 0: RandomLine = TopNeighbor[RandomLine]; break;
            case 1: RandomLine = BottomNeighbor[RandomLine]; break;
            case 2: RandomColumn = RightNeighbor[RandomColumn]; break;
            case 3: RandomColumn = LeftNeighbor[RandomColumn]; break;
          }
        }
        Substract[RandomLine * maxColumns + RandomColumn] += 1; // <- Particle Deposition.
      }
      else { // <- Duplication Process.
        RandomColumn = RCol(&RCol1, &RCol2, &RCol3, &RCol4, &RCol5, &RCol6, &RCol7, Ly);
        for (int i = 0; i < Lx; i++) { // <- Assorted Column Duplicate.
          Substract[i * maxColumns + Ly] = Substract[i * maxColumns + RandomColumn];
        }
        // Column Neighbor's Update.
        LeftNeighbor[RightNeighbor[RandomColumn]] = Ly;
        RightNeighbor[Ly] = RightNeighbor[RandomColumn];
        RightNeighbor[RandomColumn] = Ly;
        LeftNeighbor[Ly] = RandomColumn;
        Ly++; // <- Column Quantity Update.
      } 
      if (timeAmount >= timeParameter) { // <- Analysis Process.
        // Normalizing Raw and Central Moment's Input.
        avgH = 0, avgSquaredM = 0, avgCubicM = 0, avgQuadricM = 0;
        for (int i = 0; i < Lx; i++) { // <- 1st Raw Moment.
          for(int j = 0; j < Ly; j++) {
            avgH += (long double)Substract[i * maxColumns + j]/(long double)(Lx*Ly);
          }
        }
        for (int i = 0; i < Lx; i++) { // <- Central Moment's.
          for (int j = 0; j < Ly; j++) {
            avgSquaredM += Square((long double)(Substract[i * maxColumns + j] - avgH))/(long double)(Lx*Ly);
            avgCubicM += Cubic((long double)(Substract[i * maxColumns + j] - avgH))/(long double)(Lx*Ly);
            avgQuadricM += Quadric((long double)(Substract[i * maxColumns + j] - avgH))/(long double)(Lx*Ly);
          }
        }
        // Obtaining Parameter's.
        FirstK = avgH;
        SecondK = avgSquaredM;
        ThirdK = avgCubicM;
        FourthK = avgQuadricM - 3*Square(avgSquaredM);
        Skewness = ThirdK/Cubic(sqrt(SecondK));
        Kurtosis = FourthK/Square(SecondK);
        // Collecting Measures.
        MeasurementTimeDB[timeParameter - 1] += timeAmount;
        MeasurementColumnDB[timeParameter - 1] += Ly;
        RawMomentsDB[timeParameter - 1] += avgH;
        CentralMomentsDB[1 * maxTime + (timeParameter - 1)] += avgSquaredM;
        CentralMomentsDB[2 * maxTime + (timeParameter - 1)] += avgCubicM;
        CentralMomentsDB[3 * maxTime + (timeParameter - 1)] += avgQuadricM;
        CumulantsDB[0 * maxTime + (timeParameter - 1)] += FirstK;
        CumulantsDB[1 * maxTime + (timeParameter - 1)] += SecondK;
        CumulantsDB[2 * maxTime + (timeParameter - 1)] += ThirdK;
        CumulantsDB[3 * maxTime + (timeParameter - 1)] += FourthK;
        SkewnessDB[timeParameter - 1] += Skewness;
        KurtosisDB[timeParameter - 1] += Kurtosis;
        timeParameter++; // <- Time Parameter Update.
      }
    } while (timeAmount < maxTime);
  }
  // Normalizing the Input's.
  for (int i = 0; i < maxTime; i++) { 
    MeasurementTimeDB[i] /= maxSamples;
    MeasurementColumnDB[i] /= maxSamples;
    RawMomentsDB[i] /= maxSamples;
    SkewnessDB[i] /= maxSamples;
    KurtosisDB[i] /= maxSamples;
    for (int j = 0; j < 4; j++) {
      CentralMomentsDB[j * maxTime + i] /= maxSamples;
      CumulantsDB[j * maxTime + i] /= maxSamples;
    }
  }

  // Data Management: Archive's Creating.
  // Ly's Size Information's Transcript.
  FILE *LySizeDatabase = fopen("LySize-C-RSOS-Simulation_01_Sample.dat", "w");
  for (int i = 0; i < maxTime; i++) {
    fprintf(LySizeDatabase, "%.34g\t%d\n", MeasurementTimeDB[i], MeasurementColumnDB[i]);
  }
  fclose(LySizeDatabase); // <- Ly Size's DB Create.
  FILE *MethodADatabase = fopen("MethodA-C-RSOS-Simulation_01_Sample.dat", "w");
  for (int i = 0; i < maxTime; i++) {
    fprintf(MethodADatabase, "%.34g\t%.34g\t%.34g\t%.34g\t%.34g\n", MeasurementTimeDB[i], RawMomentsDB[i], SkewnessDB[i], KurtosisDB[i], CumulantsDB[1 * maxTime + i]);
  }
  fclose(MethodADatabase);  // <- Method A DB Create.
  for (int i = 0; i < maxTime; i++) { // <- Method B Calculating.
    SkewnessDB[i] = CumulantsDB[2 * maxTime + i]/Cubic(sqrt(CumulantsDB[1 * maxTime + i]));
    KurtosisDB[i] = CumulantsDB[3 * maxTime + i]/Square(CumulantsDB[1 * maxTime + i]);
  }
  FILE *MethodBDatabase = fopen("MethodB-C-RSOS-Simulation_01_Sample.dat", "w");
  for (int i = 0; i < maxTime; i++) {
    fprintf(MethodBDatabase, "%.34g\t%.34g\t%.34g\t%.34g\t%.34g\n", MeasurementTimeDB[i], RawMomentsDB[i], SkewnessDB[i], KurtosisDB[i], CumulantsDB[1 * maxTime + i]);
  }
  fclose(MethodBDatabase); // <- Method B DB Create.
  for (int i = 0; i < maxTime; i++) { // <- Method C Calculating.
    CumulantsDB[0 * maxTime + i] = RawMomentsDB[i];
    CumulantsDB[1 * maxTime + i] = CentralMomentsDB[1 * maxTime + i];
    CumulantsDB[2 * maxTime + i] = CentralMomentsDB[2 * maxTime + i];
    CumulantsDB[3 * maxTime + i] = CentralMomentsDB[3 * maxTime + i] - 3*Square(CentralMomentsDB[1 * maxTime + i]);
  }
  for (int i = 0; i < maxTime; i++) {
    SkewnessDB[i] = CumulantsDB[2 * maxTime + i]/Cubic(sqrt(CumulantsDB[1 * maxTime + i]));
    KurtosisDB[i] = CumulantsDB[3 * maxTime + i]/Square(CumulantsDB[1 * maxTime + i]);
  }
  FILE *MethodCDatabase = fopen("MethodC-C-RSOS-Simulation_01_Sample.dat", "w");
  for (int i = 0; i < maxTime; i++) {
    fprintf(MethodCDatabase, "%.34g\t%.34g\t%.34g\t%.34g\t%.34g\n", MeasurementTimeDB[i], RawMomentsDB[i], SkewnessDB[i], KurtosisDB[i], CumulantsDB[1 * maxTime + i]);
  }
  fclose(MethodCDatabase); // <- Method C DB Create.

  clock_gettime(CLOCK_MONOTONIC, &end); // <- Finish the Cronometer.
  // Time Calculating.
  double duration = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
  hours = (int)(duration / 3600);
  minutes = (int)((duration - hours * 3600) / 60);
  double seconds = duration - hours * 3600 - minutes * 60;

  // Contiguos Memory Deallocation.
  free(Substract); free(CentralMomentsDB); free(CumulantsDB);
  // Neighbor's Memory Deallocation.
  free(TopNeighbor); free(BottomNeighbor); free(RightNeighbor); free(LeftNeighbor);
  // Measurement Memory Deallocation.
  free(MeasurementTimeDB); free(MeasurementColumnDB); free(RawMomentsDB); free(SkewnessDB); free(KurtosisDB);
  // Exit Message. 
  printf("Elapsed Time: %dh %dm %.3fs.\n", hours, minutes, seconds);
  printf("Simulation Finished.\n");

  return 0;
}

// Seed Generator's.
int ChoicedOddNum (unsigned int OddNum) {
  FILE *file = fopen(SAVE_FILE, "r");
  unsigned int ChoicedOddNumber;
  while (fscanf(file, "%d", &ChoicedOddNumber) == 1) {
    if (ChoicedOddNumber == OddNum) {
      fclose(file);
      return 1;
    }
  }
  fclose(file);
  return 0;
}
void SaveOddNum (unsigned int OddNum) {
  FILE *file = fopen(SAVE_FILE, "a");
  fprintf(file, "%d\n", OddNum);
  fclose(file);
}
unsigned int GetOddNum () {
  FILE *f = fopen("/dev/random", "r");
  unsigned int OddNum;
  while (1) {
    fread(&OddNum, sizeof(OddNum), 1, f);
    OddNum = (OddNum % (MAX_INT24 - MIN_INT24 + 1)) + MIN_INT24;
    if ((OddNum % 2) == 0) {
      OddNum--;
    }
    if (!ChoicedOddNum(OddNum)) {
      fclose(f);
      SaveOddNum(OddNum);
      return OddNum;
    }
  }
}
double Ran3 (int idum) { // <- Seed Generator (Ran3 Method).
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
      ii = (21*i) % 55;
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
  return (mj*FAC);
}
unsigned int RSeed () { // <- Seed Setter.
  unsigned int RSeedParameter = 0;
  while ((RSeedParameter <= 1000000) || (RSeedParameter > 9999999)) {
    RSeedParameter = 10000000*Ran3(RAN3_SEED);
  }
  if ((RSeedParameter % 2) == 0) {
    RSeedParameter--;
  }
  return (RSeedParameter);
}

// Randomic Number's Generator:
// Random Probability Generator.
double RProb (unsigned int *RProb1, unsigned int *RProb2, unsigned int *RProb4, unsigned int *RProb5, \
unsigned int *RProb3, unsigned int *RProb6, unsigned int *RProb7) {
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
  RStrProb = ((*RProb1 >> 26) << 26) + ((*RProb6 >> 26) << 20) + ((*RProb4 >> 27) << 15) + ((*RProb3 >> 27) << 10) + ((*RProb5 >> 27) << 5) + \
  ((*RProb7 >> 27));

  // 'RandomProb' Computing.
  return (((double)RStrProb) / MAX_INT32);
}
// Randomic X-Axis Position Generator.
double RLine (unsigned int *RLine1, unsigned int *RLine2, unsigned int *RLine4, unsigned int *RLine5, \
unsigned int *RLine3, unsigned int *RLine6, unsigned int *RLine7, int XAxis) {
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
  RStrLine = ((*RLine1 >> 26) << 26) + ((*RLine6 >> 26) << 20) + ((*RLine4 >> 27) << 15) + ((*RLine3 >> 27) << 10) + ((*RLine5 >> 27) << 5) + \
  ((*RLine7 >> 27));
  // 'RandomLine' Computing.
  return (int)((((double)RStrLine) / MAX_INT32) * XAxis);
}
// Randomic Y-Axis Position Generator.
double RCol (unsigned int *RCol1, unsigned int *RCol2, unsigned int *RCol4, unsigned int *RCol5, \
unsigned int *RCol3, unsigned int *RCol6, unsigned int *RCol7, int YAxis) {
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
  RStrCol = ((*RCol1 >> 26) << 26) + ((*RCol6 >> 26) << 20) + ((*RCol4 >> 27) << 15) + ((*RCol3 >> 27) << 10) + ((*RCol5 >> 27) << 5) + \
  ((*RCol7 >> 27));
  // 'RandomColumn' Computing.
  return (int)((((double)RStrCol) / MAX_INT32) * YAxis);
}
// Random Neighbor Generator.
double RNbr (unsigned int *RNbr1, unsigned int *RNbr2, unsigned int *RNbr4, unsigned int *RNbr5, \
unsigned int *RNbr3, unsigned int *RNbr6, unsigned int *RNbr7, int NbrQuantity) {
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
  RStrNbr = ((*RNbr1 >> 26) << 26) + ((*RNbr6 >> 26) << 20) + ((*RNbr4 >> 27) << 15) + ((*RNbr3 >> 27) << 10) + ((*RNbr5 >> 27) << 5) + \
  ((*RNbr7 >> 27));
  // 'RandomNeighbor' Computing.
  return (int)((((double)RStrNbr) / MAX_INT32) * NbrQuantity);
}

// Arithmetic Function's.
int Absolute (int a) {
  int Mask = a >> (sizeof(int) * 8 - 1);
  return (a + Mask) ^ Mask;
}
double Square (double x) {
  return (x*x);
}
double Cubic (double y) {
  return (y*y*y);
}
double Quadric (double z) {
  return (z*z*z*z);
}