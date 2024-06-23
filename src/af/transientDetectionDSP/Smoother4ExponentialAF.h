#ifndef SMOOTHER4EXPONENTIALAF
#define SMOOTHER4EXPONENTIALAF

#include "inttypes.h"
#include <math.h>

#define CM (1.0 / (sqrt(pow(2.0, 1.0 / 4) - 1.0)))

typedef struct
{
    float state;
    float alpha_attack;
    float alpha_release;
} smootherTypeAf;

void resetAf(smootherTypeAf *smoother);

void setAttackAf(smootherTypeAf *smoother, float time_ms);

void setReleaseAf(smootherTypeAf *smoother, float time_ms);

float processAf(smootherTypeAf *smoother, float x);

void initAll4Af();

void resetAll4Af();

void setAttackAll4Af(float time_ms);

void setReleaseAll4Af(float time_ms);

float processEnvelopeAf(float x);

#endif