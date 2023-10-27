#ifndef TRANSIENTDSP_H
#define TRANSIENTDSP_H
#include "SmootherExponential.h"
#include "EnvelopeFollowerPeakHold.h"

extern SmootherExponential envSmoother;

void processTransientDSP(double in);

void initTransientDSP();

#endif