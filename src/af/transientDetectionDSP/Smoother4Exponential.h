#pragma once

#include "inttypes.h"
#include <math.h>

#define CM (1.0 / (sqrt(pow(2.0, 1.0 / 4) - 1.0)))

typedef struct
{
    double state;
    double alpha_attack;
    double alpha_release;
} smootherType;

void reset(smootherType *smoother);

void setAttack(smootherType *smoother, double time_ms);

void setRelease(smootherType *smoother, double time_ms);

double process(smootherType *smoother, double x);

void initAll4();

void resetAll4();

void setAttackAll4(double time_ms);

void setReleaseAll4(double time_ms);

double processEnvelope(double x);