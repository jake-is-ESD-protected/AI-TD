#ifndef TRANSIENTDSP_H
#define TRANSIENTDSP_H
#include "SmootherExponential.h"
#include "EnvelopeFollowerPeakHold.h"

void processTransientDSP(double in);

void initTransientDSP();

void setAttackTime(double aT);

#endif