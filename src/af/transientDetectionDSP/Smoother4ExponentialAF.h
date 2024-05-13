#ifndef SMOOTHER4EXPONENTIALAF
#define SMOOTHER4EXPONENTIALAF

#include "inttypes.h"
#include <math.h>

#define CM (1.0 / (sqrt(pow(2.0, 1.0 / 4) - 1.0)))

typedef struct
{
    double state;
    double alpha_attack;
    double alpha_release;
} smootherTypeAf;

void resetAf(smootherTypeAf *smoother);

void setAttackAf(smootherTypeAf *smoother, double time_ms);

void setReleaseAf(smootherTypeAf *smoother, double time_ms);

double processAf(smootherTypeAf *smoother, double x);

void initAll4Af();

void resetAll4Af();

void setAttackAll4Af(double time_ms);

void setReleaseAll4Af(double time_ms);

double processEnvelopeAf(double x);

#endif