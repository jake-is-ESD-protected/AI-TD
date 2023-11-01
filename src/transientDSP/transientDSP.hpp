#pragma once
#include "EnvelopeFollowerPeakHold.h"
#include "SmootherExponential.h"

extern double lastVarGainValue;

void transientDSPinit();

void transientDSPprocess(double in);

void transientDSPuiProcess();
