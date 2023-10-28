#ifndef TRANSIENTDSP_H
#define TRANSIENTDSP_H
#include "EnvelopeFollowerPeakHold.h"
#include "SmootherExponential.h"

extern double lastVarGainValue;

void processTransientDSP(double in);

void uiProcessTransientDSP();

void initTransientDSP();

#endif