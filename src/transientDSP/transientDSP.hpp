#pragma once
#include "EnvelopeFollowerPeakHold.h"
extern "C"
{
#include "SmootherExponential.h"
}

extern double lastVarGainValue;

void transientDSPinit();

void transientDSPprocess(double in);

void transientDSPuiProcess();
