#ifndef TRANSIENTDSP_H
#define TRANSIENTDSP_H
#include "EnvelopeFollowerPeakHold.h"
#include "SmootherExponential.h"

void processTransientDSP(double in);

void uiProcessTransientDSP();

void initTransientDSP();

#endif